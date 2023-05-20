/* TODO
 * align[v,h] should target text, anchor[v,h] origin
 * */
var element, Element, ELEMENT = scene.type('element');
;(function(){
element = Element = function (name, loc, rot, sca) {
	function on_update_colors(_) {
		// held > hover > selected > normal
		var fg = _.fg, bg = _.bg;
		if (_.state == -1 && _.fg_pressed)
			fg = _.fg_pressed;
		else if (!isundef(_.hover) && _.fg_hover)
			fg = _.fg_hover;
		else
			fg = _.fg;

		if (_.state == -1 && _.bg_pressed)
			bg = _.bg_pressed;
		else if (!isundef(_.hover) && _.bg_hover)
			bg = _.bg_hover;
		else
			bg = _.bg;

		_.text.color = fg;
		_.plane.select(_.plane.recent.all).apply_color(bg);
	}
	function on_hover(level) {
		var _ = this; on_update_colors(_);
	}
	function on_leave() {
		var _ = this; on_update_colors(_);
	}
	function on_select() {
		var _ = this; on_update_colors(_);
		if (_.plane) _.plane.show_vertices = 1;
	}
	function on_press() {
		var _ = this; on_update_colors(_);
		if (isfun(_._on_press)) _._on_press();
	}
	function on_release() {
		var _ = this; on_update_colors(_);
		if (isfun(_._on_release)) _._on_release();
	}
	function on_deselect() {
		var _ = this; on_update_colors(_);
		if (_.plane) _.plane.show_vertices = 0;
	}
	function on_update() {
		var _ = this;
		var pt = 0, pr = 0, pb = 0, pl = 0;
		if (isarr(_.pad)) pt = _.pad[0], pr = _.pad[1], pb = _.pad[2], pl = _.pad[3];
		if (isnum(_.pad)) pt = pr = pb = pl = _.pad;
		var bb = shallowcopy(_.text.bounds);
		_.text.location.y = -bb.h/3 - pt;
		
		var ver, hor;
		if (isarr(_.anchor)) ver = _.anchor[0], hor = _.anchor[1];
		else ver = hor = _.anchor;

		if (ver == element.RIGHT) {
			_.text.location.x = -pl-bb.w;
			if (_.w) bb.w = _.w; else bb.w += pr+pl;
			bb.x -= bb.w;
			bb.w -= bb.w;
		} else if (ver == element.CENTER) {
			_.text.location.x = -bb.w/2;
			if (_.w) bb.w = _.w; else bb.w += pr+pl;
			bb.x -= bb.w/2;
			bb.w -= bb.w/2;
		} else {
			_.text.location.x = pl;
			if (_.w) bb.w = _.w; else bb.w += pr+pl;
		}
		if (_.h) bb.h = -_.h; else bb.h -= pb+pt;
		if (hor == element.BOTTOM) {
			_.text.location.y -= bb.h;
			bb.y -= bb.h;
			bb.h -= bb.h;
		} else if (hor == element.MIDDLE) {
			_.text.location.y = bb.h/2;
			bb.y -= bb.h/2;
			if (_.h) bb.h = -_.h/2; else bb.h -= (pb+pt)/2;
		} else {
			_.text.location.y = -bb.h;
			bb.y -= bb.h;
			if (_.h) bb.h += _.h; else bb.h += pb+pt;
		}
		_.plane.select(_.plane.recent.tl).set_vert(bb.x, bb.y, -.01);
		_.plane.select(_.plane.recent.bl).set_vert(bb.x, bb.h, -.01);
		_.plane.select(_.plane.recent.tr).set_vert(bb.w, bb.y, -.01);
		_.plane.select(_.plane.recent.br).set_vert(bb.w, bb.h, -.01);
		_.bounds = bb;
	}
	var o = {
		location : v3(0),
		rotation : v3(0),
		scale	 : v3(1),
		rad		 : 0,
		pad		 : 0,
		layout	 : element.RELATIVE,
		anchor	 : element.LEFT,
		visible	 : 1,
		fg		 : color4(c_dwhite),
		bg		 : color4(c_transparent),

		append		: element,
		
		// these funs have _on_* versions for users, assigned via proxy
		on_release	: on_release,
		on_press	: on_press,
		on_hover	: on_hover,
		on_select	: on_select,
		on_deselect	: on_deselect,
		on_leave	: on_leave,
		on_update	: on_update,
	};
	if (loc) o.location = v3(loc.x, loc.y, loc.z);
	if (rot) o.rotation = v3(rot.x, rot.y, rot.z);
	if (sca) o.scale	= v3(sca.x, sca.y, sca.z);
	const handler = {
		set: function (o, p, v) {
			// overrides
			if (p == 'on_press') o._on_press = v;
			if (p == 'on_release') o._on_release = v;
			else {
				o[p] = v;
				if (['collection'].indexOf(p) > -1) { // temp fix
					o.text[p] = v;
				}
				// triggers
				if (['w', 'h', 'pad', 'anchor'].indexOf(p) > -1) o.on_update();
				if (['bg', 'fg'].indexOf(p) > -1) on_update_colors(o);
			}
			return true;
		}
	};
	o = new Proxy(o, handler); // proxy is passed to scene.* events this way
	o = scene.add(name, ELEMENT, o, this.type == ELEMENT ? this : undefined);
//	if (!name) o.name = 'element.'+o.guid;
	o.plane = mesh(name).add_plane(1);
	scene.reparent(o, o.plane);
	o.plane.selectable = 0;
	o.text = text3d(0, 0, 0, v3(.5));
	scene.reparent(o, o.text);
	o.text.selectable = 0;
	o.text.raycastable = 0;
	o.text.on_update = function () { o.on_update(); };
	o.text.add(name);
	
	// reroute hover/press/rel events from children to parent
	o.plane.reroute_events = 1;
	o.text.reroute_events = 1;
	
	on_update_colors(o);
	return o;
};
element.RELATIVE = 1;
element.ABSOLUTE = 2;
element.LEFT = 20;
element.RIGHT = 21;
element.CENTER = 22;
element.TOP = 23;
element.MIDDLE = 24;
element.BOTTOM = 25;
hooks.set('scene.pressed', function (o, k, m) { if (o.type == ELEMENT) {
} });
hooks.set('scene.info', function (o) { if (o.type == ELEMENT) {
	var b = o.text.bounds;
	var str = '', bb = o.text.bounds;
	var l = o.get('location');
	var r = o.get('rotation');
	var s = o.get('scale');
	str += 'loc x'+l.x+' y'+l.y+' z'+l.z
		+';  rot x'+r.x+' y'+r.y+' z'+r.z
		+';  sca x'+s.x+' y'+s.y+' z'+s.z
		+';  w'+bb.w+' h'+bb.h;
	return str;
} });
hooks.set('scene.draw', function (o, selected) { if (o.type == ELEMENT) {
	var yes = 0, a = o.animate;
	if (a) {
		animatables.forEach(function (prop) { if (a[prop]) {
			for (var i in a[prop]) {
				if (a[prop][i]) {
					if ( animate(a[prop], i, o[prop]) ) yes = 1;
				}
			}
		} });
		['pad'].forEach(function (prop) { if (a[prop]) {
			if ( animate(a, prop, o) ) yes = 1;
		} });
	}
	return yes;
} });
hooks.set('scene.selector', function (o, selector) { if (o.type == ELEMENT) {
} });
hooks.set('scene.raycast', function (o, ray) { if (o.type == ELEMENT) {
	if (o.bounds) {
		var camera = get_active_camera();
		var mat = scene.to_mat4(o);
		var d = v3( camera.project(ray.x, ray.y, mat) );
		var bb = o.bounds;
		var yes = v3.inside_rect(d.x, d.y, bb.x, bb.y, bb.w, bb.h);
		return yes;
	}
} });
hooks.set('scene.move', function (o, transform) { if (o.type == ELEMENT) {
	o.location.transform = v3.add(o.posed('location'), transform);
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == ELEMENT) {
	o.rotation.transform = v3.add(o.posed('rotation'), transform);
} });
hooks.set('scene.scale', function (o, transform) { if (o.type == ELEMENT) {
	o.scale.transform = v3.add(o.posed('scale'), transform);
} });
hooks.set('scene.apply', function (o) { if (o.type == ELEMENT) {
	if (o.location.transform) {
		o.location.pose = o.location.transform;
		o.location.transform = 0;
	}
	if (o.rotation.transform) {
		o.rotation.pose = o.rotation.transform;
		o.rotation.transform = 0;
	}
	if (o.scale.transform) {
		o.scale.pose = o.scale.transform;
		o.scale.transform = 0;
	}
} });
hooks.set('scene.cancel', function (o) { if (o.type == ELEMENT) {
	o.location.transform = 0;
	o.rotation.transform = 0;
	o.scale.transform = 0;
} });
hooks.set('scene.reset', function (o) { if (o.type == ELEMENT) {
	o.location.pose = 0;
	o.rotation.pose = 0;
	o.scale.pose = 0;
} });
hooks.set('scene.remove', function (o) { if (o.type == ELEMENT) {
	if (o.text) scene.remove(o.text.guid);
	if (o.plane) scene.remove(o.plane.guid);
} });
hooks.run('element.ready');

})();
