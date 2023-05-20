function v3(x, y, z) {
	if (isarr(x) || x instanceof Float32Array) z = x[2], y = x[1], x = x[0];
	return {
		x: isundef(x) ? 0 : x,
		y: isundef(y) ? x : y,
		z: isundef(z) ? x : z,
	};
};
v3 = Object.assign(v3, {
	between: function (val, min, max) {
		return (val >= min && val <= max);
	},
	equals: function (v, n) {
		return v.x == n && v.y == n && v.z == n;
	},
	str: function (a) {
		return 'v3('+a.x+', '+a.y+', '+a.z+')';
	},
	as_array: function (v) {
		return [v.x, v.y, v.z];
	},
	as_float32: function (v) {
		return new Float32Array([v.x, v.y, v.z]);
	},
	precision_float_array: function (m) {
		q = [];
		for (var i in m) {
			q[i] = parsefloat(m[i], 3);
		}
		return q;
	},
	precision: function (a, p) { // modifies a
		p = p||5;
		a.x = parsefloat(a.x, p);
		a.y = parsefloat(a.y, p);
		a.z = parsefloat(a.z, p);
	},
	midpoint: function (a, b, c) { // TODO make 3d
		return {
			x: ( a.x+b.x ) * .5,
			y: ( a.y+b.y ) * .5
		};
	},
	inside_poly: function (x, y, poly) {
		var isinside = 0;
		if (poly.length) {
			var minX = poly[0][0], maxX = poly[0][0];
			var minY = poly[0][1], maxY = poly[0][1];
			for (var n = 1; n < poly.length; n++) {
				minX = Math.min(poly[n][0], minX);
				maxX = Math.max(poly[n][0], maxX);
				minY = Math.min(poly[n][1], minY);
				maxY = Math.max(poly[n][1], maxY);
			}
	
			if (x < minX || x > maxX || y < minY || y > maxY) {
				return 0;
			}
	
			var i = 0, j = poly.length - 1;
			for (i, j; i < poly.length; j = i++) {
				if ( (poly[i][1] > y) != (poly[j][1] > y) &&
						x < (poly[j][0] - poly[i][0])
							* (y - poly[i][1])
							/ (poly[j][1] - poly[i][1]) + poly[i][0] )
				{
					isinside = !isinside;
				}
			}
		}
		return isinside;
	},
	inside_rect: function (a, b, x, y, w, h) {
		return v3.inside_poly(a, b, [
			[x, y],
			[w, y],
			[w, h],
			[x, h],
			[x, y],
		]);
	},
	add: function (a, b) {
		var c = {}; a = a || {}, b = b || {};
		c.x = (a.x || 0) + (b.x || 0);
		c.y = (a.y || 0) + (b.y || 0);
		c.z = (a.z || 0) + (b.z || 0);
		v3.precision(c);
		return c;
	},
	neg: function (a, b) {
		var c = {}; a = a || {}, b = b || {};
		c.x = (a.x || 0) - (b.x || 0);
		c.y = (a.y || 0) - (b.y || 0);
		c.z = (a.z || 0) - (b.z || 0);
		v3.precision(c);
		return c;
	},
	addf: function (a, b) { // add float
		var c = {}; a = a || {}, b = b || 1;
		c.x = (a.x || 0) + b;
		c.y = (a.y || 0) + b;
		c.z = (a.z || 0) + b;
		v3.precision(c);
		return c;
	},
	divf: function (a, b) { // divide by float
		var c = {}; a = a || {}, b = b || 1;
		c.x = (a.x || 0) / b;
		c.y = (a.y || 0) / b;
		c.z = (a.z || 0) / b;
		v3.precision(c);
		return c;
	},
	div: function (a, b) { // divide by vector
		var c = {}; a = a || {}, b = b || {};
		c.x = (a.x || 0) / (b.x || 0);
		c.y = (a.y || 0) / (b.y || 0);
		c.z = (a.z || 0) / (b.z || 0);
		v3.precision(c);
		return c;
	},
	mulf: function (a, b) { // multiply by float
		var c = {}; a = a || {}, b = b || 1;
		c.x = (a.x || 0) * b;
		c.y = (a.y || 0) * b;
		c.z = (a.z || 0) * b;
		v3.precision(c);
		return c;
	},
	mul: function (a, b) { // multiply by vector
		var c = {}; a = a || {}, b = b || {};
		c.x = (a.x || 0) * (b.x || 0);
		c.y = (a.y || 0) * (b.y || 0);
		c.z = (a.z || 0) * (b.z || 0);
		v3.precision(c);
		return c;
	},
	copy: function (source, target) {
		target.x = source.x;
		target.y = source.y;
		target.z = source.z;
	},
});
function print_vec3(m) {
	m = v3.precision_float_array(m);
	log(m[0], m[1], [2]);
};
function print_mat4(m) {
	m = v3.precision_float_array(m);
	log(m[0], m[4], m[ 8], m[12]);
	log(m[1], m[5], m[ 9], m[13]);
	log(m[2], m[6], m[10], m[14]);
	log(m[3], m[7], m[11], m[15]);
};
function to32bits(a, b) { // 16bit . 16bit = 32bit
	var buffer = new ArrayBuffer(4);
	var view = new DataView(buffer);

	view.setUint16(0, a, false);
	view.setUint16(2, b, false);
	
	return view.getUint32(0, false);
}
function to16bits(a) { // 32bit = 16bit . 16bit
	var buffer = new ArrayBuffer(4);
	var view = new DataView(buffer);

	view.setUint32(0, a, false);
	
	return [view.getUint16(0, false), view.getUint16(2, false)];
}
function Vector3RotateByAxisAngle(v, axis, angle) { // vec3 vec3 float
	// Using Euler-Rodrigues Formula
	// Ref.: https://en.wikipedia.org/w/index.php?title=Euler%E2%80%93Rodrigues_formula
	var result = v;
	var length = Math.sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
	if (length == 0) length = 1;
	var ilength = 1 / length;
	axis[0] *= ilength;
	axis[1] *= ilength;
	axis[2] *= ilength;
	angle /= 2;
	var a = Math.sin(angle);
	var b = axis[0] * a;
	var c = axis[1] * a;
	var d = axis[2] * a;
	a = Math.cos(angle);
	var w = vec3.fromValues( b, c, d );
	var wv = vec3.create();
	vec3.cross(wv, w, v);
	var wwv = vec3.create();
	vec3.cross(wwv, w, wv);
	vec3.scale(wv, wv, 2 * a);
	vec3.scale(wwv, wwv, 2);
	vec3.add(result, result, wv);
	vec3.add(result, result, wwv);
	return result;
}