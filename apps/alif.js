require('apps/core/reset.js');
require('apps/core/text.js');
require('apps/core/show_keys.js');
require('apps/menu.js');

var last_time = 0, frames_passed = 0, fps = 0;
function get_fps() {
	frames_passed++;
	var diff = new Date().getTime()-last_time;
	if (diff >= 1000) {
		fps = frames_passed;
		frames_passed = 0;
		last_time = new Date().getTime();
	}
	return fps;
}
var history = [];
function on_bound_window() {
	canvas.xat("Ubuntu Mono");
	canvas.font_size(18);
}
function on_paint_window() {
	canvas.save();
	
	matrix.clear(0xff112233);
	
	history.unshift(new Date()+' -- '+get_fps()+'fps');
	if (history.length > 32) history.pop();
	
	text(10, 30, history.join('\n'));
	
	canvas.restore();
	
	show_keys.draw();
	menu.draw();
	
	return 2;
}
function on_keyboard(m) {
	var yes;
	show_keys.keyboard(m);
	yes = menu.keyboard(m);
	return yes || 1;
}
function on_pointer(m) {
	var yes;
	show_keys.pointer(m);
	yes = menu.pointer(m);
	return yes || 1;
}