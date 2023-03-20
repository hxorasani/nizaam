var dirty, damage, phyiscs_enabled = 1;
var goup = 0, godown = 0, goleft = 0, goright = 0;
var targetbody = 0, targetjoint = -1, lastjoint = -1;
var raees = {}; // TODO pre-add this from C
var pos = [0, 0, 0];
var rnd = Math.round;
var xh;

require('apps/core/text.js');
require('apps/menu.js');

var phytogname = function () { return (phyiscs_enabled ? 'disable' : 'enable'); };

menu.add(0, 'PHYSICS');
menu.add('physics-debug', 'debug', function () { physics.debug(); });
menu.add('physics-toggle', phytogname(), function () {
	phyiscs_enabled = !phyiscs_enabled;
	menu.text('physics-toggle', phytogname() );
});
menu.add(0, 'JOINTS');
menu.add('connect', 0, function () {
	physics.connect(targetbody, lastjoint, targetjoint);
});
menu.add('disconnect', 0, function () {
	physics.disconnect(targetbody, lastjoint, targetjoint);
});
menu.add('add-joint', 'add', function () {
	physics.addjoint(targetbody, .5);
});
menu.add('remove-joint', 'remove', function () {
	physics.removejoint(targetbody, targetjoint);
});
menu.add(0, 'BODIES');
menu.add('add-body', 'add', function () { physics.add(1); });
menu.add('remove-body', 'remove', function () {
	targetbody = physics.remove( targetbody )-1, yes = 1;
	if (targetbody < 0) targetbody = 0;
});



