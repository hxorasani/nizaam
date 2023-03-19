require('apps/scene.js');
var dirty, damage, indicount = 0;

var spheres = [], distances = [];
var line0 = scene.line( 0, 0,  0, [
	[0, 0, 0], [0, 100, 0], [-100, 200, 0], [100, 250, 0]
], c_gray, c_white );
line0.fill = 0;

/* ALGO
 * fig out how canvas.circle does angles 0 to 360
 * adjust coord2angle's output to match that either in msfoof or duk
 * bone joint distances should be cached in the very beginning
 * */
var parsefloat = function (a, b) { return parseFloat( Number(a).toFixed(b||2) ); };

/* add multiple armatures, they contain bones
 * a bone has three parts, [start joint, middle long part, end joint]
 * 
 * 
 * */
var armature;
;(function(){
	var armatures = {};
	armature = function (name) {
		data = {};
		var armature = {
			add: function () { // bone
				
			},
			get: function () {
				
			},
			remove: function () {
				
			},
		};
		return armature;
	};
})();
var objdistance = function (a, b) {
	return msfoof.distance( a[0], a[1], b[0], b[1] );
};
var linemidpoint = function (a, b) {
	return {
		x: ( a[0]+b[0] ) * .5,
		y: ( a[1]+b[1] ) * .5
	};
};

line0.p.forEach(function (o, i) {
	spheres.push( scene.sphere( o[0],  o[1],  o[2], 25, c_silver ) );
	if (i) {
		distances.push( objdistance(line0.p[i-1], line0.p[i]) );
	}
});

function text(x, y, txt, center) {
	canvas.save();
	xh = canvas.font_bounds();
	txt.split("\n").forEach(function (c, e) {
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
	spheres.forEach(function (o, i) {
		if (i) {
			var mp = linemidpoint(line0.p[i-1], line0.p[i]);
			text(
				mp.x, mp.y,
				parsefloat( objdistance(line0.p[i-1], line0.p[i]) )+'',
				1
			);
		}
	});

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

var applyconstraints = function () {
	scene.selected.forEach(function (uid) {
		spheres.forEach(function (o, i) {
			if (uid != o.uid) {
//				if (i) {
					line0.sel = [i];
					var a = line0.p[i ? i-1 : 1];
					// calc the ang bw cur sph & prev point's
					var ang = msfoof.coord2angle(a[0], a[1], o.x, o.y);
					// take coords of prev sph & give it dist & ang
					var dist = distances[i ? i-1 : 0];
					var coords = msfoof.angle2coord(a[0], a[1], dist, ang);
					coords.z = 0;
					// assign it to current point
					scene.copy(line0.uid, coords);
					scene.copy(o.uid, coords);
//				}
			}
		});
	});
};
hooks.set('oncleartransform', function () {
	scene.selected.forEach(function (uid) {
		spheres.forEach(function (o, i) {
			if (uid == o.uid)
				line0.sel = [i],
				scene.copy(line0.uid, o);
		});
	});
	applyconstraints();
});
hooks.set('scene.move', function (sel, transform) {
	var uid = sel.uid;
	spheres.forEach(function (o, i) {
		if (uid == o.uid) {
			line0.sel = [i];
			var a = line0.p[i ? i-1 : 1];
			var b = add3(o, transform);
			var ang = msfoof.coord2angle(a[0], a[1], b.x, b.y);
			var dist = distances[i ? i-1 : i];
			var coords = msfoof.angle2coord(a[0], a[1], dist, ang);
			coords.z = 0; // 2D only (sorry for now)
			scene.copy(line0.uid, coords);
		}
	});
	applyconstraints();
});




