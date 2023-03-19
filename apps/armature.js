require('apps/scene.js');
var dirty, damage, indicount = 0;

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

/* add multiple armatures, they contain bones
 * a bone has three parts, [start joint, middle long part, end joint]
 * 
 * 
 * */
var armature, armatures = {}, BONE = scene.type();
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
	if ( t && (t.x || t.y || t.z || t.r) ) {
		// mix transform with start/mid/end and return its copy
		return Object.assign({}, o, t);
	} else {
		return o;
	}
};
var circles2poly = function (s, e) {
	var ang = msfoof.coord2angle(s.x, s.y, e.x, e.y);
	var aa = msfoof.angle2coord(s.x, s.y, s.r, ang+90);
	var bb = msfoof.angle2coord(s.x, s.y, s.r, ang-90);
	var cc = msfoof.angle2coord(e.x, e.y, e.r, ang+90);
	var dd = msfoof.angle2coord(e.x, e.y, e.r, ang-90);
	return [ [aa.x, aa.y], [bb.x, bb.y], [dd.x, dd.y], [cc.x, cc.y], [aa.x, aa.y] ];
};
var maintain_length = function (o, joint, preview) {
	/* algo, check what joint is selected
	 * check if the current transform makes 
	 * */
	joint = joint || o.sel;
	if (joint) {
		var s = transformed(joint == 3 ? o.end : o.start, preview),
			e = transformed(joint == 3 ? o.start : o.end, preview);
		var ang = msfoof.coord2angle(s.x, s.y, e.x, e.y);
		var coords = msfoof.angle2coord(s.x, s.y, o.mid.length, ang);
		msfoof.copy3(coords, preview ? e.transform : e);
	}
};
var preview_chain = function (o) {
	if (o && o.transform)
	for (var i in o.conns) {
		var target = scene.get(i);
		msfoof.copy3(
			o.transform,
			transformed(o.conns[i] == 1 ? target.start : target.end, 1).transform
		);
		maintain_length(target, o.conns[i], 1);
	}
};
var cancel_chain = function (o) {
	if (o)
	for (var i in o.conns) {
		var target = scene.get(i);
		target.start.transform = 0;
		target.end.transform = 0;
		maintain_length(target, 1);
		maintain_length(target, 3);
	}
};
;(function(){
	var _connect_ = function (a, name2, joint1, joint2) {
		if (a.uid == name2) return;
		// check if a conn like that already exists
		// one bone can only have one of its joints connected to another
		// connect joint1 of name1 to joint2 of name2
		if (joint1 == 1) {
			// we're about to connect start, make sure end is not connected
			if (a.end.conns[name2]) delete a.end.conns[name2];
			a.start.conns[name2] = joint2;
		}
		if (joint1 == 3) {
			if (a.start.conns[name2]) delete a.start.conns[name2];
			a.end.conns[name2] = joint2;
		}
	};
	
	armature = function (armature_name) {
		var data = {}, uid = 0;
		var armature = {
			add: function (start, mid, end) { // bone, {}, {}, {}
				var o = {};
				o.start		= start	|| vec3( 0 ); // at 0
				o.start.r	= o.start.r || 3;
				o.end		= end	|| add3(o.start, vec3(0, 100)); // at 100
				o.end.r		= o.end.r || 3;

				o.mid		= mid	|| {
					length: parsefloat( msfoof.distance(o.start.x, o.start.y, o.end.x, o.end.y) )
				}; // length
				
				var bone_name = armature_name+'.'+uid++;
				
				o.start.START = bone_name;
				o.end  .END   = bone_name;

				o.start.conns = o.start.conns || {};
				o.end  .conns = o.end  .conns || {};
				
				var arm = this;
				o.start.connect = function (joint) {
					arm.connect_both(bone_name, joint.START || joint.END, 1, joint.START ? 1 : 3);
				};
				o.end.connect = function (joint) {
					arm.connect_both(bone_name, joint.START || joint.END, 3, joint.START ? 1 : 3);
				};
				
				o.armature = armature_name;
				data[bone_name] = o;
				
				return scene.add(bone_name, BONE, o);
			},
			get: function (name) {
				return data[name];
			},
			remove: function () {
				// TODO break connections .conns
				delete data[name];
			},
			extend: function () {
				
			},
			connect_both: function (name1, name2, joint1, joint2) {
				var a = scene.get(name1), b = scene.get(name2);
				if (a) _connect_(a, name2, joint1, joint2);
				// do the same for other side
				if (b) _connect_(b, name1, joint2, joint1);
				
				var target = joint2 == 1 ? b.start : b.end;
				var source = joint1 == 1 ? a.start : a.end;
				
				msfoof.copy3(source, target);
				maintain_length(b, joint2);
			},
		};
		armatures[armature_name] = armature;
		return armature;
	};
})();

