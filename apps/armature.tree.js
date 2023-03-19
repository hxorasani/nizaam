/* TODO
 * there should be two modes, edit mode and pose mode
 * in pose mode, moving a bone including root is IK based
 * in edit mode, moving a bone affects the origin
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
 * */
require('apps/scene.js');
var dirty, damage, indicount = 0, pose_mode = 0;

var parsefloat = function (a, b) { return parseFloat( Number(a).toFixed(b||2) ); };
var linemidpoint = function (a, b) {
	return {
		x: ( a.x+b.x ) * .5,
		y: ( a.y+b.y ) * .5
	};
};
msfoof.inpolygon = function (x, y, poly) {
	var isinside = 0;
	if (poly.length) {
		var minX = poly[0][0], maxX = poly[0][0];
		var minY = poly[0][1], maxY = poly[0][1];
		for (var n = 1; n < poly.length; n++) {
			minX = Math.min(poly[n][0], minX);
			maxX = Math.max(poly[n][0], maxX);
			minY = Math.min(poly[n][1], minY);
			maxY = Math.max(poly[n][1], maxY);
		}

		if (x < minX || x > maxX || y < minY || y > maxY) {
			return 0;
		}

		var i = 0, j = poly.length - 1;
		for (i, j; i < poly.length; j = i++) {
			if ( (poly[i][1] > y) != (poly[j][1] > y) &&
					x < (poly[j][0] - poly[i][0])
						* (y - poly[i][1])
						/ (poly[j][1] - poly[i][1]) + poly[i][0] )
			{
				isinside = !isinside;
			}
		}
	}
	return isinside;
};
msfoof.copy3 = function (source, target) {
	target.x = source.x;
	target.y = source.y;
	target.z = source.z;
};

