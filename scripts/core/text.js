/*  
 * quick 2d text rendering, limited sizes and prolly only one font
 * */
var text = function (x, y, txt, dir, dry, fg) { // dry == 2 = rect bg
	xh = nuk.font_extents();
	var w = 0, h = 0;
	String(txt).split("\n").forEach(function (c, e) {
		var c = c.replace(/\t/g, '    ');
		var mb = nuk.text_extents(c);
		var ww = dir ? mb.w/2 : 0;
		if (dir == 2) ww = -Window.w+mb.w;
		var hh = dir ? mb.h : 0;
		w = Math.max(w, mb.w-ww+3);
		h += ((xh.a+xh.d))-hh;
		if (dry == 2)
		nuk.fill_rect(x  -ww-4, y  +(e*(xh.a+xh.d))-2, mb.w+8, xh.h+4, 0, 0, 0, 0, 1);

		if (!dry || dry == 2) {
			if (dry != 2) {
				if (fg) {
					nuk.draw_text( x  -ww, y  +(e*(xh.a+xh.d)), c, fg[0], fg[1], fg[2], fg[3] );
				} else {
					nuk.draw_text( x+1-ww, y+1+(e*(xh.a+xh.d)), c, 0, 0, 0, 1 );
					nuk.draw_text( x  -ww, y  +(e*(xh.a+xh.d)), c, 1, 1, 1, 1 );
				}
			}
		}
	});
	return dir ?
		{ x: x-w, y: y-h, w: w*2, h: h } : { x: x, y: y, w: w, h: h };
}