hooks.set('scene.draw', function (type, o, selected) { if (type == BONE) {
	var s = transformed(o.start), e = transformed(o.end);
	canvas.save();

	if (selected && o.sel == 2) canvas.lwn.apply(null, c_white);
	else canvas.lwn.apply(null, c_silver);
	
	var poly = circles2poly(s, e);
	poly.forEach(function (o, i) {
		canvas.line(o[0], o[1]);
	});
	canvas.fill(1);
	canvas.lwn.apply(null, c_gray);
	canvas.stroke();

	if (selected && o.sel == 1) canvas.lwn.apply(null, c_white);
	else canvas.lwn.apply(null, c_silver);
	canvas.circle(s.x, s.y, s.r, 0, 360);
	canvas.fill(1);
	canvas.lwn.apply(null, c_gray);
	canvas.stroke();

	if (selected && o.sel == 3) canvas.lwn.apply(null, c_white);
	else canvas.lwn.apply(null, c_silver);
	canvas.circle(e.x, e.y, e.r, 0, 360);
	canvas.fill(1);
	canvas.lwn.apply(null, c_gray);
	canvas.stroke();

	var mp = linemidpoint(s, e);
	text(mp.x, mp.y, o.mid.length+'\n'+o.uid, 1);
	
	for (var i in s.conns) {
		var conn = scene.get(i);
		var other = transformed(s.conns[i] == 1 ? conn.start : conn.end);
		canvas.lwn.apply(null, c_red);
		canvas.move_to(s.x, s.y);
		canvas.line   (other.x, other.y);
		canvas.stroke();
	}
	for (var i in e.conns) {
		var conn = scene.get(i);
		var other = transformed(e.conns[i] == 1 ? conn.start : conn.end);
		canvas.lwn.apply(null, c_red);
		canvas.move_to(e.x, e.y);
		canvas.line   (other.x, other.y);
		canvas.stroke();
	}

	canvas.restore();
} });
hooks.set('scene.raycast', function (type, o, ev) { if (type == BONE) {
	// test contact for start, mid, end
	var s = transformed(o.start), e = transformed(o.end);
	var contact;
	if (!contact) // 1 start
		contact = msfoof.distance(ev.x, ev.y, s.x, s.y) <= s.r ? 1 : 0;
	if (!contact) // 3 end
		contact = msfoof.distance(ev.x, ev.y, e.x, e.y) <= e.r ? 3 : 0;
	if (!contact) { // 2 mid
		var poly = circles2poly(s, e);
		contact = msfoof.inpolygon(ev.x, ev.y, poly) ? 2 : 0;
	}
	
	if (ev.state && contact) o.sel = contact;

	return contact ? 1 : 0;
} });
hooks.set('scene.move', function (type, o, transform) { if (type == BONE) {
	var s = o.start, e = o.end;
	if (o.sel == 1 || o.sel == 2) {
		s.transform = add3( s, shallowcopy(transform) );
		maintain_length(o, 0, 1);
		preview_chain(s);
		preview_chain(e);
	}
	if (o.sel == 3 || o.sel == 2) {
		e.transform = add3( e  , shallowcopy(transform) );
		maintain_length(o, 0, 1);
		preview_chain(s);
		preview_chain(e);
	}
} });
hooks.set('scene.scale', function (type, o, transform) { if (type == BONE) {
	if (o.sel == 1 || o.sel == 2) {
		o.start.transform = { r: o.start.r+transform.x/4 };
		if (o.start.transform.r < 1) o.start.transform.r = 2;
	}
	if (o.sel == 3 || o.sel == 2) {
		o.end.transform = { r: o.end.r+transform.x/4 };
		if (o.end.transform.r < 1) o.end.transform.r = 2;
	}
} });
hooks.set('scene.apply', function (type, o) { if (type == BONE) {
	if (o.sel == 1 || o.sel == 2) o.start = transformed( o.start ), o.start.transform = 0;
	if (o.sel == 3 || o.sel == 2) o.end   = transformed( o.end   ), o.end  .transform = 0;
} });
hooks.set('scene.cancel', function (type, o) { if (type == BONE) {
	o.start.transform = 0;
	o.end  .transform = 0;
	cancel_chain(o.start);
	cancel_chain(o.end  );
} });
hooks.set('scene.remove', function (type, o) { if (type == BONE) {
	if (armatures[o.armature]) {
		armatures[o.armature].remove(o.uid);
	}
} });

var marco = armature('marco');
var first = marco.add();
first.start.r = 20;
first.end.r = 15;

var second = marco.add(vec3(50, 100));
second.start.r = 15;
second.end.r = 10;

var third = marco.add(vec3(150, 200), 0, vec3(100, 200));
third.start.r = 10;
third.end.r = 5;

first .end.connect(second.start);
second.end.connect(third .start);

function text(x, y, txt, center) {
	canvas.save();
	xh = canvas.font_bounds();
	String(txt).split("\n").forEach(function (c, e) {
		var mb = canvas.matn(c, 2);
		var ww = center ? mb.w/2 : -5;
		var hh = center ? mb.h : 0;
		canvas.lwn(0, 0, 0);
		canvas.move_to(x+1-ww, y+1+((1+e)*xh.h)-hh);
		canvas.matn(c);
		canvas.lwn(1, 1, 1);
		canvas.move_to(x-ww, y+((1+e)*xh.h)-hh);
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
	scene.offsets();
	canvas.fill();
	

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
	dirty = yes;
	return yes;
}