/* an armature is an organized set of bones, this is the tree algorithm
 * the armature starts with a root, the root is like an object's origin
 * moving the root moves the entire armature, children can't move the root
 * 
 * the root contains bones, each bone has only one parent but can have many kids
 * a bone has an origin, length, rotation, children
 * 		origin
 * 		length
 * 		size
 * 		rotation
 * 		children
 * the children are relatively placed according to their parents.
 * a bone can be rotated around the origin, this rotation travels down the chain
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
var vec3 = function (x, y, z) {
	return {
		x: x,
		y: isundef(y) ? x : y,
		z: isundef(z) ? x : z,
	};
};
var transformed = function (o, create) { // create .transform if not found
	// POTENTIAL BUG, TEST further
	if (create && !o.transform) o.transform = {};
	var t = o.transform;
	// t exists and any axis are not zero
	if ( t && (t.x || t.y || t.z || t.size) ) {
		// mix transform with start/mid/end and return its copy
		return Object.assign({}, o, t);
	} else {
		return o;
	}
};
var is_posed = function (o) { 
	return !!(o.origin.pose || o.rotation.pose || o.pose);
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
	var size = transformed(o).size;
	var ang = msfoof.coord2angle(s.x, s.y, e.x, e.y);
	var aa = msfoof.angle2coord(s.x, s.y, size, ang+90);
	var bb = msfoof.angle2coord(s.x, s.y, size, ang-90);
	var cc = msfoof.angle2coord(e.x, e.y, size/2, ang+90);
	var dd = msfoof.angle2coord(e.x, e.y, size/2, ang-90);
	return [ [aa.x, aa.y], [bb.x, bb.y], [dd.x, dd.y], [cc.x, cc.y], [aa.x, aa.y] ];
};
var maintain_length = function (o, joint, preview) {
	/* algo, check what joint is selected
	 * check if the current transform makes 
	 * */
	joint = joint || o.sel;
	if (joint) {
		var s = transformed(o.origin, preview),
			e = transformed(o.origin, preview);
		var ang = msfoof.coord2angle(s.x, s.y, e.x, e.y);
		var coords = msfoof.angle2coord(s.x, s.y, o.length, ang);
		msfoof.copy3(coords, preview ? e.transform : e);
	}
};
var preview_chain = function (o) {
	if (o && o.transform)
	for (var i in o.conns) {
		var target = scene.get(i);
		msfoof.copy3(
			o.transform,
			transformed(target.origin, 1).transform
		);
		maintain_length(target, o.conns[i], 1);
	}
};
var cancel_chain = function (o) {
	if (o)
	for (var i in o.conns) {
		var target = scene.get(i);
		target.origin.transform = 0;
		maintain_length(target);
	}
};
var assign = Object.assign;
var show_bone_names = 0;
var deepen_transforms = function (old, cur) {
	if (!old) {
		return cur;
	} else
	return {
		translate: v3.add(old.translate, cur.translate),
		rotate	 : v3.add(old.rotate	 , cur.rotate	),
		scale	 : v3.add(old.scale	 , cur.scale	),
	};
};
;(function(){
	menu.add(0, 'armature');
	menu.add('bone_names', 0, function () { show_bone_names = !show_bone_names; });

	var transform3 = function (old, cur, fn) {
		fn = fn || add3;
		if (!old) {
			return cur;
		} else
		return {
			translate: fn(old.translate , cur.translate ),
			rotate	 : fn(old.rotate	, cur.rotate	),
			scale	 : fn(old.scale		, cur.scale		),
		};
	};
	var get_chain = function (o) {
		var arr = [o];
		if (o.parent) {
			var parent = scene.get(o.parent);
			arr = get_chain(parent).concat( arr );
		}
		return arr;
	};
	var calc_world_space_above = function (o, t) {
		t = t || {};
		if (o.parent) {
			var parent = scene.get(o.parent);
			if (parent) {
				if (t)
					t = deepen_transforms(parent.child_transforms, t);
				t = calc_world_space_above( parent, t );
			}
		}
		return t;
	};
	var calc_world_space_below = function (o, t) {
		if (Object.keys(o.children).length) {
			for (var i in o.children) {
				var c = o.children[i];
				c.world_space = deepen_transforms(t, c.child_transforms);
				calc_world_space_below( c, c.world_space );
			}
		}
	};
	var calc_world_space = function (o, t) { // modifies o
		var s = transform_or_pose_or_raw(o.origin);
		var rot = transform_or_pose_or_raw(o.rotation);
		var len = transform_or_pose_or_raw(o, 'length');
		var e = msfoof.angle2coord(s.x, s.y, len, rot.x);
		
		o.child_transforms = o.child_transforms || {};
		o.child_transforms.translate = e;
		o.child_transforms.rotate = rot;

		o.world_space = calc_world_space_above(o, t || o.child_transforms);
		return o.world_space;
	};
	var calc_inverse_kinematics = function (o, transform) {
		var parent = scene.get(o.parent);
		if (!parent) return 0;
		
		// freshen up chain's world_space coords
		o.world_space = calc_world_space(o);
		
		var chain = armature.get_chain(o), real_root = transform3({}, o.child_transforms);
		// temporarily set o as the "root", some math with world space i guess
		for (var i in chain) {
			var other = chain[i];
			other.world_space = transform3( other.world_space, real_root, neg3 );
			var tt = other.world_space.translate;
			other.debug = tt.x+' '+tt.y;
		}
		
		var s = pose_or_raw(o.origin);
		var tr = add3( pose_or_raw(s), shallowcopy(transform) );
		o.origin.transform = tr;
		var po = pose_or_raw(parent.origin);
		var rot = pose_or_raw(o, 'rotation');
		var len = pose_or_raw(o, 'length');
		var len2 = pose_or_raw(parent, 'length');

		var e = add3( o.world_space.translate, tr );
//		o.world_space.translate = e;

		var ang = msfoof.coord2angle(s.x, s.y, po.x, po.y)-rot.x;
//		parent.world_space.rotate = ang;
//		parent.world_space.translate = add3( po, tr );
		parent.origin.transform
			= msfoof.angle2coord(po.x, po.y, len2, ang);
		parent.debug += ' a'+Math.round(ang);

		// restore real root
		for (var i in chain) {
			var other = chain[i];
			other.world_space = transform3( other.world_space, real_root );
		}
		
//		o.origin.transform = 0;
	};
	var calc_child_transforms = function (o) { // modifies o
		var s = transform_or_pose_or_raw(o.origin);
		var rot = transform_or_pose_or_raw(o.rotation);
		var len = transform_or_pose_or_raw(o, 'length');
		var e = msfoof.angle2coord(s.x, s.y, len, rot.x);
		
		o.child_transforms = o.child_transforms || {};
		o.child_transforms.translate = e;
		o.child_transforms.rotate = rot;
		
		o.world_space = calc_world_space_above(o, o.child_transforms);
		calc_world_space_below(o, o.world_space);
	};

	armature = function (armature_name) {
		var data = {}, uid = 0; // data is a tree structure

		var make_bone = function (name, rotation, origin, length, size) {
			var o = {
				name     : name || ++uid				,
				armature : armature_name				,
				origin   : origin   || vec3(0)			,
				length   : length   || 100				,
				size     : size     || 10				,
				rotation : rotation || vec3(0, 0, 0)	,
//				pose     : {}							,
				extend   : extend						,
			};
			data[name] = o; // optimally, scene should handle this, so we won't need this
			return o;
		};
		var extend = function () {
			var o = make_bone.apply(make_bone, arguments);
			o = scene.add(armature_name+'.'+o.name, BONE, o, this);
			calc_child_transforms(o);
			return o;
		};

		var methods = {
			add: function (name) {
				var o = make_bone.apply(make_bone, arguments);
				o = scene.add(armature_name+'.'+name, BONE, o);
				calc_child_transforms(o);
				return o;
			},
			get: function (name) {
				return data[name];
			},
			set: function (name, key, value) {
				var o = this.get(name);
				if (o) {
					o[key] = value;
					if ( ['origin', 'length', 'rotation'].indexOf(key) > -1 ) {
						calc_child_transforms(o);
					}
				}
			},
			update: function (o) {
				calc_child_transforms(o);
			},
			remove: function (name) {
				delete data[name];
			},
			extend: function () {
				var o = this.get('root');
				if (o) {
					o = o.extend.apply(o, arguments);
					this.update(o);
					return o;
				}
			},
		};

		methods.add('root', vec3(270, 0, 0), vec3(0, 50), 50, 15);
		armatures[armature_name] = methods;
		return methods;
	};
	
	armature.cancel = function () {};
	armature.get_chain = get_chain;
	armature.calc_world_space_above = calc_world_space_above;
	armature.calc_world_space_below = calc_world_space_below;
	armature.calc_world_space = calc_world_space;
	armature.calc_inverse_kinematics = calc_inverse_kinematics;
	armature.calc_child_transforms = calc_child_transforms;
})();

