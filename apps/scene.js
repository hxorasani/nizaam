/* scene graph, keeps track of scene in 3D space, groups, inheritance
 * 
 * objects have .sel for internal multi-selection like LINE points, BONE joints
 * 
 * hooks: prefix scene.
 *	draw				type, object, selected
 *	raycast				type, object, {x,y}
 *	lock				type, object, bool
 *	move|rotate|scale	type, object, {x,y,z}
 *	apply				type, object
 *	cancel				type, object
 *	
 * the first 0-99 types are reserved, your type should start at 100
 * e.g BONE = .type() -> 100+n
 * 
 * .get objects themselves, or their properties with pose or transform applied
 * .posed ignores transform
 * both accept a prop, if no prop is provided, they return transform or pose
 * this can help with primitive root props like .length .size
 * */

if (!scene) {
require('apps/core/reset.js'); // TODO require should check for dup
require('apps/core/v3.js'); // vector math, add neg copy ...
require('apps/core/colors.js'); // rgb array colors like c_black c_white ...
require('apps/core/hooks.js');
require('apps/core/text.js');
require('apps/core/show_keys.js');
require('apps/menu.js');
require('apps/list.js');


var scene, cameras;
;(function(){
	var xh = canvas.font_bounds(), prefix = 'scene.';
	var SPHERE = 0, CUBE = 1, LINE = 2;
	var MOVE = 1, SCALE = 2, ROTATE = 3;
	var MODE = ['textured', 'single', 'smooth', 'sharp', 'barycentric', 'tri_index'];

	var show_object_names = 1, xray_mode = 0;
	menu.add(0, 'scene');
	menu.add('names', 0, function () { show_object_names = !show_object_names; });
	menu.add('xray', 0, function () { xray_mode = !xray_mode; });

	list.hide();
	list.transparent(1);

	var camdata = {}, tid = 100, ortho = 1;
	var data = {}, guid = 0, lastside = 1, dragging = 0;
	var genuid = function () { return 'obj-'+guid++; };
	var rnd = Math.round;
	var typenames = ['sphere', 'cube', 'line'];
	var transform = { x: 0, y: 0, z: 0 };
	var affectpoints = 1;
	var mode = 0, modestr = '', lock = { x: 0, y: 0, z: 0 };
	var action = 0, offset = { x: 0, y: 0, z: 0 }, lastpointer, pointer_first;
	var selector = { x: 0, y: 0, w: 0, h: 0 };

	var updatelength = function () {
		scene.length = Object.keys(data).length;
		if (list) {
			list.empty(1);
			Object.keys(data).forEach(function (o, i) {
				var props = '';
				if (data[o].type == LINE) {
					props = (data[o].p||[]).length+' points';
				} else {
					props = data[o].name ? data[o].name : data[o].uid;
				}
				list.add(o, i+' '+typenames[data[o].type]+' '+props, function () {
					if (scene.includes(o))
						scene.deselect(o)
					else
						scene.select(o);
				});
			});
		}
	};
	var applytransform = function () {
		scene.selected.forEach(function (key) {
			var sel = scene.get(key);
			if (sel && mode == MOVE) {
				if (sel.type == LINE && affectpoints) {
					sel.p.forEach(function (p, i) {
						if (sel.sel && sel.sel.length && sel.sel.indexOf(i) > -1) {
							p[0] += transform.x;
							p[1] += transform.y;
							p[2] += transform.z;
						}
					});
				} else if (sel.type < 100) {
					sel.x += transform.x;
					sel.y += transform.y;
					sel.z += transform.z;
				}
			}
			if (sel) {
				hooks.run(prefix+'apply', sel);
			}
		});
		transform = { x: 0, y: 0, z: 0 };
		lock = { x: 0, y: 0, z: 0 };
		pointer_first = 0;
		mode = 0, modestr = '';
	};
	var previewtransform = function () {
		scene.selected.forEach(function (key) {
			var o = scene.get(key);
			if (o) {
//				hooks.run(prefix+'transform', mode, transform);
				if (mode == MOVE  ) hooks.run(prefix+'move'  , o, transform, lock);
				if (mode == SCALE ) hooks.run(prefix+'scale' , o, transform, lock);
				if (mode == ROTATE) hooks.run(prefix+'rotate', o, transform, lock);
			}
		});
	};
	var canceltransform = function () {
		scene.selected.forEach(function (key) {
			var sel = scene.get(key);
			if (sel) hooks.run(prefix+'cancel', sel);
		});
	};
	var selectorlogic = function (m) {
		var x = m.x, y = m.y;
		if (m.state == -1) {
			selector.x = x;
			selector.y = y;
			dragging = { x: x, y: y };
		}
		if (dragging) {
			if (x < dragging.x) selector.x = x, selector.w = Math.abs(x-dragging.x);
			else
				selector.w = x-dragging.x;
			if (y < dragging.y) selector.y = y, selector.h = Math.abs(y-dragging.y);
			else
				selector.h = y-dragging.y;

			if (selector.w > 5 && selector.h > 5) {
				canvas.save();
				var
//					side = selector.x > window.w/2 ? 0 : 1,
					side = 1,
					ox = 0, oy = 0;
				ox = side ? offset.x : offset.z;
				oy = offset.y;
//				canvas.translate(side ? ox+window.w/4 : ox+window.w/4*3, oy+window.h/2);
				canvas.translate(ox+window.w/2, oy+window.h/2);
				if (scene.zoom != 1) canvas.scale(scene.zoom, scene.zoom);
				var d2u = canvas.device2user(selector.x, selector.y);
				x = d2u.x; y = d2u.y; d2u.state = m.state;
				d2u.w = selector.w; d2u.h = selector.h;
				canvas.restore();
				// TODO BUG draw this d2u rect to debug it
				for (var i in data) {
					var o = data[i];
					var xz = side ? o.x : o.z;
					var contact = 0;
					if (o.type == CUBE) {
						var wd = side ? o.w : o.d;
						contact = matrix.overlap(xz, o.y, wd, o.h,
												x, y, selector.w, selector.h);
					} else
					if (o.type == SPHERE) {
						contact = matrix.overlap(xz-o.r, o.y-o.r, o.r*2, o.r*2,
												x, y, selector.w, selector.h);
					} else if (o.type >= 100) {
						recurse_selector(o, d2u);
					}
					
					if (contact) {
						o.state = 1;
						if (m.state == 1)
							scene.selected.push(i);
					}
				}
			}
		}
		if (m.state == 1) {
			dragging = 0;
			selector = { x: 0, y: 0, w: 0, h: 0 };
		}
	};
	var string_parents = function (o) {
		var str = '';
		if (o.parent) {
			var parent = scene.get(o.parent);
			str = string_parents(parent) + str;
		}
		if (str) str += ' > ';
		if (o.name) str += o.name;
		return str;
	};

	var drawinfo = function () {
		xh = canvas.font_bounds();

		var sel = scene.selected[0], str = '';
		if (scene.selected.length > 1) {
			str += (scene.selected.length+' objects selected')+'\n';
		} else
		if (sel) {
			var o = scene.get(sel);
			if (o) {
				var tmpinfo = string_parents(o);
				if (tmpinfo) str += (tmpinfo)+'\n';

				if (o.type == CUBE)
					str += ('x'+o.x+' y'+o.y+' z'+o.z+' w'+o.w+' h'+o.h+' d'+o.d)+'\n';
				else if (o.type == SPHERE)
					str += ('x'+o.x+' y'+o.y+' z'+o.z+' r'+o.r)+'\n';
				else {
					var tmpinfo = hooks.rununtilconsumed(prefix+'info', o);
					if (tmpinfo) str += (tmpinfo)+'\n';
				}
			}
		}

		str += ('pan x'+offset.x+' y'+offset.y+' z'+offset.z+' zoom '+rnd(scene.zoom*100)+'%')+'\n';
		if (mode) {
			str += (modestr+'x'+transform.x+' y'+transform.y+' z'+transform.z)+'\n';
		}

		str += ('lock x'+lock.x+' y'+lock.y+' z'+lock.z)+'\n';
		if (selector.w && selector.h) {
			str += ('selector x'+selector.x+' y'+selector.y+' w'+selector.w+' h'+selector.h)+'\n';
		}
		text(0, 16, str);

		var loc = camera.translation()	;
		var rot = camera.rotation()		;
		var sca = camera.scale()		;
		var foc = camera.focal()		;
		
		str = 'x'+sgl2unit(loc.x)+' y'+sgl2unit(loc.y)+' z'+sgl2unit(loc.z)+' loc\n'
			+ 'x'+sgl2deg(rot.x)+' y'+sgl2deg(rot.y)+' z'+sgl2deg(rot.z)+' rot\n'
			+ 'x'+sgl2unit(sca.x)+' y'+sgl2unit(sca.y)+' z'+sgl2unit(sca.z)+' sca\n'
			+ (foc ? sgl2unit(foc)+' focal length' : 'ortho')+'\n'
		
		str += '\n'+sgl.models().length+' models\n';

		var fog = sgl.fog() ? 'fog ' : '';
		var noise = sgl.noise() ? 'noise ' : '';
		var light = sgl.light() ? 'light ' : '';
		var wire = sgl.wire() ? 'wire ' : '';
		
		str += ''+fog+noise+light+wire+'\n'
			+ MODE[ sgl.get_mode() ] +' mode\n'
			;

		text(-4, 12, str, 2);
	};
	var sgl_line = function (x, y, c, d) {
		var a = sgl.to_screen(x, y, 0);
		var b = sgl.to_screen(c, d, 0);
		canvas.move_to(a.x+.5, a.y+.5), canvas.line(b.x+.5, b.y+.5);
	};
	var drawsubgrid = function (w, h) { // TODO fix overdraw :(
		sx = 50; sy = 50;
		w = w*2; h = h*2;
		canvas.save();
		canvas.lwn(.5,.5,.5);
		canvas.line_width(.15*scene.zoom);
		for (var y = .5; y <  h+sy; y+=sy) //canvas.move_to(-w, y), canvas.line(w, y);
			sgl_line(-w, y, w, y);
		for (var y = .5; y > -h   ; y-=sy) //canvas.move_to(-w, y), canvas.line(w, y);
			sgl_line(-w, y, w, y);
		
		for (var x = .5; x <  w+sx; x+=sx) //canvas.move_to(x, -h), canvas.line(x, h);
			sgl_line(x, -h, x, h);
		for (var x = .5; x > -w   ; x-=sx) //canvas.move_to(x, -h), canvas.line(x, h);
			sgl_line(x, -h, x, h);

		canvas.stroke();
		canvas.restore();
	};
	var drawgrid = function (w, h) {
		sx = 100; sy = 100;
		w = w*2; h = h*2;
		canvas.save();
		canvas.lwn(.5,.5,.5);
		canvas.line_width(.25*scene.zoom);
		for (var y = .5; y <  h+sy; y+=sy) //canvas.move_to(-w, y), canvas.line(w, y);
			sgl_line(-w, y, w, y);
		for (var y = .5; y > -h   ; y-=sy) //canvas.move_to(-w, y), canvas.line(w, y);
			sgl_line(-w, y, w, y);
		
		for (var x = .5; x <  w+sx; x+=sx) //canvas.move_to(x, -h), canvas.line(x, h);
			sgl_line(x, -h, x, h);
		for (var x = .5; x > -w   ; x-=sx) //canvas.move_to(x, -h), canvas.line(x, h);
			sgl_line(x, -h, x, h);
		canvas.stroke();

		canvas.line_width(1*scene.zoom);
//		canvas.move_to(-w, .5), canvas.line(w, .5);
		sgl_line(-w, .5, w, .5);
		canvas.lwn(1,0,0);
		canvas.stroke();
//		canvas.move_to(.5, -h), canvas.line(.5, h);
		sgl_line(.5, -h, .5, h);
		canvas.lwn(0,1,0);
		canvas.stroke();
		canvas.restore();
		drawsubgrid(w, h);
	};
	var draworigin = function (sel, x, y, z) {
		canvas.save();
//		if (scene.zoom != 1) canvas.scale(1/scene.zoom, 1/scene.zoom);
		var o = sgl.to_screen(x, y, z);
		canvas.circle(o.x, o.y, sel ? 2 : 1, 0, 360);
		canvas.lwn(1,1,1);
		canvas.fill();
		canvas.renew_path();
		canvas.restore();
	};
	var drawselector = function () {
		if (!selector.w || !selector.h) return;
		
		canvas.save();

		canvas.line_width(1);
		canvas.set_dash(5, 1);

		canvas.lwn(0, 0, 0);
		canvas.rect(selector.x-.5, selector.y-.5, selector.w, selector.h);
		canvas.stroke();

		canvas.lwn(1, 1, 1);
		canvas.rect(selector.x+.5, selector.y+.5, selector.w, selector.h);
		canvas.stroke();
		
		canvas.renew_path();
		canvas.restore();
	};

	var update_sgl_camera = function () {
		var U = 512;
		
		camera.translation(0, 0, -U*4, 0);
//		var factor = 0.877192982;
		var sca = U
//					* factor
				;
		camera.focal(ortho ? 0 : U*8);

		var foc = camera.focal();
		var delta = {z:-U*4};
		var z = scene.zoom*(sca);
		if (foc) {
			delta.z = -U*4 +z;
			camera.scale(U, U, U, 0);
		} else {
			camera.scale(z, z, z, 0);
		}

		delta.x = (-offset.x * 2) / scene.zoom;
		delta.y = ( offset.y * 2) / scene.zoom;
		camera.translation(delta.x, delta.y, delta.z, delta.w);
	};

	// .children logic
	var recurse_selector = function (o, m) {
		var contact; o.hover = 0;
		if (Object.keys(o.children).length) {
			for (var i in o.children) {
				contact = recurse_raycast(o.children[i], m);
				if (contact) break;
			}
		}
		if (!contact) {
			contact = hooks.rununtilconsumed(prefix+'selector', o, m);
			if (contact) {
				o.hover = 1;
				if (m.state == 1) scene.select(o.uid);
			}
		}

		return contact;
	};
	var recurse_raycast = function (o, m) {
		var contact; o.hover = 0;
		if (Object.keys(o.children).length && o.raycastable != 0) {
			for (var i in o.children) {
				contact = recurse_raycast(o.children[i], m);
				if (contact) break;
			}
		}
		if (!contact) {
			contact = hooks.rununtilconsumed(prefix+'raycast', o, m);
			if (contact) {
				contact = o.uid;
				if (m.state == 1) {
					if (m.shift) {
						if (scene.includes(o.uid))
							scene.deselect(o.uid);
						else
							scene.select(o.uid);
					} else {
						scene.deselect();
						scene.select(o.uid);
					}
				}
				if (!m.state) o.hover = 1;
			}
		}

		return contact;
	};
	var recurse_draw    = function (o, t) {
		hooks.run(prefix+'draw', o, scene.includes(o.uid));
		if (Object.keys(o.children).length) {
			for (var i in o.children) {
				recurse_draw( o.children[i] );
			}
		}
	};
	var flatten_parents = function (o) {
		var arr = [];
		if (o.parent) {
			var parent = scene.get(o.parent);
			arr = flatten_parents(parent).concat(arr);
		}
		if (o) arr.push(o);
		return arr;
	};
	var flatten_children = function (o) {
		var arr = [];
		if (o.children) {
			for (var i in o.children) {
				var child = o.children[i];
				arr = flatten_children(child).concat(arr);
			}
		}
		if (o) arr.push(o);
		return arr;
	};

	cameras = {
		add: function (uid, x, y, w, h) {
			camdata[uid] = {
				x: x,
				y: y,
				w: w,
				h: h,
			};
		},
		get: function (uid) {
			return camdata[uid];
		},
		remove: function (uid) {
			delete camdata[uid];
		},
	};

	var transform_or_pose_or_raw = function (prop) {
		var o = this;
		if (prop) {
			// first try to find it as a first level prop in .transform or .pose
			if (o.transform && o.transform[prop])
				return o.transform[prop];
			if (o.pose && o.pose[prop])
				return o.pose[prop];
			
			// then try the prop itself and see if it has .transform or .pose
			prop = o[prop];
			if (prop) {
				if (prop.transform) return prop.transform;
				if (prop.pose) return prop.pose;
				return prop;
			}
		} else
		return o.transform || o.pose || o;
	};
	var pose_or_raw = function (prop) { // can be merged with above func
		var o = this;
		if (prop) {
			// first try to find it as a first level prop in .pose
			if (o.pose && o.pose[prop])
				return o.pose[prop];
			
			// then try the prop itself and see if it has .pose
			prop = o[prop];
			if (prop) {
				if (prop.pose) return prop.pose;
				return prop;
			}
		} else
		return o.pose || o;
	};
	
	scene = {
		selected: [], // key strings, not index nums
		length: 0,
		type: function (name) { typenames[tid] = name; return tid++; },
		zoom: 1,
		set_zoom: function (v) {
			this.zoom = v;
			if (scene.zoom < .25) scene.zoom = .25;
			if (scene.zoom >   3) scene.zoom =   3;
			update_sgl_camera();
		},
		zoomin: function (shift) {
			if (scene.zoom < 3) scene.zoom += (shift ? .0625 : .03125);
			update_sgl_camera();
		},
		zoomout: function (shift) {
			if (scene.zoom > .25) scene.zoom -= (shift ? .0625 : .03125);
			update_sgl_camera();
		},
		includes: function (key) {
			return scene.selected.indexOf(key) > -1;
		},
		select: function (key) {
			var _ = this;
			if (!_.includes(key)) {
				var o = _.get(key), selectable = 1;
				
				if (o) {
					if (o.selectable == 0) selectable = 0;
					if (selectable) {
						var prevented = 0;
						if (isfun(o.on_select)) prevented = o.on_select(key);
						if (!prevented) _.selected.push(key);
					}
				}
			}
		},
		select_all: function (store) {
			var _ = this;
			if (!store) _.deselect();
			store = store || data;
			for (var i in store) {
				if (store[i].children) _.select_all(store[i].children);
				_.select(i);
			}
		},
		deselect: function (key) { // specific key OR all
			if (key) {
				var index = scene.selected.indexOf(key);
				scene.selected.splice(index, 1);
			} else
				scene.selected = [];
		},
		draw: function (side) {
			canvas.save();
			canvas.line_width(.5);
			
			side = 1;
			
			if (camera.focal() == 0)
			drawgrid(window.w, window.h);

			sgl.render();

			for (var i in data) {
				var o = data[i], sel = scene.includes(i);
				var xz = side ? o.x : o.z, y = o.y;
				if (!(affectpoints && o.type == LINE)) {
					if (sel) xz = side ? transform.x+o.x : transform.z+o.z;
					if (sel) y = transform.y+o.y;
				}

				var f = [].concat(o.f);
				if (xray_mode) f.push(.9);

				if (o.type == CUBE) {
					var wd = side ? o.w : o.d;
					var owd = wd/2;
					var oy = o.h/2;
					canvas.rect(xz-owd, y-oy, wd, o.h);
				} else
				if (o.type == SPHERE) {
					canvas.circle(xz, y, o.r, 0, 360);
				} else
				if (o.type == LINE) {
					canvas.lwn.apply(null, c_dsilver);
					canvas.line_width(.5);
					o.p.forEach(function (p, i) {
						canvas.save();
						var pxz = side ? p[0] : p[2];
						if (sel && o.sel && o.sel.length && o.sel[0] == i)
							canvas.lwn.apply(null, c_green);
						if (sel && o.sel && o.sel.length && o.sel[0] == i)
							canvas.circle(xz+transform.x+pxz, y+transform.y+p[1], 4, 0, 360);
						else
							canvas.circle(xz+pxz, y+p[1], 4, 0, 360);
						canvas.stroke();
						canvas.restore();
					});
					canvas.renew_path();
					canvas.line_width(2);
					var lx, ly;
					o.p.forEach(function (p, i) {
						var pxz = side ? p[0] : p[2], px, py;
						if (sel && affectpoints) {
							if (o.sel && o.sel.length && o.sel[0] == i)
							px = xz+transform.x+pxz, py = y+transform.y+p[1];
							else
							px = xz+pxz, py = y+p[1];
						} else
							px = xz+pxz, py = y+p[1];
						
						canvas.line(px, py);
					});
				} else {
					recurse_draw(o);
				}

				if (o.type < 100) { // only for internal types
					var luma = light.luma(o);
					if (f.length > 2) {
						var lumacolor = [].concat(f);
						lumacolor[0] = lumacolor[0] * luma;
						lumacolor[1] = lumacolor[1] * luma;
						lumacolor[2] = lumacolor[2] * luma;
						canvas.lwn.apply(null, lumacolor);
					}
					if (o.fill) canvas.fill(1);
//					text(xz, y-o.w/4, 'l'+luma);
				}
				
				canvas.save();
				var strclr = o.s || c_dgray;
				if (o.state && !sel) canvas.line_width(.5);
				if (o.state || sel) {
					canvas.lwn.apply(null, c_red);
					strclr = c_green;
					canvas.set_dash(5, 1);
					canvas.stroke(1);
					canvas.set_dash(5, 1, 5);
				}
				canvas.lwn.apply(null, strclr);
				canvas.stroke();
				canvas.restore();

			}

			for (var i in data) {
				var o = data[i], sel = scene.includes(i);
				if (o.no_info != 1) {
					var loc = o.get('location');
					draworigin(sel, loc.x, loc.y, loc.z);
				}
			}

			if (show_object_names) {
				canvas.font_size(12);
				for (var i in data) {
					var o = data[i];
					if (o.no_info != 1) {
						var loc = o.get('location');
						loc = sgl.to_screen(loc.x, loc.y, loc.z);
						text(loc.x, loc.y-30, i, 1);
					}
				}
			}

			canvas.renew_path();
			canvas.restore();

			drawinfo();
			drawselector();
			show_keys.draw();
		},
		add: function (uid, type, obj, parent) {
			var store_in = parent ? parent.children : data;

			var o = {
				uid: uid,
				type: type,
				fill: 1,
				sel: [],
				children: {},
				parent: parent ? parent.uid : 0,
				get: transform_or_pose_or_raw,
				posed: pose_or_raw,
			};

			if (obj) o = Object.assign(obj, o);
			
			store_in[uid] = o;

			updatelength();
			return o;
		},
		get: function (uid, parent) {
			var sel, store = (parent || data);
			for (var i in store) {
				if (i == uid) {
					sel = store[uid];
					break;
				} else {
					sel = scene.get(uid, store[i].children);
					if (sel) break;
				}
			}
			return sel;
		},
		remove: function (key) {
			var arr = key ? [key] : scene.selected;
			arr.forEach(function (key) {
				var sel = scene.get(key);
				if (sel) {
					var children = flatten_children(sel); // includes parent
					children.forEach(function (o) {
						hooks.run(prefix+'remove', o);
					});
					if (sel.parent) {
						var parent = scene.get(sel.parent);
						if (parent && parent.children && parent.children[key])
							delete parent.children[key];
					} else {
						delete data[key];
					}
				}
			});
			
			arr.forEach(function (key) {
				scene.deselect(key);
			});
			updatelength();
		},
		sphere: function (x, y, z, r, f) {
			var o = {};
			o.x = x || 0;
			o.y = y || 0;
			o.z = z || 0;
			o.r = r || 3;
			o.f = f || [1, 1, 1];
			o = scene.add(genuid(), SPHERE, o);
			return o;
		},
		cube: function (x, y, z, w, h, d, f) {
			var o = {};
			o.x = x || 0;
			o.y = y || 0;
			o.z = z || 0;
			o.w = w || 3;
			o.h = h || 3;
			o.d = d || 3;
			o.f = f || [1, 1, 1];
			o = scene.add(genuid(), CUBE, o);
			return o;
		},
		line: function (x, y, z, p, f, s) {
			var o = {};
			o.x = x || 0;
			o.y = y || 0;
			o.z = z || 0;
			o.p = p || [];
			o.f = f || [1, 1, 1];
			o.s = s || [1, 1, 1];
			o = scene.add(genuid(), LINE, o);
			return o;
		},
		offsets: function () {
			canvas.translate(offset.x+window.w/2, offset.y+window.h/2);
			if (scene.zoom != 1) canvas.scale(scene.zoom, scene.zoom);
		},
		transform: function (uid, mode, transform, copy) {
			var sel = data[uid];
			if (sel && mode == MOVE) {
				if (sel.type == LINE && affectpoints) {
					sel.p.forEach(function (p, i) {
						if (sel.sel && sel.sel.length && sel.sel.indexOf(i) > -1) {
							if (copy) {
								p[0]  = transform.x;
								p[1]  = transform.y;
								p[2]  = transform.z;
							} else {
								p[0] += transform.x;
								p[1] += transform.y;
								p[2] += transform.z;
							}
						}
					});
				} else {
					if (copy) {
						sel.x  = transform.x;
						sel.y  = transform.y;
						sel.z  = transform.z;
					} else {
						sel.x += transform.x;
						sel.y += transform.y;
						sel.z += transform.z;
					}
				}
			}
		},
		copy: function (uid, transform) { scene.transform(uid, MOVE, transform, 1); },
		move: function (uid, transform) { scene.transform(uid, MOVE, transform); },
		scale: function (uid, transform) { scene.transform(uid, SCALE, transform); },
		rotate: function (uid, transform) { scene.transform(uid, ROTATE, transform); },
		raycast: function (x, y, state, shift) {
			var yes;
			canvas.save();
			var side = 1;

			var d2u = canvas.device2user(x, y);
			canvas.restore();

			for (var i in data) {
				var o = data[i];
				o.state = 0;
			}
			var ray = sgl.raycast(d2u.x, d2u.y); // sgl raycast is expensive (!)
			var brk, objs = Object.keys(data);
			for (var i = objs.length-1; i > -1; --i) {
				if (brk) break;
				
				var key = objs[i];
				var o = data[ key ];
				var xz = side ? o.x : o.z;
				var contact = 0;
				if (o.type == CUBE) {
					var wd = side ? o.w : o.d;
					contact = matrix.contains(d2u.x, d2u.y, xz-wd/2, o.y-o.h/2, wd, o.h);
				} else if (o.type == SPHERE) {
					contact = matrix.distance(d2u.x, d2u.y, xz, o.y) <= o.r;
				} else if (o.type == LINE) {
					for (var j = 0; j < o.p.length; ++j) {
						var point = o.p[j];
						var pxz = side ? point[0] : point[1];
						contact = matrix.distance(d2u.x, d2u.y, o.x+pxz, o.y+point[1]) <= 5;
						if (contact) {
							if (state == -1)
								o.sel = [j];
							j = o.p.length;
						}
					}
				} else if (o.sgl_uids && o.raycastable != 0) {
					if (ray) {
						if ( o.sgl_uids.indexOf(ray.model) > -1 ) {
							contact = 1;
						}
					}
				} else {
					/*
					 * you can optionally handle sub-selection using this as well
					 * return 1 = your item got hit & normal multi-selection behavior follows
					 * return 2 = you're doing sub-selection & don't want default multi-selection behvior
					 * */
					contact = recurse_raycast(o, {x: d2u.x, y: d2u.y, state: state, shift: shift});
				}

				if (contact) {
					if (o.type < 100 || o.sgl_uids) {
						if (state == -1) {
							o.state = -1;
							if (shift && contact != 2) {
								if (scene.includes(key))
									scene.deselect(key);
								else
									scene.select(key);
							} else if (contact != 3) {// not handled while recursing
								scene.deselect();
								scene.select(key);
							}
							brk = 1;
						} else {
							o.state = 1;
							brk = 1;
						}
					}
					
					yes = 1;
				}
			}

			return yes;
		},
		pointer: function (m) {
			var yes;
			yes = show_keys.pointer(m);
			
			var lockxz;
			if (m.alt && m.shift && m.state == -1) action = MOVE, yes = 1;

			if (action || mode) {
				lockxz = lastside ? lock.x : lock.z;
				if (!pointer_first) {
					var tmp = sgl.to_screen(m.x, m.y, 0);
					pointer_first = v3(tmp.x, tmp.y, 0);
				}
			}
			
			if (action == MOVE && lastpointer) {
				if (!lockxz)
					if (lastside) offset.x -= lastpointer.x-m.x;
					else offset.z -= lastpointer.x-m.x;
				
				if (!lock.y) offset.y -= lastpointer.y-m.y;
				
				update_sgl_camera();
				yes = 1;
			} else if (mode && pointer_first) {
				canvas.save();
				if (scene.zoom != 1) canvas.scale(scene.zoom, scene.zoom);
				var tmp = sgl.to_screen(m.x, m.y, 0);
				
				var d2u = canvas.device2user(pointer_first.x-tmp.x, pointer_first.y-tmp.y);
				var x, y;
				if (mode != ROTATE) x = rnd(d2u.y), y = rnd(d2u.x);
				else y = rnd(d2u.x), x = rnd(d2u.y);
				canvas.restore();
				
				if (!m.ctrl) {
					if (!lockxz)
						transform.x = -y;
					
					if (!lock.y) transform.y = -x;
				} else {
					if (!lock.x) transform.z = -y;
				}
				yes = 1;
				previewtransform();
			} else {
//				lastside = m.x > window.w/2 ? 0 : 1;
				lastside = 1;
				if (m.w) {
					if (m.w == -1) scene.zoomin (m.shift);
					if (m.w ==  1) scene.zoomout(m.shift);
					yes = 1;
				} else {
					yes = scene.raycast(m.x, m.y, m.n == 1 ? m.state : undefined, m.shift);
					if (!m.shift && !yes && m.n == 1 && !mode) scene.deselect(), yes = 1; // click outside
				}
			}

			if (!action && mode && m.state == 1) applytransform(), yes = 1;

			// panning is released here to allow transforming while panning!
			if (((!m.alt && !m.shift) || m.state == 1) && action == MOVE) action = 0, yes = 1;
			
			if (!action && !mode) selectorlogic(m);

			lastpointer = m;
			return yes;
		},
		keyboard: function (m) {
			var yes, k = keyism[m.key], t = m.text, step = m.alt ? 1 : 5;
			show_keys.keyboard(m);
			var sel = scene.selected.length;
			if (m.state) { // press or repeat
				if (k == 'equal'	 ) scene.zoomin(m.shift) , yes = 1;
				if (k == 'minus'	 ) scene.zoomout(m.shift), yes = 1;

				if (action) {
					if (k == 'left'  && !lock.x) !m.ctrl ? offset.x-=step : offset.z-=step, yes = 1;
					if (k == 'right' && !lock.x) !m.ctrl ? offset.x+=step : offset.z+=step, yes = 1;
					if (k == 'up'    && !lock.y) offset.y-=step, yes = 1;
					if (k == 'down'  && !lock.y) offset.y+=step, yes = 1;
					if (yes) update_sgl_camera();
				} else
				if (mode && sel) {
					if (k == 'left'  && !lock.x) transform.x-=step, yes = 1;
					if (k == 'right' && !lock.x) transform.x+=step, yes = 1;
					if (k == 'up'    && !lock.y) !m.ctrl ? transform.y+=step : transform.z-=step, yes = 1;
					if (k == 'down'  && !lock.y) !m.ctrl ? transform.y-=step : transform.z+=step, yes = 1;
					if (yes) previewtransform();
				}
			}
			if (!m.state) { // release
				if (k == 'backspace')
					offset = { x: 0, y: 0, z: 0 }, update_sgl_camera(), yes = 1;
				if (k == '0')
					scene.zoom = 1, update_sgl_camera(), yes = 1;
				if (k == 'kp5')
					ortho = !ortho, update_sgl_camera(), yes = 1;
				if (k == 'a' && m.ctrl && !m.shift) scene.select_all(), yes = 1;
				if (k == 'a' && m.ctrl && m.shift) scene.deselect(), yes = 1;
				if (m.shift) {
					if (k == 'n') sgl.noise(-1), yes = 1;
					if (k == 'f') sgl.fog  (-1), yes = 1;
					if (k == 'l') sgl.light(-1), yes = 1;
					if (k == 'z') sgl.wire (-1), yes = 1;
					if (k == 'm') sgl.set_mode(), yes = 1;
				}
				if (sel) {
					if (mode) {
						if (k == 'x') {
							lock.x = 0;
							lock.z = lock.y ? 0 : 1;
							lock.y = lock.y ? 0 : 1;
							transform.z = transform.y = 0;
							yes = 1;
						}
						if (k == 'y') {
							lock.y = 0;
							lock.z = lock.x ? 0 : 1;
							lock.x = lock.x ? 0 : 1;
							transform.z = transform.x = 0;
							yes = 1;
						}
						if (k == 'z') {
							lock.z = 0;
							lock.y = lock.x ? 0 : 1;
							lock.x = lock.x ? 0 : 1;
							transform.y = transform.x = 0;
							yes = 1;
						}
						if (yes && 'xyz'.indexOf(k) > -1) previewtransform();
					}
					if (k == 'delete') scene.remove(), yes = 1;
					if (k == 'enter') {
						if (mode) applytransform();
						yes = 1;
					}
					if (k == 'esc') {
						transform = v3(0, 0, 0);
						lock = v3(0, 0, 0);
						pointer_first = 0;
						if (mode) mode = 0, modestr = '', yes = 1;
						else scene.deselect(), yes = 1;
						canceltransform();
					}
					if (k == 'g') mode = MOVE,	modestr = 'MOVE ' ,yes = 1;
					if (k == 's') mode = SCALE,	modestr = 'SCALE ' ,yes = 1;
					if (k == 'r') mode = ROTATE,modestr = 'ROTATE ' ,yes = 1;

					if (k == 'p' && m.alt)
						scene.selected.forEach(function (key) {
							var sel = scene.get(key);
							if (sel) {
								yes = hooks.rununtilconsumed(prefix+'reset', sel);
							}
						});

					if (!yes) {
						scene.selected.forEach(function (key) {
							var sel = scene.get(key);
							if (sel) {
								yes = hooks.rununtilconsumed(prefix+'pressed', sel, k, m);
							}
						});
					}
				}
			}
			return yes;
		},
	};
	
	update_sgl_camera();
})();
}



/*

//	text(0, 0, 'front -- x ++');
//	text(window.w/2, 0, 'side -- z ++');
	
//	canvas.lwn(1, 0, 0);
//	canvas.move_to(window.w/2, 0);
//	canvas.line(window.w/2, window.h);
//	canvas.stroke();


// torso
scene.sphere( 0, 90,  0, 60, c_gray );
// head
//scene.sphere( 0,  0,  0, 50, c_gray );
scene.cube( 0,  0,  0, 100, 100, 100, c_gray );
// eyes
	// pupils
scene.sphere(-25, 0, 30, 20, c_white);
	scene.sphere(-25, 0, 35, 10, c_black);
scene.sphere( 25, 0, 30, 20, c_white);
	scene.sphere( 25, 0, 35, 10, c_black);
// hands
scene.sphere(-55,120, 30, 10, c_gray);
scene.sphere( 55,120, 30, 10, c_gray);
// feet
scene.sphere(-30,220, 0, 10, c_gray);
scene.sphere( 30,220, 0, 10, c_gray);
// testing
scene.line( 0, 20, 0, [
	[0, 0, 0, 0], [0, 11, 11, 1], [0, 0, 11, 1]
], c_gray);
*/

