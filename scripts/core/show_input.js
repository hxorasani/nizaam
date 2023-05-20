var show_input;
;(function(){
var last_touch, last_touch_time, touches = {};
show_input = function () {
	var yes;
	for (var i in touches) {
		var t = touches[i];
		var ms = new Date().getTime() - t.time;
		if (t.state != 1 && ms < 3000) ms = 1;
		var pct = ease.elasticin(1-ms/2000, 2, 1, 1);
		var size = pct*50;
		var size2 = size/2;
		if (t.c)
		nuk.stroke_circle(t.x-size2, t.y-size2, size, size, 5, t.c.r, t.c.g, t.c.b, .75-ms/1500);
//		text(t.x, t.y-size-20, t.w+'\n'+ms, 1);
		if (ms > 2000) {
			delete touches[i];
		}
		yes = 1;
	}
	return yes ? 1 : 0; // frames of animation
}
hooks.set('nizaam.touch', function (m) {
	last_touch_time = new Date().getTime();
	var new_touch = isundef(touches[m.w]);
	var t = touches[m.w] = Object.assign(touches[m.w] || {}, m);
	if (new_touch) {
		t.c = color4(2*Math.random(), 2*Math.random(), 2*Math.random(), 1);
	}
	t.time = last_touch_time;
	last_touch = m;
});
hooks.set('nizaam.pointer', function (arg_one) {
	// ignore while touching
	if (last_touch) { // ugly hack to disable pointer input temporarily
		if (last_touch.state == 1) { // TODO if all fingers lifted
			if (new Date().getTime() - last_touch_time > 2000)
				last_touch = 0; // forget last touch
		}
	} else {
//		return 1; // prevent pointer during touch
	}
});
})();
