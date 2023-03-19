var dirty, damage;
var goup = 0, godown = 0, goleft = 0, goright = 0;
var targetbody = 2, debug = 1;
var raees = {}; // TODO pre-add this from C
var pos = [0, 0, 0];
var rnd = Math.round;
var xh;

//taba3('physics!');

function now() { return new Date().getTime(); }
function on_bound_window(r) {
	raees = r;
	var w = r.w, h = r.h;
	pos[0] = 16;
	pos[2] = -w+5;
	physics.debug(debug);
	physics.campos(pos[0], pos[1], pos[2]);
	physics.camrot(0, 0, 0);
	physics.camview(w, h, 95.6);
	physics.camfactor(1, 1);
	physics.gridres(16);
	physics.gridsize(256);

	canvas.state(1);
	canvas.xat("Ubuntu Mono");
	canvas.font_size(14);
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
function text(x, y, txt) {
	canvas.lwn(1, 1, 1);
	txt.split("\n").forEach(function (c, e) {
		canvas.move_to(x+(xh.x/2), y+((1+e)*xh.h));
		canvas.matn(c);
	});
	canvas.renew_path();
}
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

	// apply physics
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

	// draw bodies
	for (var j = 0; j < physics.bodies(); ++j) {
		var active = physics.isactive(j);

		// draw body joints:
		canvas.line_width (1);
		for (var k = 0; k < physics.joints(j); ++k) {
			var nq = physics.joints(j, k);

			circle(
				nq.x, nq.y, nq.s, -1, active ? 0xff9999 : 0x999999
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
	
	physics.zlimit();

	text(0, 0,
			   "U"+goup+" D"+godown+" L"+goleft+" R"+goright
		+'\n'+ physics.isactive(0)+" "+physics.isactive(1)
		+'\n'+ pos.join(', ')
		+'\n'+ bodyinfo
		+'\n'+ jointsinfo
	);
	
	canvas.restore();
	return damage || dirty;
}
function on_focus (yes) {
	if (!yes)
		goup = 0, godown = 0, goleft = 0, goright = 0;
}
function on_keyboard (m) {
	var yes, k = keyism[m.key];
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
	} else {
		if (m.ctrl && m.shift && k == 'd' )
			debug = !debug, physics.debug(debug), yes = 1;

		if (k == 'left' ) goleft  = 0, yes = 1;
		if (k == 'right') goright = 0, yes = 1;
		if (k == 'up'   ) goup    = 0, yes = 1;
		if (k == 'down' ) godown  = 0, yes = 1;
	}
	dirty = yes;
	return yes;
}


/*var global = new Function('return this;')();
taba3('global: {');
for (var i in global) {
	if (typeof global[i] == 'object' && i != 'global') {
		taba3( '  '+i+': {');
		for (var j in global[i]) {
			taba3( '    '+j+' '+typeof global[i][j]+' '+global[i][j] );
		}
		taba3('  }');
	} else {
		taba3( '  '+i+' '+global[i] );
	}
}
taba3('}');*/
