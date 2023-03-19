/* TODO
 * there should be two modes, edit mode and pose mode
 * in pose mode, moving a bone including root is IK based
 * in edit mode, moving a bone affects the location
 * 
 * pose mode:
 * move a bone, the chain above and below should compensate & recalc
 * it should be done in world space and then translated into the least pose
 * this means the root can also do IK and move as you animate it
 * without having to move the entire object in the world space
 * 
 * this should allow, moving the legs and the hips are dragged to them
 * moving the hips and the legs drag to them to compensate
 * moving the calves high up and first the hips leap to them & then the chest...
 * 
 * migrate .get .set to scene api in a smooth compat way
 * */
require('apps/scene.js');
require('apps/materials.js');
require('apps/mesh.js');
var dirty, damage, indicount = 0, pose_mode = 0;

/* an armature is an organized set of bones, this is the tree algorithm
 * the armature locations with a root, the root is like an object's location
 * moving the root moves the entire armature, children can't move the root
 * 
 * the root contains bones, each bone has only one parent but can have many kids
 * a bone has an location, length, rotation, children
 * 		location
 * 		length
 * 		size
 * 		rotation
 * 		children
 * the children are relatively placed according to their parents.
 * a bone can be rotated around the location, this rotation travels down the chain
 * if any bone in the chain is locked, the propagation stops right there.
 * 		imagine hand holding onto something and the rest of the body flying away
 * 		or a foot on the ground as the rest of the body moves around it
 * 
 * inverse kinematics
 * 		this propagates rotation up the chain towards parents
 * 		again, a locked bone stops the propagation
 * 
 * 
 * root
 * 		hip_l -> thigh_l -> calf_l -> foot_l
 * 		hip_r ...
 * 		tail -> back_lower -> back_upper
 * 									pec_l
 * 									pec_r
 * */
