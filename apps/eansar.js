var cajrah = cajrah || {}, uid = uid || 0, drag = -1, dirty, damage, last;
var raees = {}; // TODO pre-add this from C

taba3('startup!');

function now() { return new Date().getTime(); }
function on_bound_window(r) {
	raees = r;
}
function muctamil(x, y, g) {
	if (x >= g.x && y >= g.y && x <= g.x+g.w && y <= g.y+g.h) return 1;
}
function findobj(x, y) {
	for (var i in cajrah) 
		if (muctamil(x, y, cajrah[i]))
			return i;
}
function X_on_paint_indicator () {
	canvas.save();
	canvas.lwn(0, .4, .2);
	canvas.paint();

	canvas.font_size(13);
	canvas.lwn(1, 1, 1);
	canvas.move_to(3, 18);
	canvas.matn('tot: '+Object.keys(cajrah).length);
	
	canvas.restore();
	return damage || dirty;
}
function imsatal(c, i) {
	if (!c) return;
	
//	canvas.rotate(10);
	var blue = 1- c.y/raees.h;
	
	var ms = now();
	canvas.line_width(1);
	canvas.rect(c.x-.5, c.y-.5, c.w, c.h);
	canvas.lwn(.1, i == drag ? .4 : .2, blue, c.alpha);
	canvas.fill(1);
	canvas.lwn(i == last ? 1 : .1, .6, blue, c.alpha);
	canvas.stroke(1);
	canvas.clip();
	canvas.renew_path();

	var xq = c.w/4 - 1;
	canvas.font_size(xq);

	canvas.lwn(.1, 1, 1, c.alpha);

	canvas.move_to(c.x+5, c.y+xq);
	canvas.matn('x '+Math.round(c.x));

	canvas.move_to(c.x+5, c.y+(xq*2));
	canvas.matn('y '+Math.round(c.y));

	canvas.move_to(c.x+5, c.y+(xq*3));
	canvas.matn('i '+i);

	canvas.move_to(c.x+5, c.y+(xq*4));
	c.ms = now()-ms;
	canvas.matn('ms '+c.ms);

	canvas.unclip();

//	canvas.rotate(-10);
}
function X_on_paint_window () {
//	taba3("something");
	canvas.save();
	canvas.lwn(0, 0, 0);
	canvas.paint();
	canvas.font_size(11);

	canvas.lwn(1, 1, 1);
	
	canvas.line_width(1);

	damage = 0;
	for (var i in cajrah) {
		var o = cajrah[i];
		if (!o.accel) o.accel = 0;
		if (o.y+o.h < raees.h) {
			var pct = o.y / raees.h;
			var step = 1;
			if (step < 0) step = 1;
			o.accel += step;
			if (o.accel > 50) o.accel = 50;
			if (pct > .85 && o.accel > 3) o.accel *= 1-pct;
			o.y += o.accel;
			damage = 2;
		} else if (!o.kill) {
			o.kill = now();
			o.alpha = 1;
			damage = 2;
		} else if (o.kill) {
			damage = 2;
		}
		imsatal(o, i);
	}
	for (var i in cajrah) {
		var o = cajrah[i];
		if (o.kill) {
			if (o.alpha > 0) {
				var pct = 1-((now() - o.kill) / 2000)
				o.alpha = pct;
				o.h = o.h*pct;
			} else if (now() - o.kill > 2000) {
				delete cajrah[i];
				damage = 2;
			}
		}
	}
	
	canvas.restore();
	return damage || dirty;
}
function b_icaarah (m) {
	var yes;
	if (!m.w) {
		if (m.haal == -1) {
			if (m.n < 2) {
				for (var i in cajrah) 
					if (muctamil(m.x, m.y, cajrah[i])) drag = i, yes = 1;
			}
			if (m.n == 2) {
				cajrah[uid++] = {
					x: m.x, y: m.y,
					w:  64, h:  64,
				};
			}
			if (m.n == 3) {
				last = findobj(m.x, m.y);
				if (last > -1)
					delete cajrah[last];
			}
		}
		if (drag > -1 && !m.haal) {
			var cd = cajrah[drag];
			if (cd) {
				cd.x = m.x-(cd.w/2);
				cd.y = m.y-(cd.h/2);
			}
		}
		if (m.haal == 1 && drag > -1) {
//			last = drag;
			drag = -1;
		}
		yes = 1;
	} else if (m.w) {
		last = findobj(m.x, m.y);
		
		if (last > -1) {
			var cd = cajrah[last];
			if (cd) {
				cd.x += m.w*2;
				cd.y += m.w*2;
				cd.w -= m.w*4;
				cd.h -= m.w*4;
				if (cd.w <=  48) cd.w = 48;
				if (cd.w >= 128) cd.w = 128;
				if (cd.h <=  48) cd.h = 48;
				if (cd.h >= 128) cd.h = 128;
				yes = 1;
			}
		}
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
