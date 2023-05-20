require('apps/menu.js');
require('apps/list.js');
require('apps/core/text.js');

var dirty, damage, indicount = 0, xh;

menu.add('clear-console', 0, function () { clear(); });

var dotnamesort = function (a, b) {
	const name_a = a.name.toUpperCase();
	const name_b = b.name.toUpperCase();
	if (name_a < name_b) return -1;
	if (name_a > name_b) return 1;
	return 0;
};

// TODO .watch (process_watch), .get <Buffer>, ...
var crumbs = ['apps/'];
var navigate = function (path) {
	apps = [], dirs = [];
	(files.list(path.join('/')) || [{ name: '..', type: 'dir' }])
	.forEach(function (o, i) {
		if (o.type == 'dir') {
			if (o.name != '.')
			dirs.push( o );
		}
		else apps.push( o );
	});
	apps.sort(dotnamesort);
	dirs.sort(dotnamesort);
	dirs.forEach(function (o, i) {
		list.add('dir'+i, '[ '+o.name+' ]', function () {
			list.empty(1);
			if (o.name == '.') {}
			else if (o.name == '..') {
				if (crumbs.length > 1)
					crumbs.pop();
			}
			else crumbs.push(o.name);
			
			navigate(crumbs);
		});
	});
	apps.forEach(function (o, i) {
		list.add('app'+i, o.name, function () {
			if (o.name.endsWith('.js')) {
				process.run( o.name.slice(0, -3) );
			}
		});
	});
};
navigate(crumbs);

function now() { return new Date().getTime(); }
function on_bound_window(win) {
	canvas.state(1);
	canvas.xat("Ubuntu Mono");
	canvas.font_size(30);
	list.show();
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
function on_paint_window () {
	canvas.save();
	canvas.lwn(0, 0, 0);
	canvas.paint();
	damage = 1;

	canvas.lwn(1, 1, 1);
	canvas.line_width(1);

	

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
	text(-2, 0, 'launcher\n...\n'+indicount++);

	canvas.restore();
	return 1;
}
function on_pointer(m) {
	var yes = 0;
	if (!yes) yes = menu.pointer(m);
	if (!yes) yes = list.pointer(m);
	return yes;
}
function on_keyboard (m) {
	var yes;
	if (!yes) yes = menu.keyboard(m);
	if (!yes) yes = list.keyboard(m);
	dirty = yes;
	return yes;
}