var armature, armatures = {}, BONE = scene.type('armature');
var is_posed = function (o) { 
	return !!(o.location.pose || o.rotation.pose || o.pose);
};
var pose_or_raw = function (o, prop) {
	if (prop) {
		if (o.pose && o.pose[prop]) return o.pose[prop];
		if (o[prop]) return o[prop];
	} else
	return o.pose || o;
};
var transform_or_pose_or_raw = function (o, prop) {
	if (prop) {
		if (o.transform && o.transform[prop]) return o.transform[prop];
		if (o.pose && o.pose[prop]) return o.pose[prop];
		if (o[prop]) return o[prop];
	} else
	return o.transform || o.pose || o;
};
var circles2poly = function (o, s, e) {
	var size = transform_or_pose_or_raw(o).size*(scene.zoom/3)+2;
	var ang = matrix.coord2angle(s.x, s.y, e.x, e.y);
	var aa = matrix.angle2coord(s.x, s.y, size, ang+90);
	var bb = matrix.angle2coord(s.x, s.y, size, ang-90);
	var cc = matrix.angle2coord(e.x, e.y, size/2, ang+90);
	var dd = matrix.angle2coord(e.x, e.y, size/2, ang-90);
	return [ [aa.x, aa.y], [bb.x, bb.y], [dd.x, dd.y], [cc.x, cc.y], [aa.x, aa.y] ];
};
;(function(){
	menu.add(0, 'armature');

	armature = function (armature_name) {
		var uid = 0;

		var transform_rotation = function (o, transform) {
			var l = o.get('location'), rot = o.rotation, b = transform;
//			var ang = matrix.coord2angle( l.x, l.y, b.x, b.y );
			rot.transform = v3.add( pose_or_raw(rot), transform );
//			rot.transform.x %= 360;
//			if (rot.transform.x < 0) rot.transform.x += 360;
		};
		var transform_location = function (o, transform) {
			if (pose_mode) { // only IK
				
			} else {
				o.location.transform = v3.add( pose_or_raw(o.location), transform );
//				o.calc_end();
			}
		};
		var get = function (key) { // get property
			var o = this, value = transform_or_pose_or_raw( o[key] );
			if (key == 'rotation') {
				if (o.offset_rotation) value = v3.add(o.offset_rotation, value);
			}
			switch (key) {
				case 'location':
				case 'end':
					if (o.offset_location) value = v3.add(o.offset_location, value);
					o.calc_end();
					break;
			}
			return value;
		};
		var set = function (key, value) { // set property
			var o = this;
			switch (key) {
				case 'location':
					transform_location(o, value);
					break;
				case 'rotation':
					transform_rotation(o, value);
					break;
				default: o[key] = value; break;
			}
			if ( ['location', 'length', 'rotation', 'size'].indexOf(key) > -1 ) {
				o.calc_end();
			}
		};
		var calc_end = function () {
			var o = this;

			var l = transform_or_pose_or_raw(o.location);
			var r = o.get('rotation');

			var alpha = matrix.to_radians(r.x),
				beta = matrix.to_radians(r.y);
				
			var end_x = Math.cos(alpha) * Math.cos(beta);
			var end_z = Math.sin(alpha) * Math.cos(beta);
			var end_y = Math.sin(beta);

			o.end = v3(l.x+end_x*o.length, l.y+end_y*o.length, l.z+end_z*o.length);

			if (o.mesh) {
				var source = l;
				var target = o.end;
				var ori = sgl.point_at(
					target.x, target.y, target.z,
					source.x, source.y, source.z
				);
				ori.x = ori.x-(512*.25);
				o.mesh.location = v3.add(l, o.offset_location);
				o.mesh.rotation = ori;
				o.mesh.update();
			}

			for (var i in o.children) {
				var c = o.children[i];
				c.offset_location = v3.add(o.offset_location, o.end);

				var sum_rot = v3(0, 0, 0);
				// children should inherit a rotational offset
				// pose or transform + offset_rotation
				if (o.rotation.transform) {
					sum_rot = v3.neg(o.rotation.transform, o.rotation);
				} else
				if (o.rotation.pose) {
					sum_rot = v3.neg(o.rotation.pose, o.rotation);
				}
				
				if (o.offset_rotation) {
//					var orig_offset = v3.neg(c.posed('rotation'), o.offset_rotation);
//					var orig_offset = c.posed('rotation');
//					log( c.name, orig_offset.x, orig_offset.y, orig_offset.z );
//					log( '    ', o.offset_rotation.x, o.offset_rotation.y, o.offset_rotation.z );

//					sum_rot.x = sum_rot.y - orig_offset.x;
//					if (sum_rot.y) sum_rot.y = sum_rot.x - orig_offset.y;
//					sum_rot = v3.neg(sum_rot, orig_offset);
					sum_rot = v3.add(sum_rot, o.offset_rotation);
				}
				
				c.offset_rotation = sum_rot;

				c.calc_end(); // TODO fig out better way, this is poten expens
			}
		};
		var apply = function () {
			var o = this;
			if (o.rotation.transform) o.rotation.pose = shallowcopy(o.rotation.transform);
			if (o.location.transform) o.location.pose = shallowcopy(o.location.transform);
			if (o.transform) {
				if (o.transform.size  ) {
					o.pose = o.pose || {};
					o.pose.size   = o.transform.size  ;
				}
				if (o.transform.length) {
					o.pose = o.pose || {};
					o.pose.length = o.transform.length;
				}
			}
			o.location.transform = 0;
			o.rotation.transform = 0;
			o.transform = 0;
			o.calc_end();
		};
		var cancel = function () {
			var o = this;
			if (pose_mode) { // only IK
			} else {
				o.location.transform = 0;
				o.rotation.transform = 0;
				o.offset_rotation = 0;
				o.transform = 0;
				o.calc_end();
			}
		};
		var make_bone = function (name, rotation, location, length, size) {
			var o = {
				name     : name || ++uid				,
				armature : armature_name				,
				location : location || v3(0)			,
				length   : length   || 100				,
				size     : size     || 10				,
				rotation : rotation || v3(0, 0, 0)		,
//				pose     : {}							,
				extend   : extend						,
			};
			o.get = get;
			o.set = set;
			o.cancel = cancel;
			o.apply = apply;
			o.calc_end = calc_end;
			
			var m = o.mesh = mesh(o.name, shallowcopy(o.location));
			m.no_info = 1;
			m.on_select = function () {
				scene.select(o.uid);
				return 1;
			};
			m.add_cube(o.size+5)
//						.select(m.recent.all   ).apply_material(1)
						.select(m.recent.front ).apply_material(1)
						.select(m.recent.right ).apply_material(2)
						.select(m.recent.left  ).apply_material(3)
						.select(m.recent.top   ).apply_material(4)
						.select(m.recent.bottom).apply_material(5)
						.select(m.recent.back  ).apply_material(6)
						;
			m.select(m.recent.bottom).translate(0, 10, 0)
			 .select(m.recent.top).translate(0, o.length-10, 0)
								  .scale	(.3, 1, .3);

			o.calc_end();
			return o;
		};
		var extend = function () {
			var o = make_bone.apply(make_bone, arguments);
			o = scene.add(armature_name+'.'+o.name, BONE, o, this);
			o.get = get;
			o.set = set;
			return o;
		};

		var methods = {
			add: function (name) {
				var o = make_bone.apply(make_bone, arguments);
				o = scene.add(armature_name+'.'+name, BONE, o);
				o.get = get;
				o.set = set;
				return o;
			},
			extend: function () {
				var o = scene.get(armature_name+'.root');
				if (o) {
					o = o.extend.apply(o, arguments);
					return o;
				}
			},
		};

		methods.add('root', v3(0, 90, 0), v3(0, 50), 50, 15);
		armatures[armature_name] = methods;
		return methods;
	};

	
})();

