var c_white   = [1, 1, 1, 1],
	c_dwhite  = [.75, .75, .75, 1],
	c_lsilver = [.5, .5, .5, 1],
	c_silver  = [.4, .4, .4, 1],
	c_dsilver = [.3, .3, .3, 1],
	c_lgray   = [.25,.25,.25, 1],
	c_gray    = [.2, .2, .2, 1],
	c_dgray   = [.1, .1, .1, 1],
	c_green   = [.1,  1, .1, 1],
	c_dgreen  = [.1,  .8, .1, 1],
	c_yellow  = [.8, .8, .2, 1],
	c_lyellow = [.6, .6, .2, 1],
	c_blue    = [.1, .1, 1, 1],
	c_red     = [ 1, .1, .1, 1],
	c_dred    = [ .6, .1, .1, 1],
	c_black   = [ 0,  0,  0, 1];
	c_transparent   = [ 0,  0,  0, 0];
var color4 = function (r, g, b, a) {
	if (isarr(r) || r instanceof Float32Array) a = r[3], b = r[2], g = r[1], r = r[0];
	return {
		r: isundef(r) ? 0 : r,
		g: isundef(g) ? 0 : g,
		b: isundef(b) ? 0 : b,
		a: isundef(a) ? 0 : a,
	};
};
color4.add = function (c, step) {
	var c4 = color4(c.r, c.g, c.b, c.a);
	c4.r += step || 0;
	c4.g += step || 0;
	c4.b += step || 0;
	c4.a += step || 0;
	return c4;
};