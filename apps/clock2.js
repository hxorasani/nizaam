;(function(){
var line = Stroke(), _ = line;
scene.select(line.guid);
scene.edit_mode = 1;
//scene.wire_mode = 1;
_.point_size = .1;

var circle_radius = 6, line_width = .1;

for (var t = 0; t <= 180; t += 20) {
	var p = matrix.angle2coord(0, 0, circle_radius, t+90 );
	v3.precision(p);
	var point = Point(t/15-6, p.y, 0, line_width, [0, .3, 1, 1]);
	point.angle = t;
	_.points.push( point );
}
_.upload();

var animating = 0;
function randomize() {
	animating = 1;
	_.points.forEach(function (o, i) {
		if (i % 4 == 0 || i == 0) {
			var p, r = 0;
			r = 2-1*Math.random();
			p = matrix.angle2coord(0, 0, circle_radius+r, o.angle+90 );
			p.r = r/8;
			o.z = r;

			if (_.points.length-1 == i) {
				var q = _.points[0];
				o.x = q.x;
				o.y = q.y;
				o.r = q.r;
				q.c[0] = q.r*8;
				o.c[0] = q.r*8;
			} else {
				o.x = p.x;
				o.y = p.y;
				o.r = p.r;
				o.c[0] = r;
			}
		}
	});
	_.upload();
//	set_timeout(5000, function () { // BUG need clear_timeout
//		randomize();
//	});
}

//randomize();

hooks.set('nizaam.paint', function (m, k) {
	animating = 0;
});
hooks.set('nizaam.keyboard', function (m, k) {
	if (m.shift && k == 'r') {
		if (m.state) {
			if (!animating) randomize();
		}
		return 1;
	}
});
})();