hooks.set('scene.pressed', function (o, k, m) { if (o.type == BONE) {
	var yes;
	if (k == 'tab') {
		pose_mode = !pose_mode;
		yes = 1;
	}
	if (k == 'e') {
		o.extend && o.extend();
		yes = 1;
	}
	return yes;
} });
hooks.set('scene.info', function (o) { if (o.type == BONE) {
	var l = o.get('location');
	var r = o.get('rotation');
	var len = o.get('length');
	var str = pose_mode ? 'pose mode\n' : '\n';
	str += 'x'+l.x+' y'+l.y+' angle'+r.x+' len'+len+'\n';
	var t = o.end;
	var ori = sgl.point_at(
		t.x, t.y, t.z,
		l.x, l.y, l.z
	);
	str += 'x'+parsefloat(t.x, 2)+' y'+parsefloat(t.y, 2)+' z'+parsefloat(t.z, 2)+'\n';
	str += 'ori x'+sgl2deg(ori.x)+' y'+sgl2deg(ori.y)+' z'+sgl2deg(ori.z)+'\n';
	return str;
} });
hooks.set('scene.draw', function (o, selected) { if (o.type == BONE) {
	canvas.save();
	var s = o.get('location');
	var e = o.get('end');
	
	canvas.line_width(1.5);
	
	s = sgl.to_screen(s.x, s.y, s.z);
	e = sgl.to_screen(e.x, e.y, e.z);
	
	var clr = c_lsilver;
	if (selected) clr = c_dwhite;
	if (is_posed(o)) clr = c_lyellow;
	
	var poly = circles2poly(o, s, e);

	poly.forEach(function (o, i) { canvas.line(o[0]+1, o[1])+1; });
	canvas.lwn.apply(null, c_black);
	canvas.stroke();

	poly.forEach(function (o, i) { canvas.line(o[0], o[1]); });
	canvas.lwn.apply(null, clr);
//	canvas.fill(1);
	canvas.stroke();

//	canvas.circle(s.x, s.y, transform_or_pose_or_raw(o, 'size')*(scene.zoom/3)+3, 0, 360);
//	canvas.lwn.apply(null, clr);
//	canvas.stroke();

	if (o.name == 'foot_l') {
		var mp = v3.midpoint(s, e);
		text(mp.x, mp.y-50,
		 'x'+o.rotation.x+' y'+o.rotation.y+'\n'
		+'x'+o.offset_rotation.x+' y'+o.offset_rotation.y+'\n'
		, 1);
	}
	
	canvas.restore();
} });
hooks.set('scene.selector', function (o, selector) { if (o.type == BONE) {
	
} });
hooks.set('scene.raycast', function (o, ev) { if (o.type == BONE) {
	var s = o.get('location');
	var e = o.get('end');

	s = sgl.to_screen(s.x, s.y, s.z);
	e = sgl.to_screen(e.x, e.y, e.z);

	var contact;
	if (!contact) // 1 start
		contact = matrix.distance(ev.x, ev.y, s.x, s.y) <= o.size ? 1 : 0;
	if (!contact) { // 2 mid
		var poly = circles2poly(o, s, e);
		contact = v3.inpolygon(ev.x, ev.y, poly) ? 2 : 0;
	}
	
	if (ev.state && contact) o.sel = contact;

	return contact ? 1 : 0;
} });
hooks.set('scene.move', function (o, transform) { if (o.type == BONE) {
	o.set('location', transform);
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == BONE) {
	o.set('rotation', transform);
} });
hooks.set('scene.scale', function (o, transform) { if (o.type == BONE) {
//	o.set('scale', transform);
} });
hooks.set('scene.apply', function (o) { if (o.type == BONE) {
	o.apply();
} });
hooks.set('scene.cancel', function (o) { if (o.type == BONE) {
	o.cancel();
} });
hooks.set('scene.remove', function (o) { if (o.type == BONE) {
	for (var i in o.children) {
		var c = o.children[i];
		if (c.mesh) {
			scene.remove(c.mesh.uid);
		}
	}
	if (o.mesh) scene.remove(o.mesh.uid);
} });
hooks.set('scene.reset', function (o) { if (o.type == BONE) {
	if (o.location.pose) o.location.pose = 0;
	if (o.rotation.pose) o.rotation.pose = 0;
	if (o.pose) o.pose = 0;
	o.calc_end();
	return 1;
} });

