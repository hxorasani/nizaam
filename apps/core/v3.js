var v3 = function (x, y, z) {
	return {
		x: isundef(x) ? 0 : x,
		y: isundef(y) ? x : y,
		z: isundef(z) ? x : z,
	};
};
v3 = Object.assign(v3, {
	midpoint: function (a, b, c) { // TODO make 3d
		return {
			x: ( a.x+b.x ) * .5,
			y: ( a.y+b.y ) * .5
		};
	},
	inpolygon: function (x, y, poly) {
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
	add: function (a, b) {
		var c = {}; a = a || {}, b = b || {};
		c.x = parsefloat( (a.x || 0) + (b.x || 0), 2 );
		c.y = parsefloat( (a.y || 0) + (b.y || 0), 2 );
		c.z = parsefloat( (a.z || 0) + (b.z || 0), 2 );
		return c;
	},
	neg: function (a, b) {
		var c = {}; a = a || {}, b = b || {};
		c.x = parsefloat( (a.x || 0) - (b.x || 0), 2 );
		c.y = parsefloat( (a.y || 0) - (b.y || 0), 2 );
		c.z = parsefloat( (a.z || 0) - (b.z || 0), 2 );
		return c;
	},
	copy: function (source, target) {
		target.x = source.x;
		target.y = source.y;
		target.z = source.z;
	},
});
