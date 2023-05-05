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

var MOVE = 1, SCALE = 2, ROTATE = 3;
if (!scene) {
require('apps/core.js');

var scene, Collection, Collections, two_dee_cam;
;(function(){
var prefix = 'scene.';
var SPHERE = 0, CUBE = 1, LINE = 2;
var MODE = ['textured', 'single', 'smooth', 'sharp', 'barycentric', 'tri_index'];

var show_object_names = 1, xray_mode = 0;
hooks.set('menu.ready', function () {
	menu.add(0, 'scene');
	menu.add('names', 0, function () { show_object_names = !show_object_names; });
	menu.add('xray', 0, function () { xray_mode = !xray_mode; });
});

var tid = 100, data = {}, guid = 1, lastside = 1, dragging = 0;
var genuid = function () { return guid++; }, rnd = Math.round;
var typenames = ['sphere', 'cube', 'line'];
var transform = { x: 0, y: 0, z: 0 };
var affectpoints = 1;
var mode = 0, modestr = ['', 'MOVE', 'SCALE', 'ROTATE'],
	lock = { x: 0, y: 0, z: 0 };
var default_offset = v3(0);
var action = 0, offset = shallowcopy(default_offset), lastpointer, pointer_first;
var selector = { x: 0, y: 0, w: 0, h: 0 };
var global_hits, global_hovers = [], global_helds = [];
var last_time = 0, frames_passed = 0, fps = 0, picked,
	commands = '', history = [], history_cursor = 0, results = '';

function updatelength(parent) {
	var store = parent || data;
	if (!store) return 0;
	var len = Object.keys(store).length;
	if (len) {
		for (var i in store)
			if (store[i].children)
				len += updatelength(store[i].children);
	}
	if (!parent) scene.length = len;
	return len;
};
function applytransform() {
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
	mode = 0;
};
function previewtransform() {
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
function canceltransform() {
	scene.selected.forEach(function (key) {
		var sel = scene.get(key);
		if (sel) hooks.run(prefix+'cancel', sel);
	});
};
function selectorlogic(m) {
	return;
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
			var
//					side = selector.x > window.w/2 ? 0 : 1,
				side = 1,
				ox = 0, oy = 0;
			ox = side ? offset.x : offset.z;
			oy = offset.y;
//				canvas.translate(side ? ox+window.w/4 : ox+window.w/4*3, oy+window.h/2);
			canvas.translate(ox+window.w/2, oy+window.h/2);
			var d2u = canvas.device2user(selector.x, selector.y);
			x = d2u.x; y = d2u.y; d2u.state = m.state;
			d2u.w = selector.w; d2u.h = selector.h;
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
function string_parents(o) {
	var str = '';
	if (o.parent) {
		var parent = scene.get(o.parent);
		str = string_parents(parent) + str;
	}
	if (str) str += ' > ';
	if (o.name) str += String(o.name).replace(/\n/g, ' ');
	return str;
};

function time_now() { return new Date().getTime(); }
function get_fps() {
	frames_passed++;
	var diff = time_now()-last_time;
	if (diff >= 1000) {
		fps = frames_passed;
		frames_passed = 0;
		last_time = time_now();
	}
	return fps;
};
function drawinfo () {
	if (!scene.show_info) return;
	var date = new Date();
	var sel = scene.selected[0],
		str = date.toLocaleTimeString()+'.'+date.getMilliseconds()+'\n';
	
	str += get_fps()+'fps\n';
	
	if (scene.length) str += scene.length+' object'+(scene.length ? 's' : '')+'\n';
	
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

	if (scene.edit_mode) { str += 'edit\n'; }
	if (mode) {
		str += (modestr[mode]+' x'+transform.x+' y'+transform.y+' z'+transform.z)+'\n';
	}

	if (lock.x || lock.y || lock.z) str += ('lock x'+lock.x+' y'+lock.y+' z'+lock.z)+'\n';

	if (selector.w && selector.h) {
		str += ('selector x'+selector.x+' y'+selector.y+' w'+selector.w+' h'+selector.h)+'\n';
	}

	text(5, 18, str);

	if (scene.console_mode) {
		var mb = text(0, window.h-200, results+'\nconsole mode\n'+commands, 0, 1);
		text(0, window.h-mb.h, results+'\nconsole mode ('+history.length+')\n'+commands+'|');
	}

	str = '';
	var cam = get_active_camera();
	if (cam) {
		str +=  'loc '+vec3.str(v3.precision_float_array(cam.location))
			+ '\ntgt '+vec3.str(v3.precision_float_array(cam.target  ));
	}
	if (global_hits && global_hits.length) {
		str += '\nhits '+global_hits.length+'\n';
		global_hits.forEach(function (o, i) {
			str += stringify(o.name)+' '+o.location.z+'\n';
		});
	}
	text(-5, 18, str, 2);
};
function drawselector() {
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

// .children logic
function recurse_selector(o, m) {
	var contact;
	if (Object.keys(o.children).length) {
		for (var i in o.children) {
			contact = recurse_raycast(o.children[i], m);
			if (contact) break;
		}
	}
	if (!contact) {
		contact = hooks.rununtilconsumed(prefix+'selector', o, m);
		if (contact) {
			if (m.state == 1) scene.select(o.guid);
		}
	}

	return contact;
};
function recurse_raycast(o, m, hits) {
	var contact, store = isundef(o.guid) ? o : o.children;
	var length = store ? Object.keys(store).length : 0;
	if (length) {
		for (var i in store) {
			var o = store[i];
			if (o.visible != 0) {
				recurse_raycast(store[i], m, hits);
				if (o.raycastable != 0) {
					contact = hooks.rununtilconsumed(prefix+'raycast', o, m);
					if (contact) {
						hits.push(o);
					}
				}
			}
		}
	}
};
function recurse_draw(o, parent, frames) {
	if (o.visible == 0) return frames;
	var sel = scene.includes(o.guid);
	if (o.no_info != 1)
	if ((show_object_names && sel) || o.info) {
		var loc = o.get('location');
		var cam = get_active_camera();
		var q = cam.unproject(loc.x, loc.y+10, loc.z);
		text(q.x, q.y, o.name + (o.info ? '\n'+ o.info : ''), 1);
	}
	frames += hooks.rununtilconsumed(prefix+'draw', o, sel) || 0;
	var keys = Object.keys(o.children);
	if (keys.length) {
		keys.forEach(function (key, i) {
			var child = o.children[key];
			child.head = o.children[keys[0]];
			child.prev = o.children[keys[i-1]];
			child.next = o.children[keys[i+1]];
			frames = recurse_draw( child, o, frames );
		});
	}
	return frames;
};
function flatten_parents(o) {
	var arr = [];
	if (o.parent) {
		var parent = scene.get(o.parent);
		arr = flatten_parents(parent).concat(arr);
	}
	if (o) arr.push(o);
	return arr;
};
function flatten_children(o) {
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

function transform_or_pose_or_raw(prop) {
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
function pose_or_raw(prop) { // can be merged with above func
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
function first_selected() {
	if (scene.selected.length) return scene.get( scene.selected[0] );
}

var collections = {};
Collections = {
	get: function (uid, parent) {
		var sel, store = (parent || collections);
		for (var i in store) {
			if (i == uid) {
				sel = store[uid];
				break;
			} else {
				sel = Collections.get(uid, store[i].children);
				if (sel) break;
			}
		}
		return sel;
	},
};
Collection = function (name) {
	var o = {
		guid: name || guid(),
		name: name,
	};
	collections[o.guid] = o;
	return o;
};

// this collections is rendered as 2D objects in ortho view
var two_dee = Collection('2d');
hooks.set('camera.ready', function () {
	two_dee_cam = Camera( 'two_dee' );
	two_dee_cam.projection = CAMERA_ORTHOGRAPHIC;
});

scene = {
	selected: [], // key strings, not index nums
	length: 0,
	console_mode: 0,
	show_info: 1,
	wire_mode: 0,
	edit_mode: 0,
	type: function (name) { typenames[tid] = name; return tid++; },
	set_wire: function (v) {
		if (isundef(v)) v = !this.wire_mode;
		this.wire_mode = v;
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
		if (_.edit_mode) {
			var o = first_selected();
			if (o) hooks.rununtilconsumed(prefix+'select_all', o);
			return;
		}
		if (!store) _.deselect();
		store = store || data;
		for (var i in store) {
			if (store[i].children) _.select_all(store[i].children);
			_.select(i);
		}
	},
	deselect: function (key, except) { // specific key OR all
		var _ = this;
		if (_.edit_mode) {
			var o = first_selected();
			if (o) hooks.rununtilconsumed(prefix+'deselect', o);
			return;
		}
		if (key) {
			var index = _.selected.indexOf(key);
			var o = _.get(key);
			if (o) if (isfun(o.on_deselect)) o.on_deselect(key);
			_.selected.splice(index, 1);
		} else {
			_.selected.forEach(function (key) {
				if (except == key) return;
				var o = _.get(key);
				if (o) if (isfun(o.on_deselect)) o.on_deselect(key);
			});
			_.selected = _.includes(except) ? [except] : [];
		}
	},
	frame_start: function () {
		sgl.uniform_1ui(uniforms.draw_id, 0);
		sgl.uniform_1ui(uniforms.object_id, 0);
		sgl.clear_color(0, 0, 0, 0);
		sgl.clear(sgl.GL_DEPTH_BUFFER_BIT | sgl.GL_COLOR_BUFFER_BIT);
		nuk.begin_canvas("scene-canvas", 0, 0, window.w, window.h, nuk.NO_INPUT);
	},
	frame_end: function () {
		drawinfo();
		drawselector();
		show_keys.draw();

		nuk.end_canvas();
	},
	draw2d: function (frames) {
		scene.two_dee = 1;
		two_dee_cam.activate();
		two_dee_cam.apply();
		sgl.clear(sgl.GL_DEPTH_BUFFER_BIT);
		for (var i in data) {
			var o = data[i], sel = scene.includes(i);
			if (o.collection && o.collection.guid == two_dee.guid)
			frames += recurse_draw(o, 0, frames);
		}
		scene.two_dee = 0;
		return frames;
	},
	draw: function () {
		default_cam.activate();
		default_cam.apply();
		var frames = 0;

		for (var i in data) {
			var o = data[i], sel = scene.includes(i);
			if (isundef(o.collection) || o.collection.guid != two_dee.guid) {
				frames += recurse_draw(o, 0, frames);
			}
		}

		frames = this.draw2d(frames);

		frames += show_input(); // TODO move to frame end

		default_cam.activate();
		default_cam.apply();

		return frames;
	},
	add: function (name, type, obj, parent) {
		var store_in = parent ? parent.children : data;

		var o = {
			name: name, // unique per module, more like a nickname
			guid: genuid(), // used for selection, unique globally
			type: type,
			fill: 1,
			sel: [],
			children: {},
			parent: parent ? parent.guid : 0,
			get: transform_or_pose_or_raw,
			posed: pose_or_raw,
		};

		if (obj) o = Object.assign(obj, o);
		
		store_in[o.guid] = o;

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
				if (!scene.edit_mode) {
					if (sel.parent) {
						var parent = scene.get(sel.parent);
						if (parent && parent.children && parent.children[key])
							delete parent.children[key];
					} else {
						delete data[key];
					}
				}
			}
		});
		
		if (!scene.edit_mode)
		scene.selected = arr.filter(function (key) {
			scene.deselect(key);
			return 0;
		});
		
		if (scene.selected.length == 0) scene.edit_mode = 0;
		
		updatelength();
	},
	escape: function () {
		var yes;
		transform = v3(0, 0, 0);
		lock = v3(0, 0, 0);
		pointer_first = 0;
		if (mode) mode = 0, yes = 1;
		else if (!scene.edit_mode) scene.deselect(), yes = 1;
		canceltransform();
		return yes;
	},
	to_mat4: function (o, mat) { // get cached parent-aware transforms
		var l = o.get('location');
		var r = o.get('rotation');
		var s = o.get('scale');
		mat = mat || mat4.create();
		var parent = o.parent ? scene.get(o.parent) : 0;
		if (parent) scene.to_mat4(parent, mat);
		mat4.translate(mat, mat, [l.x, l.y, l.z]);
		mat4.rotateX  (mat, mat, matrix.to_radians(r.x));
		mat4.rotateY  (mat, mat, matrix.to_radians(r.y));
		mat4.rotateZ  (mat, mat, matrix.to_radians(r.z));
		if (s) mat4.scale  (mat, mat, [s.x, s.y, s.z]);
		return mat;
	},
	reparent: function (parent, child) { // {}, {}
		var _ = this;
		// find child's store
		// delete it from there
		if (child.parent) {
			var old_parent = _.get(child.parent);
			delete old_parent.children[child.guid];
		} else {
			delete data[child.guid];
		}
		// insert it under parent's children
		parent.children[child.guid] = child;
		child.parent = parent.guid;
	},
	raycast: function (x, y, state, shift) {
		var yes;
		sgl.bind(sgl.GL_FRAMEBUFFER, glsl.pb);
		sgl.read_buffer(sgl.GL_COLOR_ATTACHMENT0);
		picked = sgl.pixel(x, y);
		sgl.read_buffer(sgl.GL_NONE);
		sgl.bind(sgl.GL_FRAMEBUFFER, 0);

		if (scene.edit_mode || action || mode) {
			if (scene.edit_mode) {
				if (scene.selected.length) {
					var o = scene.get( scene.selected[0] );
					var m = { x: x, y: y, state: state, shift: shift };
					yes = hooks.rununtilconsumed(prefix+'raycast', o, m, picked);
				}
			}
		}
		var key;
		global_hits = [];
		
		/*
		 * you can optionally handle sub-selection using this as well
		 * return 1 = your item got hit & normal multi-selection behavior follows
		 * return 2 = you're doing sub-selection & don't want default multi-selection behvior
		 * */
//		recurse_raycast(data, {x: x, y: y, state: state, shift: shift}, global_hits);
		
		if (picked && picked.r) {
			var obj = scene.get( picked.r );
			if (obj) {
				if (obj.reroute_events) {
					var p = scene.get( obj.parent );
					if (p) obj = p;
				}
				global_hits.push( obj );
			}
		}

		var global_hits_indices = {};
		global_hits.forEach(function (o, i) {
			global_hits_indices[o.guid] = i; // level
		});
		global_hovers = global_hovers.filter(function (guid, i) {
			var level = global_hits_indices[guid];
			if (isundef(level)) { // not in global_hits
				var o = scene.get(guid);
				if (o && !isundef(o.hover)) {
					o.hover = undefined;
					// if held, change to 1 to remember pressed
					o.state = o.state == -1 ? 1 : undefined;
					if (isfun(o.on_leave)) o.on_leave();
					return 0;
				}
			}
			return 1;
		});
		if (global_hits.length) {
			global_hits.sort(function (a, b) {
				return b.get('location').z - a.get('location').z;
			});
			var o = global_hits[0];
			if (state == -1) {
				o.state = -1;
				if (global_helds.indexOf(o.guid) == -1)
					global_helds.push(o.guid);
				if (isfun(o.on_press)) o.on_press();
				if (shift && o.contact != 2) {
					if (scene.includes(o.guid))
						scene.deselect(o.guid);
					else
						scene.select(o.guid);
				} else if (o.contact != 3) {// not handled while recursing
					scene.deselect(0, o.guid);
					scene.select(o.guid);
				}
			} else {
				if (o.state == -1 && state == 1) {
					o.state = undefined;
					if (isfun(o.on_release)) o.on_release();
				}
				global_hits.forEach(function (o, i) {
					o.hover = i;
					// restore on reentry
					if (o.state == 1) {
						o.state = -1;
						if (global_helds.indexOf(o.guid) == -1)
							global_helds.push(o.guid);
					}
					if (global_hovers.indexOf(o.guid) == -1) {
						global_hovers.push(o.guid);
						if (isfun(o.on_hover)) o.on_hover(i);
					}
				});
			}
			yes = 1;
		}
		global_helds.forEach(function (o, i) {
			o = scene.get(o);
			if (state == 1 && o) {
				o.state = undefined;
				global_helds.splice(i);
			}
		});

		return yes;
	},
	pointer: function (m) {
		var yes;
		if (scene.show_info) yes = show_keys.pointer(m);
		
		var lockxz, cam = get_active_camera();

		if (action || mode) {
			if (!pointer_first) {
				offset = v3(0,0,0);
				if (cam) {
					pointer_first = v3( cam.project(m.x, m.y) );
				}
			}
		}
		
		if (mode && pointer_first) {
			var gran = 10, x, y, z;
			var cur = v3( cam.project(m.x, m.y) );
			var d2u = {
				x: pointer_first.x-cur.x,
				y: pointer_first.y-cur.y,
				z: pointer_first.z-cur.z,
			};
			if (mode == ROTATE) {
				gran = -0.8;
				y = d2u.x; x = d2u.y;
			} else {
				x = d2u.x; y = -d2u.y;
			}
			z = d2u.z;
			if (cam.projection == CAMERA_ORTHOGRAPHIC) {
				gran /= vec3.dot([1,1,1], cam.location) / cam.fovy * 15;
			} else {
				gran /= vec3.dot([1,1,1], cam.location) / cam.fovy * 1;
			}
			if (mode == SCALE) {
				gran = 0.01;
				var s = -x * gran;
				transform = v3(s,s,s);
			} else {
				if (!lock.x) transform.x = -x / gran;
				if (!lock.y) transform.y =  y / gran;
				if (!lock.z) transform.z = -z / gran;
			}
			if (m.ctrl) { // grid lock
				transform.x = Math.round(transform.x);
				transform.y = Math.round(transform.y);
				transform.z = Math.round(transform.z);
			}
			v3.precision(transform);
			transform.gran = gran;
			yes = 1;
			previewtransform();
		} else {
			lastside = 1;
			if (!m.w) {
				yes = scene.raycast(m.x, m.y, m.n == 1 ? m.state : undefined, m.shift);
				if (scene.edit_mode) {
					// TODO selection limited to active object
					return yes;
				} else if (!m.shift && !yes && m.n == 1 && !mode && m.state == -1)
					scene.deselect(), yes = 1; // click outside
			}
		}

		if (!action && mode && m.state == 1) applytransform(), yes = 1;
		
		if (!action && !mode) selectorlogic(m);

		lastpointer = m;
		return yes;
	},
};
hooks.set('nizaam.keyboard', function (m, k, held) {
	var yes, t = m.text, step = m.alt ? .02 : (m.shift ? .2 : .1);
	if (scene.show_info) show_keys.keyboard(m);
	var sel = scene.selected.length;
	if (!m.state) { // release
		if (k == 'f12') {
			scene.console_mode = !scene.console_mode;
			yes = 1;
		}
		if (scene.console_mode) return yes;
		if (m.ctrl && k == 'e') {
			if (sel)
			scene.edit_mode = !scene.edit_mode;
			yes = 1;
		}
		if (k == 'a' && m.ctrl && !m.shift) scene.select_all(), yes = 1;
		if (k == 'a' && m.ctrl &&  m.shift) scene.deselect(), yes = 1;
		if (m.ctrl) {
			if (k == 'h') {
				composer.cursor( composer.cursor() ? 0 : 1 );
				yes = 1;
			}
		}
		if (m.shift) {
			if (k == 'n') scene.noise(-1), yes = 1;
			if (k == 'f') scene.fog  (-1), yes = 1;
			if (k == 'l') scene.light(-1), yes = 1;
			if (k == 'z') scene.set_wire(), yes = 1;
			if (k == 'm') scene.set_mode(), yes = 1;
		}
		if (sel && !m.shift) {
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
			if (!mode && (k == 'delete' || k == 'x')) scene.remove(), yes = 1;
			if (k == 'enter') {
				if (mode) applytransform();
				yes = 1;
			}
			if (k == 'esc') {
				yes = scene.escape();
			}
			if (k == 'h') {
				scene.selected.forEach(function (key) {
					var sel = scene.get(key);
					if (sel) {
						sel.visible = !sel.visible;
					}
				});
				yes = 1;
			}
			if (!m.ctrl) {
				if (k == 'g') mode = MOVE	, yes = 1;
				if (k == 's') mode = SCALE	, yes = 1;
				if (k == 'r') mode = ROTATE , yes = 1;
			}
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
						yes = hooks.rununtilconsumed(prefix+'pressed', sel, k, m, held);
					}
				});
			}
		}
	}
	if (!scene.console_mode && m.state) { // press or repeat
		if (mode && sel) {
			if (mode == ROTATE) step *= 10;
			else if (mode == MOVE) transform.gran = 20;
			if (held['left'    ] && !lock.x) transform.x -=step, yes = 1;
			if (held['right'   ] && !lock.x) transform.x +=step, yes = 1;
			if (held['up'      ] && !lock.y) transform.y +=step, yes = 1;
			if (held['down'    ] && !lock.y) transform.y -=step, yes = 1;
			if (held['pageup'  ] && !lock.z) transform.z -=step, yes = 1;
			if (held['pagedown'] && !lock.z) transform.z +=step, yes = 1;
			if (yes) v3.precision(transform), previewtransform();
		}
	}
	if (!scene.console_mode && scene.edit_mode) { // send all input to selected objects
		if (!yes)
		scene.selected.forEach(function (key) {
			var sel = scene.get(key);
			if (sel) {
				yes = hooks.rununtilconsumed(prefix+'pressed', sel, k, m, held);
			}
		});
		return yes;
	}
	if (scene.console_mode) {
		if (!m.state || m.state || m.text.length) {
			if (k == 'delete') {
				if (commands.length) {
					commands = commands.splice(commands.length+1, 1, '');
					yes = 1;
				}
			} else if (k == 'backspace') {
				if (commands.length) {
					commands = commands.splice(commands.length-1, 1, '');
					yes = 1;
				}
			} else if (k == 'enter' && !m.shift) {
				if (history[history.length-1] != commands) {
					if (history.length > 99) history = history.slice(1);
					history.push(commands);
					history_cursor++;
				}
				try { results = eval(commands); } catch (e) { results = e; }
				pretty( results );
				commands = ''; yes = 1;
			} else if (k == 'up') {
				if (history_cursor && history.length) {
					history_cursor--;
					commands = history[history_cursor];
				}
			} else if (k == 'down') {
				if (history_cursor < history.length) {
					history_cursor++;
					commands = history[history_cursor] || '';
				}
			} else if (k == 'enter') {
				commands += ('\n'); yes = 1;
			} else if (k == 'tab') {
				var where = global, cmd = commands;
				var splat = commands.split('.');
				if (commands.indexOf('.') > -1) {
					var tmp = global[splat[0]];
					if (tmp) {
						where = tmp;
						cmd = splat[1];
	//					for (var i = 0; i < splat.length; ++i) {
	//						var o = splat[i];
	//						if (tmp[o]) tmp = tmp[o];
	//						else break;
	//					}
	//					if (tmp) where = tmp;
					}
				}
				var last, matches = [];
				for (var i in where) {
					if (matches.length > 40) break;
					if (i.startsWith(cmd) || cmd == '') {
						matches.push(i);
						last = i;
					}
				}
				results = matches.join('\n');
				pretty( matches );
				if (matches.length == 1 && splat.length) {
					if (splat.length > 1)
						commands = splat.slice(0, -1).join('.')+'.'+last;
					else
						commands = last;
				}
			} else if (m.text) {
				commands += m.text; yes = 1;
			}
		}
	}
	return yes;
});

})();
}