//var ground = mesh('ground', v3(0, -100));
//ground.add_plane(200).select(ground.recent.all).apply_material(2);
//ground.update();

var skeleton = armature('skeleton'); // this is the root
skeleton.extend('hip_l', 0, 0, 50)
	.extend('thigh_l', v3(0, 270))
	.extend('calf_l', v3(0, 270))
	.extend('foot_l', v3(-90, 0, 0), 0, 50)
	;
skeleton.extend('hip_r', v3(0, -180, 0), 0, 50)
	.extend('thigh_r', v3(0, -90, 0)).extend('calf_r', v3(0, -90, 0))
	.extend('foot_r', v3(-90, 0), 0, 50)
	;
skeleton.extend('tail', v3(0, 90, 0), 0, 50).extend('back_lower', v3(0, 90, 0), 0, 50);

scene.get('skeleton.root').calc_end();

materials.add( 0xff000000 ); // black
materials.add( 0xffff136f ); // red
materials.add( 0xff0084f0 ); // blue
materials.add( 0xff6e983c ); // green
materials.add( 0xffd35b0a ); // choco orange
materials.add( 0xff3fdada ); // cyan dull
materials.add( 0xff57554d ); // dune
scene.set_zoom(3);

function on_bound_window(win) {
//	cameras.add('cam0', 0, 0, win.w/2, win.h);
//	cameras.add('cam1', win.w/2, 0, win.w/2, win.h);
	canvas.state(1);
}
function on_paint_window () {
	canvas.save();
	canvas.xat("Ubuntu Mono");
	canvas.font_size(14);

	matrix.clear();
	damage = 1;

	canvas.lwn(1, 1, 1);
	canvas.line_width(1);

	scene.draw();

	canvas.restore();

	list.draw();
	menu.draw();
	return damage || dirty;
}
function on_paint_indicator () {
	canvas.save();
	canvas.lwn(0, 0, 0);
	canvas.paint();
	canvas.font_size(11);
	
	canvas.lwn(.2, .2, .2);
	canvas.rect(0, indicator.h-20, indicator.w, 20);
	canvas.fill();

	canvas.lwn(1, 1, 1);
	text(-2, 0, 'armature\n...\n'+indicount++);

	canvas.restore();
	return 1;
}
function on_pointer(m) {
	var yes = 0;
	if (!yes) yes = menu.pointer(m);
	if (!yes) yes = list.pointer(m);
	if (!yes) yes = scene.pointer(m);
	
	return yes;
}
function on_keyboard (m) {
	var yes;
	if (!yes) yes = menu.keyboard(m);
	if (!yes) yes = list.keyboard(m);
	if (!yes) yes = scene.keyboard(m);
	if (!yes && !m.state && m.text == '1') {
		scene.select('marco.root');
		scene.get('marco.root').sel = 2;
		yes = 1;
	}
	
	dirty = yes;
	return yes;
}