hooks.set('scene.pressed', function (o, k, m) { if (o.type == BONE) {
	var yes, arma = armatures[o.armature];
	if (k == 'tab') {
		pose_mode = !pose_mode;
		yes = 1;
	}
	if (k == 'c') {
		arma.update(o);
		yes = 1;
	}
	if (k == 'e') {
		o.extend && o.extend();
		yes = 1;
	}
	if (m.alt && k == 'p') {
		if (o.origin  .pose) o.origin  .pose = 0;
		if (o.rotation.pose) o.rotation.pose = 0;
		if (o.pose) o.pose = 0;
		arma.update(o);
		yes = 1;
	}
	return yes;
} });
hooks.set('scene.info', function (o) { if (o.type == BONE) {
	var ori = transform_or_pose_or_raw(o.origin);
	var rot = transform_or_pose_or_raw(o.rotation);
	var len = transform_or_pose_or_raw(o, 'length');
	var str = pose_mode ? 'pose mode ' : '';
	str += 'x'+ori.x+' y'+ori.y+' angle'+rot.x+' len'+len;
	if (o.child_transforms) {
		str += ' | local x'+Math.round(o.child_transforms.translate.x);
		str += ' y'+Math.round(o.child_transforms.translate.y);
	}
	if (o.world_space) {
		str += ' | world x'+Math.round(o.world_space.translate.x);
		str += ' y'+Math.round(o.world_space.translate.y);
	}
	return str;
} });
hooks.set('scene.draw', function (o, selected) { if (o.type == BONE) {
	var parent;
	if (o.parent) parent = scene.get(o.parent);
	var s = transform_or_pose_or_raw(o.origin);
	var rot = transform_or_pose_or_raw(o.rotation);
	var len = transform_or_pose_or_raw(o, 'length');
	var e = msfoof.angle2coord(s.x, s.y, len, rot.x);

	canvas.save();

	if (selected && o.sel == 2) canvas.lwn.apply(null, c_dwhite);
	else canvas.lwn.apply(null, o.hover ? c_lsilver : c_silver);
	
	var poly = circles2poly(o, s, e);
	poly.forEach(function (o, i) { canvas.line(o[0], o[1]); });
	canvas.fill(1);
	canvas.lwn.apply(null, is_posed(o) ? c_lyellow : c_gray);
	canvas.stroke();

	if (selected && o.sel == 1) canvas.lwn.apply(null, c_dwhite);
	else canvas.lwn.apply(null, o.hover ? c_lsilver : c_silver);
	canvas.circle(s.x, s.y, transformed(o).size, 0, 360);
	canvas.fill(1);
	canvas.lwn.apply(null, c_gray);
	canvas.stroke();

	if (parent && parent.world_space) {
		var pe = parent.world_space.translate;
		canvas.set_dash(5, 1, 5);
		canvas.lwn.apply(null, c_red);
		canvas.move_to(s.x, s.y);
		canvas.line(pe.x, pe.y);
		canvas.stroke();
		canvas.renew_path();
	}

	if (show_bone_names || (o.hover && !selected) ) {
		var mp = linemidpoint(s, e);
		var rot = transform_or_pose_or_raw(o.rotation );
		text(mp.x, mp.y, o.name+'\n'+rot.x+'deg', 1);
	} else if (o.debug) {
		var mp = linemidpoint(s, e);
		var dist = msfoof.distance(s.x, s.y, e.x, e.y);
		text(mp.x, mp.y-20, o.debug, 1);
	}

	canvas.restore();
} });
hooks.set('scene.selector', function (o, selector) { if (o.type == BONE) {
	// test contact for start, mid, end
	var parent;
	if (o.parent) parent = scene.get(o.parent);
	var s   = transform_or_pose_or_raw(o.origin   );
	if (parent && parent.world_space) s = add3(s, parent.world_space.translate);

	var contact;
	if (!contact) // 1 start
		contact = msfoof.overlap(s.x, s.y, o.size*2, o.size*2,
				selector.x, selector.y, selector.w, selector.h);
	
	if (contact) o.sel = 1;

	return contact ? 1 : 0;
} });
hooks.set('scene.raycast', function (o, ev) { if (o.type == BONE) {
	// test contact for start, mid
	var parent;
	if (o.parent) parent = scene.get(o.parent);
	var s   = transform_or_pose_or_raw(o.origin   );
	if (parent && parent.world_space) s = add3(s, parent.world_space.translate);
	var e = o.world_space.translate;

	var contact;
	if (!contact) // 1 start
		contact = msfoof.distance(ev.x, ev.y, s.x, s.y) <= o.size ? 1 : 0;
	if (!contact) { // 2 mid
		var poly = circles2poly(o, s, e);
		contact = msfoof.inpolygon(ev.x, ev.y, poly) ? 2 : 0;
	}
	
	if (ev.state && contact) o.sel = contact;

	return contact ? 1 : 0;
} });
hooks.set('scene.move', function (o, transform) { if (o.type == BONE) {
	if (pose_mode) { // only IK to .world_space -> .pose
		armature.calc_inverse_kinematics(o, transform);
	} else {
		o.origin.transform = add3( pose_or_raw(o.origin), shallowcopy(transform) );
		armatures[o.armature].update(o);
	}
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == BONE) {
	var a = pose_or_raw(o.origin), b = transform;
	var ang = msfoof.coord2angle( a.x, a.y, b.x, b.y );
	o.rotation.transform = add3( pose_or_raw(o.rotation), shallowcopy(transform) );
	o.rotation.transform.x %= 360;
	if (o.rotation.transform.x < 0) o.rotation.transform.x += 360;
	armatures[o.armature].update(o);
} });
hooks.set('scene.scale', function (o, transform) { if (o.type == BONE) {
	o.transform = o.transform || {};
	o.transform.length = pose_or_raw(o, 'length') + transform.x;
	armatures[o.armature].update(o);
} });
hooks.set('scene.apply', function (o) { if (o.type == BONE) {
	if (o.rotation.transform) o.rotation.pose = shallowcopy(o.rotation.transform);
	if (o.origin  .transform) o.origin  .pose = shallowcopy(o.origin  .transform);
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
	o.origin  .transform = 0;
	o.rotation.transform = 0;
	o.transform = 0;
	armatures[o.armature].update(o);
} });
hooks.set('scene.cancel', function (o) { if (o.type == BONE) {
	if (pose_mode) { // only IK to .world_space -> .pose
	} else {
		o.origin  .transform = 0;
		o.rotation.transform = 0;
		o.transform = 0;
		armatures[o.armature].update(o);
	}
} });
hooks.set('scene.remove', function (o) { if (o.type == BONE) {
	if (armatures[o.armature]) armatures[o.armature].remove(o.uid);
} });

