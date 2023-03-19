/* TODO
 * 
 * */
var dirty, damage, indicount = 0;

var light, LIGHT = scene.type('light'), show_light_names = 0;
;(function(){
	menu.add(0, 'light');
	menu.add('light_add', 'add', function () {
		var o = light();
		scene.deselect();
		scene.select(o.uid);
	});

	var uid = 0, lights = {};

	light = function (name, location, length, power, color) {
		var o = {
			name     : name      || ++uid			,
			location : location  || v3(0)			,
			length   : length    || 100				,
			power    : power     || 10				,
			color    : color     || c_white			,
		};

		o = scene.add('light.'+o.name, LIGHT, o);
		lights[name] = o;

		return o;
	};
	light.remove = function (o) {
		delete lights[o.name];
	};
	light.luma = function (o) { // takes an obj, returns luma percentage
		var luma = 0;
		var a = o.get('location');
		if (o.type < 100) a = o;
		for (var i in lights) {
			var l = lights[i], len = l.get('length');
			var b = l.get('location');
			var dist = matrix.distance(a.x, a.y, b.x, b.y);
			if (dist < len) {
				luma += parsefloat(1-dist/len, 3);
			}
		}
		return luma;
	};
})();

hooks.set('scene.pressed', function (o, k, m) { if (o.type == LIGHT) {
	var yes;
	if (k == 'tab') {
		yes = 1;
	}
	if (k == 'c') {
		yes = 1;
	}
	if (m.alt && k == 'p') {
		if (o.location.pose) o.location.pose = 0;
		if (o.length  .pose) o.length  .pose = 0;
		if (o.power   .pose) o.power   .pose = 0;
		if (o.pose) o.pose = 0;
		yes = 1;
	}
	return yes;
} });
hooks.set('scene.info', function (o) { if (o.type == LIGHT) {
	return ('x'+o.location.x+' y'+o.location.y+' z'+o.location.z)+
	' - power'+o.get('power')+' length'+o.get('length');
} });
hooks.set('scene.draw', function (o, selected) { if (o.type == LIGHT) {
	canvas.save();
	var s = o.get('location');
	s = sgl.to_screen(s.x, s.y, 0);
	var r = o.get('power')*scene.zoom;

	if (selected) canvas.lwn.apply(null, c_dwhite);
	else canvas.lwn.apply(null, o.hover ? c_lsilver : c_silver);

	canvas.line_width(1*scene.zoom);
	
	canvas.circle(s.x, s.y, r/2, 0, 360);
	canvas.fill();

	canvas.circle(s.x, s.y, r, 0, 360);
//	canvas.fill(1);
	canvas.lwn.apply(null, o.location.pose ? c_lyellow : c_gray);
	canvas.stroke();

	canvas.set_dash(5, 1, 5);
	canvas.circle(s.x, s.y, r+3, 0, 360);
	canvas.lwn.apply(null, c_silver);
	canvas.stroke();

	canvas.set_dash(5, 1, 5);
	canvas.circle(s.x, s.y, o.get('length')*scene.zoom, 0, 360);
	canvas.lwn.apply(null, c_silver);
	canvas.stroke();

	canvas.restore();
} });
hooks.set('scene.selector', function (o, selector) { if (o.type == LIGHT) {
	var s = o.get('location');
//	s = sgl.to_screen(s.x, s.y, 0);
//	var q = sgl.to_screen(selector.x, selector.y, 0);
	var r = o.get('power');
	

	var contact = matrix.overlap(s.x-r*3, -(s.y-r), r*2, r*2,
							selector.x, selector.y, selector.w, selector.h);

	return contact ? 1 : 0;
} });
hooks.set('scene.raycast', function (o, ev) { if (o.type == LIGHT) {
	var s = o.get('location');
	s = sgl.to_screen(s.x, s.y, 0);

	var contact;
	if (!contact) // 1 start
		contact = matrix.distance(ev.x, ev.y, s.x, s.y) <= o.get('power') ? 1 : 0;

	return contact ? 1 : 0;
} });
hooks.set('scene.move', function (o, transform) { if (o.type == LIGHT) {
	o.location.transform = v3.add(o.posed('location'), transform);
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == LIGHT) {
} });
hooks.set('scene.scale', function (o, transform, lock) { if (o.type == LIGHT) {
	o.transform = {};
	if (lock.x == 1) // only works if x axis is locked
	o.transform.power = o.posed('power') - transform.y/10;
	else
	o.transform.length = o.posed('length') - transform.x;
} });
hooks.set('scene.apply', function (o) { if (o.type == LIGHT) {
	if (o.location.transform) {
		o.location.pose = o.location.transform;
		o.location.transform = 0;
	}
	if (o.transform) {
		o.pose = o.pose || {};
		o.pose.length = o.transform.length;
		o.transform = 0;
	}
} });
hooks.set('scene.cancel', function (o) { if (o.type == LIGHT) {
	o.location.transform = 0;
	o.transform = 0;
} });
hooks.set('scene.reset', function (o) { if (o.type == LIGHT) {
	o.location.pose = 0;
	o.pose = 0;
} });
hooks.set('scene.remove', function (o) { if (o.type == LIGHT) {
	light.remove(o.name);
} });