function now() { return new Date().getTime(); }
function on_bound_window(r) {
	raees = r;
	var w = r.w, h = r.h;
	pos[0] = 16;
	pos[2] = -w+5;
	physics.campos(pos[0], pos[1], pos[2]);
	physics.camrot(0, 0, 0);
	physics.camview(w, h, 95.6);
	physics.camfactor(1, 1);
	physics.gridres(16);
	physics.gridsize(256);

	canvas.state(1);
	canvas.xat("Ubuntu Mono");
	canvas.font_size(20);
	xh = canvas.font_bounds();
}
function circle(x, y, r, bg, fg) {
	canvas.circle (x, y, r, 0, 360);

	if (bg != -1) {
		var b = colors.rgba2lwnf(bg);
		canvas.lwn(b.r, b.g, b.b);
		canvas.fill(1);
	}

	if (fg != -1) {
		var f = colors.rgba2lwnf(fg);
		canvas.lwn(f.r, f.g, f.b);
		canvas.stroke(0);
	}
}
function apply_physics() { if (phyiscs_enabled) {
	for (var i = 0; i < 2; ++i) {
		if (goleft ) physics.accelerate(targetbody, -1      );
		if (goright) physics.accelerate(targetbody,  1      );

		if (goup   == -1 ) physics.accelerate(targetbody,  0, 0,  1 );
		if (godown == -1 ) physics.accelerate(targetbody,  0, 0, -1 );

		if (goup   ==  1 ) physics.accelerate(targetbody,  0,  1 );
		if (godown ==  1 ) physics.accelerate(targetbody,  0, -1 );

		physics.step();
	
		for (var j = 0; j < physics.bodies(); ++j) {
			physics.gravitate(j);

			if ( physics.isactive(j) ) damage = 2;
		}
	}

	physics.zlimit();
} }
function on_paint_window () {
//	taba3("something");
	canvas.save();
	canvas.lwn(0, 0, 0);
	canvas.paint();
	canvas.lwn(1, 1, 1);
	canvas.line_width(1);
	damage = 1;
	
	var bodyinfo = "";
	var jointsinfo = "";
	var connsinfo = "";

	apply_physics();

	// draw bodies
	for (var j = 0; j < physics.bodies(); ++j) {
		var active = physics.isactive(j);
		var body = physics.get(j);

		if (targetbody == j && physics.connections(j))
		for (var k = 0; k < physics.connections(j); ++k) {
			var cn = physics.connections(j, k);
			connsinfo += ' ['+cn.a+' '+cn.b+'] ';
		}
		canvas.line_width (1);
		// draw body joints:
		for (var k = 0; k < physics.joints(j); ++k) {
			var nq = physics.joints(j, k);

			var jointfill = -1;
			if (targetbody == j) {
				if (lastjoint == k) {
					jointfill = 0x444444;
				}
				if (targetjoint == k) {
					jointfill = 0x994444;
				}
			}
			circle(
				nq.x, nq.y, nq.s,
				jointfill,
				active ? 0xff9999 : 0x999999
			);
			
			if (targetbody == j) jointsinfo += ' <'+rnd(nq.x)+' '+rnd(nq.y)+'> ';
		}

		// draw the body center:
		var nq = physics.getcenter(j);
		
		canvas.line_width (2);
		circle(
			nq.x, nq.y,
			12, targetbody == j ? 0x666666 : -1, active ? 0xff6666 : 0x666666
		);
		text(nq.x-(xh.x), nq.y-xh.a, j+'');
		
		if (targetbody == j) bodyinfo += ' ['+rnd(nq.x)+' '+rnd(nq.y)+'] ';
		
		if (active) damage = 2;
	}
	
	text(0, 0,
		   'physics: '+ phyiscs_enabled
		+'\ntotal  : '+ physics.bodies()
		+'\nbody   : '+ bodyinfo
		+'\njoints : '+ jointsinfo
		+'\nconns  : '+ connsinfo
	);
	
	canvas.restore();

	menu.draw();

	return damage || dirty;
}
function on_focus (yes) {
	if (!yes)
		goup = 0, godown = 0, goleft = 0, goright = 0;
}
function on_pointer(m) {
	var yes = 0;
	if (m.state == 1 && m.n == 2) {
		menu.toggle(), yes = 1;
	} else if (menu.visible()) {
		if (m.w) {
			if (m.w == 1) menu.prev();
			if (m.w == -1) menu.next();
			yes = 1;
		} else {
			yes = menu.raycast(m.x, m.y, m.n == 1 ? m.state : undefined);
			if (!yes && m.n == 1) menu.toggle(), yes = 1; // hide on click outside
		}
	}
	if (!yes && m.state == 1) {
		for (var j = 0; j < physics.bodies(); ++j) {
			var nq = physics.getcenter(j);
			if ( msfoof.distance(m.x, m.y, nq.x, nq.y) <= 12 ) {
				targetbody = j;
				lastjoint = targetjoint = -1;
				return 1;
			}

			for (var k = 0; k < physics.joints(j); ++k) {
				nq = physics.joints(j, k);
				if ( msfoof.distance(m.x, m.y, nq.x, nq.y) <= nq.s ) {
					targetbody = j;
					if (targetjoint > -1) lastjoint = targetjoint;
					targetjoint = k;
					return 1;
				}
			}
		}
	}
	return yes;
}
function on_keyboard (m) {
	var yes, k = keyism[m.key];
	if (menu.visible()) {
		if (!m.state) {
			if (k == 'enter') menu.press(), yes = 1;
		}
		if (m.state) {
			if (k == 'up'   ) menu.prev(), yes = 1;
			if (k == 'down' ) menu.next(), yes = 1;
		}
	} else
	if (m.state) {
		if (k == 'tab') {
			if (targetbody < physics.bodies()-1) targetbody++;
			else targetbody = 0;
			yes = 1;
		}
		
		if (m.ctrl) {
			if (k == 'a'	) pos[0]--, yes = 1;
			if (k == 'z'	) pos[0]++, yes = 1;
			if (k == 'left'	) pos[1]--, yes = 1;
			if (k == 'right') pos[1]++, yes = 1;
			if (k == 'up'	) pos[2]--, yes = 1;
			if (k == 'down'	) pos[2]++, yes = 1;
			physics.campos(pos[0], pos[1], pos[2]);
		} else if (m.shift) {
			if (k == 'up'   ) goup    = -1, yes = 1;
			if (k == 'down' ) godown  = -1, yes = 1;
		} else {
			if (k == 'left' ) goleft  = 1, yes = 1;
			if (k == 'right') goright = 1, yes = 1;
			if (k == 'up'   ) goup    = 1, yes = 1;
			if (k == 'down' ) godown  = 1, yes = 1;
		}
	}
	if (!m.state) {
		if (m.ctrl && m.shift && k == 'd' )
			physics.debug(), yes = 1;
		else {
			if (k == 'c' ) physics.connect(targetbody, lastjoint, targetjoint), yes = 1;
			if (k == 'd' ) physics.disconnect(targetbody, lastjoint, targetjoint), yes = 1;
			if (k == 'j' ) physics.addjoint(targetbody, .5), yes = 1;
			if (k == 'k' ) physics.removejoint(targetbody, targetjoint), yes = 1;

			if (k == 'a' ) physics.add(1), yes = 1;
			if (k == 'r' ) {
				targetbody = physics.remove( targetbody )-1, yes = 1;
				if (targetbody < 0) targetbody = 0;
			}
		}

		if (k == 'left' ) goleft  = 0, yes = 1;
		if (k == 'right') goright = 0, yes = 1;
		if (k == 'up'   ) goup    = 0, yes = 1;
		if (k == 'down' ) godown  = 0, yes = 1;

		if (k == 'compose'	) {
			menu.toggle(), yes = 1;
		}
	}
	dirty = yes;
	return yes;
}