var marco = armature('marco'); // this is the root
marco.extend('hip_l', 0, 0, 50)
	.extend('thigh_l', vec3(90, 0)).extend('calf_l', vec3(90, 0))
	.extend('foot_l', vec3(0), 0, 50)
	;
marco.extend('hip_r', vec3(180, 0), 0, 50)
	.extend('thigh_r', vec3(90, 0)).extend('calf_r', vec3(90, 0))
	.extend('foot_r', vec3(180, 0), 0, 50)
	;
marco.extend('tail', vec3(270, 0), 0, 50).extend('back_lower', vec3(270, 0), 0, 50);
//marco.extend('hip_l').extend('thigh_l').extend('calf_l').extend('foot_l');
//marco.extend('hip_r').extend('thigh_r').extend('calf_r').extend('foot_r');
//var back_upper = marco.extend('tail' ).extend('back_lower').extend('back_upper')
//back_upper.extend('pec_l');
//back_upper.extend('pec_r');


function text(x, y, txt, center) {
	canvas.save();
	xh = canvas.font_bounds();
	String(txt).split("\n").forEach(function (c, e) {
		var mb = canvas.matn(c, 2);
		var ww = center ? mb.w/2 : -5;
		var hh = center ? mb.h : 0;
		canvas.lwn(0, 0, 0);
		canvas.move_to(x+1-ww, y+1+((1+e)*(xh.a+xh.d))-hh);
		canvas.matn(c);
		canvas.lwn(1, 1, 1);
		canvas.move_to(x-ww, y+((1+e)*(xh.a+xh.d))-hh);
		canvas.matn(c);
	});
	canvas.renew_path();
	canvas.restore();
}

function on_bound_window(win) {
//	cameras.add('cam0', 0, 0, win.w/2, win.h);
//	cameras.add('cam1', win.w/2, 0, win.w/2, win.h);
	canvas.state(1);
	canvas.xat("Ubuntu Mono");
	canvas.font_size(14);
}
function on_paint_window () {
	canvas.save();
	canvas.lwn(0, 0, 0);
	canvas.paint();
	damage = 1;

	canvas.lwn(1, 1, 1);
	canvas.line_width(1);


	scene.draw();
//	scene.offsets();
//	canvas.fill();
	

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
	if (m.ctrl && m.text == 'l') clear();
	if (!yes && !m.state && m.text == '1') {
		scene.select('marco.foot_l');
		scene.get('marco.foot_l').sel = 2;
		yes = 1;
	}
	dirty = yes;
	return yes;
}




