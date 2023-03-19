/* TODO 
 * this should replace kaatib and provide text boxes to all mods
 * also quick text rendering
 * */
var text = function (x, y, txt, dir, dry) {
	canvas.save();
	xh = canvas.font_bounds();
	var w = 0, h = 0;
	String(txt).split("\n").forEach(function (c, e) {
		var mb = canvas.text(c, 2);
		var ww = dir ? mb.w/2 : -5;
		if (dir == 2) ww = -window.w+mb.w;
		var hh = dir ? mb.h : 0;
		if (!dry) {
			canvas.lwn(0, 0, 0);
			canvas.move_to(x+1-ww, y+1+((1+e)*(xh.a+xh.d)));
			canvas.text(c);
			canvas.lwn(1, 1, 1);
			canvas.move_to(x-ww, y+((1+e)*(xh.a+xh.d)));
			canvas.text(c);
		}
		w = Math.max(w, mb.w-ww+3);
		h += ((1+e)*(xh.a+xh.d))-hh;
	});
	canvas.renew_path();
	canvas.restore();
	return dir ?
		{ x: x-w, y: y-h/4, w: w*2, h: h } : { x: x, y: y, w: w, h: h/1.4 };
}