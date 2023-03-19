/* TODO
 * 
 * */
require('apps/scene.js');
require('apps/light.js');
require('apps/mesh.js');
var dirty, damage, indicount = 0;

//scene.cube(0, 0, 0, 100, 100, 100);
//scene.cube(150, 0, 0, 100, 100, 100);
//scene.cube(0, -150, 0, 100, 100, 100);
//var lamp = light('lamp', v3(200, -200), 500, 15);
//scene.select('light.lamp');

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
	canvas.xat("Ubuntu Mono");
	canvas.font_size(14);
}
function on_paint_window () {
	canvas.save();
	canvas.xat("Ubuntu Mono");
	canvas.font_size(14);
	canvas.lwn(0, 0, 0);
	canvas.paint();
	damage = 1;

	canvas.lwn(1, 1, 1);
	canvas.line_width(1);

	scene.draw();

	canvas.restore();

	show_keys.draw();
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
	text(-2, 0, 'funnan\n...\n'+indicount++);

	canvas.restore();
	return 1;
}
function on_pointer(m) {
	var yes = 0;
	if (!yes) yes = menu.pointer(m);
	if (!yes) yes = list.pointer(m);
	if (!yes) yes = scene.pointer(m);

	yes = show_keys.pointer(m);
	return yes;
}
function on_keyboard (m) {
	var yes;
	if (!yes) yes = menu.keyboard(m);
	if (!yes) yes = list.keyboard(m);
	if (!yes) yes = scene.keyboard(m);
	if (m.ctrl && m.text == 'l') clear();

	yes = show_keys.keyboard(m);
	dirty = yes;
	return yes;
}




