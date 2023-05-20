/* IMPORTANT
 * whatever sgl uids you lease, hook them to scene.remove
 * */
var Stroke, STROKE = scene.type('stroke'), Point;
;(function(){
hooks.set('menu.ready', function () {
	menu.add(0, 'Strokes');
	menu.add('stroke_new', 'Add', function () {
		var o = Stroke();
		o.points = [
			Point(),
			Point(1, 0),
		];
		scene.select(o.guid);
	});
});
var stroke_uid = 0;
var point_uid = 0;
var circle_slice = 60;
var circle_tris = (360 / circle_slice + 1) * 3;

Point = function (x, y, z, r, c) { // adds radius + color to v3 {}
	var point = v3(x, y, z);
	point.guid = ++point_uid; // for tracking sub-selection
	point.r = r;
	point.c = c;
	return new Proxy(point, {
		set: function (o, p, v) {
			if (p != 'dirty' && o[p] != v) o.dirty = 1;
			o[p] = v;
			return true;
		}
	});
};

function set_segment(_, index, v1, v2) {
	var tv1 = v1.transform || v1;
	var tv2 = v2.transform || v2;

	var x0 = tv1.x, y0 = tv1.y, z0 = tv1.z,
		x1 = tv2.x, y1 = tv2.y, z1 = tv2.z,
		dx = Math.abs(x1 - x0),
		dy = Math.abs(y1 - y0);

	var ang = matrix.coord2angle( x0, y0, x1, y1 );

	var sw = v1.r || .02;
	var ew = v2.r || .02;
	var c1 = v1.c || c_white;
	var c2 = v2.c || c_white;

	if (v1.scale	) sw += v1.scale.x;
	if (v1.hover	) sw += .05;
	if (v1.selected	) sw += .05;
	if (v2.scale	) ew += v2.scale.x;
	if (v2.hover	) ew += .05;
	if (v2.selected	) ew += .05;

	var a, b, c, d;
	a = matrix.angle2coord(x0, y0, sw, ang+90 );
	b = matrix.angle2coord(x0, y0, sw, ang-90 );
	c = matrix.angle2coord(x1, y1, ew, ang-90 );
	d = matrix.angle2coord(x1, y1, ew, ang+90 );

	var p = x0 < x1 ? x0 : x1,
		q = y0 < y1 ? y0 : y1;

	p = (p-sw); q = (q-ew);

	var arr = [ // 12
//		 x		, y		, 	z,
		a.x		, a.y	, 	z0, // bl s
		b.x		, b.y	, 	z0, // br s
		c.x		, c.y	, 	z1, // tr e
		d.x		, d.y	, 	z1, // tl e
	];

	var vert_offset = index * 12;
	var col_offset  = index * 16;
	var tri_offset  = index *  6;

	// TODO fix the <= 12 part, clearly there's a bug somewhere
	for (var i = 0; i < 12; ++i) {
		_.segments.verts[vert_offset+i] = arr[i];
	}

	arr = [];
	
	var c1 = v1.c || c_white,
		c2 = v2.c || c_white;
	
		 if (v1.selected) c1 = c_dgreen;
	else if (v1.hover	) c1 = c_green;
		 if (v2.selected) c2 = c_dgreen;
	else if (v2.hover	) c2 = c_green;
	
	for (var i = 0; i < 2; ++i) arr = arr.concat(c1);
	for (var i = 0; i < 2; ++i) arr = arr.concat(c2);

	for (var i = 0; i < 16; ++i) { // 4 * 4 = 16
		_.segments.colors[col_offset+i] = arr[i];
	}

	var offset = vert_offset/3; // 1st seg calc: 12 / 3 = 4
	arr = [
		offset+0,  offset+1,  offset+2,
		offset+2,  offset+3,  offset+0
	];
	for (var i = 0; i < 6; ++i) {
		_.segments.tris[tri_offset+i] = arr[i];
	}
}
function set_circle(_, index, o) {
	if (_.point_size == -1) return;
	var r = o.r || .025, c1 = o.c || c_white;
		 if (o.selected ) c1 = c_dgreen;
	else if (o.hover	) c1 = c_green;
	if (o.scale		) r += o.scale.x;
	if (o.hover		) r += .1;
	if (o.selected	) r += .1;

	var point_size = !isnum(o.point_size) ? _.point_size : o.point_size;
	if (isnum(point_size) && point_size > 0) r = r+point_size;
	var i = 0, segs = 360 / circle_slice;
	var buf = [], col = [], tris = [];
	var obj = o.transform || o;
	buf.push( obj.x, obj.y, obj.z );
	col = col.concat(c1);
	for (var t = 0; t <= 360; t += circle_slice) {
		var p = matrix.angle2coord(obj.x, obj.y, r, t+90 );
		v3.precision(p);
		buf.push( p.x, p.y, obj.z );
		i++;
		col = col.concat(c1);
	}
	var offset = index * buf.length/3;
	for (var i = 0; i <= segs; ++i) {
		tris.push( offset+0, offset+i, offset+i+1 );
	}
	
	var vert_offset = index * buf.length;
	var col_offset  = index * col.length;
	var tri_offset  = index * circle_tris;

	// TODO fix bug <= buf, prolly a bug in meshbuf
	for (var i = 0; i < buf.length; ++i) {
		_.dots.verts[vert_offset+i] = buf[i];
	}
	for (var i = 0; i < col.length; ++i) {
		_.dots.colors[col_offset+i] = col[i];
	}
	for (var i = 0; i < circle_tris; ++i) {
		_.dots.tris[tri_offset+i] = tris[i];
	}
}
function apply_color(c) {
	var uid = this.colors;
	if (!isundef(uid))
		this.selection.forEach(function (o, i) {
			sgl.array_sub_buffer(uid, o*4, new Float32Array([c.r, c.g, c.b, c.a]));
		});
	return this;
};
function select(arr, add) { // [p0, p1, ...]
	if (!add) this.deselect();
	for (var i in arr) {
		this.selection.push(arr[i]);
	}
	return this;
};
function deselect() {
	this.selection = [];
	return this;
};
function upload(renew) { // TODO optimize
	var _ = this;
	if (renew) {
		point_uid = 0;
		_.segments.renew();
		_.dots.renew();
	}
	for (var i = 1; i < _.points.length; ++i) {
		var a = _.points[i-1];
		var b = _.points[i];
		if (a && b && (a.dirty || b.dirty || renew))
			set_segment(_, i-1, a, b);
	}
	for (var i = 0; i < _.points.length; ++i) {
		var a = _.points[i];
		if (a && (a.dirty || renew)) {
			if (renew) a.guid = 1+i;
			set_circle(_, i, a);
			a.dirty = 0;
		}
	}
	_.segments.upload();
	_.dots.upload();
	composer.damage();
}
Stroke = function (name, loc, rot, sca) {
	var o = {
		name     : name      || ++stroke_uid,
		location : v3(0)					,
		rotation : v3(0)					,
		scale	 : v3(1,1,1)				,
		points	 : []						,
		recent	 : {}						,
		
		segments : MeshBuffer(),
		dots : MeshBuffer(),

		point_size: 0, // -1 = dont draw points

		selection		: []				,
		select			: select			,
		deselect		: deselect			,
		upload			: upload			,
	};
	if (loc) o.location = v3(loc.x, loc.y, loc.z);
	if (rot) o.rotation = v3(rot.x, rot.y, rot.z);
	if (sca) o.scale	= v3(sca.x, sca.y, sca.z);
	o = scene.add('stroke.'+o.name, STROKE, o);
	return o;
};

hooks.set('scene.pressed', function (o, k, m) { if (o.type == STROKE) {
	var yes;
	if (!m.state && scene.edit_mode) {
		if (k == 'e') {
			var p = o.points[ o.points.length-1 ];
			if (p) {
				p = Point(p.x+1, p.y, p.z);
			} else {
				p = Point();
			}
			if (Pointer.last) {
				var cam = get_active_camera();
				var q = cam.project(Pointer.last.x, Pointer.last.y);
				var p3 = vec3.fromValues(p.x, p.y, p.z);
				
				p.x = q[0];
				p.y = q[1];
				p.z = q[2];
			}
			p.selected = 1;
			o.points.push( p );
			o.upload(1);
			yes = 1;
		}
	}
	return yes;
} });
hooks.set('scene.info', function (o) { if (o.type == STROKE) {
	var str = '';
	var l = o.get('location');
	var r = o.get('rotation');
	var sg = o.segments;
	var dt = o.dots;
	str += 'loc x'+l.x+' y'+l.y+' z'+l.z
		+';  rot x'+r.x+' y'+r.y+' z'+r.z
		+'\npoints '+o.points.length
		+'\nverts ' +sg.verts.length
		+'\ncolors '+sg.colors.length
		+'\ntris '  +sg.tris.length
		+'\nsegs '+(sg.tris.length+sg.verts.length+sg.colors.length)+'/'+sg.cap
		+'\ndots '+(dt.tris.length+dt.verts.length+dt.colors.length)+'/'+dt.cap;
	return str;
} });
hooks.set('scene.draw', function (o, selected) { if (o.type == STROKE) {
	var mat = scene.to_mat4(o);
	sgl.uniform_matrix_4fv(uniforms.model, mat);
	var wire_mode = scene.wire_mode || o.wire;
	if (scene.picking_mode) {
		wire_mode = 0;
		sgl.uniform_1ui(uniforms.object_id, o.guid );
	} else {
		sgl.uniform_1i(uniforms.selected, selected ? 1 : 0);
	}

	if (o.image) {
		sgl.active_texture(sgl.GL_TEXTURE0);
		sgl.bind(sgl.GL_TEXTURE_2D, o.image);
		sgl.uniform_1f(uniforms.has_color, 2);
	} else
		sgl.uniform_1f(uniforms.has_color, 1);

	if (wire_mode) sgl.polygon_mode( sgl.GL_FRONT_AND_BACK, sgl.GL_LINE );

	if (scene.edit_mode && scene.picking_mode) {
		for (var i = 0; i < o.points.length; ++i) {
			sgl.uniform_1ui(uniforms.object_id, to32bits(o.guid, o.points.length+o.points[i].guid) );
			o.segments.draw(i*6, 6);
		}
		for (var i = 0; i < o.points.length; ++i) {
			sgl.uniform_1ui(uniforms.object_id, to32bits(o.guid, o.points[i].guid) );
			o.dots.draw(i*circle_tris, circle_tris);
		}
	} else {
		o.segments.draw();
		o.dots.draw();
		var cam = get_active_camera();
		for (var i = 0; i < o.points.length; ++i) {
			var p = o.points[i];
			var t = p.transform || p;
			var q = cam.unproject(t.x, t.y, t.z, mat);
			text(q.x, q.y, p.guid);
		}
	}

	if (wire_mode) sgl.polygon_mode( sgl.GL_FRONT_AND_BACK, sgl.GL_FILL );
	sgl.uniform_1f(uniforms.has_color, 0);
	sgl.uniform_1i(uniforms.selected, 0);

	var frames = 0, a = o.animate;
	if (a)
 		animatables.forEach(function (prop) {
			if (a[prop])
			for (var i in a[prop]) {
				if (isarr(a[prop][i])) {
					frames += animate(a[prop], i, o[prop]);
				}
			}
		});
	return frames ? 1 : 0;
} });
hooks.set('scene.selector', function (o, selector) { if (o.type == STROKE) {
} });
hooks.set('scene.select_all', function (o) { if (o.type == STROKE) {
	for (var i = 0; i < o.points.length; ++i) {
		var p = o.points[i];
		if (!p.selected) {
			p.selected = 1;
		}
	}
	o.upload();
} });
hooks.set('scene.deselect', function (o) { if (o.type == STROKE) {
	for (var i = 0; i < o.points.length; ++i) {
		var p = o.points[i];
		if (p.selected) {
			p.selected = undefined;
		}
	}
	o.upload();
} });
hooks.set('scene.raycast', function (o, m, picked) { if (o.type == STROKE) {
	if (!picked) return;
	
	var uids = to16bits(picked.r), u2;
	if (uids[1] > o.points.length) {
		u2 = uids[1]-o.points.length;
		uids[1] = u2+1;
	}
	for (var i = 0; i < o.points.length; ++i) {
		var p = o.points[i];
		if (uids[0] == o.guid && (p.guid == uids[1] || p.guid == u2)) {
			if (m.state == -1) {
				if (isundef(u2)) p.selected = !p.selected;
				else p.selected = 1;
			}
			p.hover = 1;
		} else {
			if (!m.shift && m.state == -1 && p.selected) p.selected = undefined;
			if (p.hover) p.hover = undefined;
		}
	}
	o.upload();
} });
hooks.set('scene.move', function (o, transform) { if (o.type == STROKE) {
	if (scene.edit_mode) {
		for (var i = 0; i < o.points.length; ++i) {
			var p = o.points[i];
			if (p.selected) {
				p.transform = v3.add(p, transform);
			}
		}
		o.upload();
	} else
	o.location.transform = v3.add(o.posed('location'), transform);
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == STROKE) {
	o.rotation.transform = v3.add(o.posed('rotation'), transform);
} });
hooks.set('scene.scale', function (o, transform) { if (o.type == STROKE) {
	if (scene.edit_mode) {
		for (var i = 0; i < o.points.length; ++i) {
			var p = o.points[i];
			if (p.selected) {
				p.scale = transform;
			}
		}
		o.upload();
	} else
	o.scale.transform = v3.add(o.posed('scale'), transform);
} });
hooks.set('scene.apply', function (o) { if (o.type == STROKE) {
	if (scene.edit_mode) {
		for (var i = 0; i < o.points.length; ++i) {
			var p = o.points[i];
			if (p.transform) {
				v3.copy(p.transform, p);
				p.transform = undefined;
			}
			if (p.scale) {
				p.r = p.scale.x;
				p.scale = undefined;
			}
		}
		o.upload();
	}
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
hooks.set('scene.cancel', function (o) { if (o.type == STROKE) {
	if (scene.edit_mode) {
		for (var i = 0; i < o.points.length; ++i) {
			var p = o.points[i];
			if (p.transform) {
				p.transform = undefined;
			}
			if (p.scale) {
				p.scale = undefined;
			}
		}
		o.upload();
	}
	o.location.transform = 0;
	o.rotation.transform = 0;
	o.scale.transform = 0;
} });
hooks.set('scene.reset', function (o) { if (o.type == STROKE) {
	o.location.pose = 0;
	o.rotation.pose = 0;
	o.scale.pose = 0;
} });
hooks.set('scene.remove', function (o) { if (o.type == STROKE) {
	if (scene.edit_mode) {
		o.points = o.points.filter(function (p, i) {
			return p.selected ? 0 : 1;
		});
		o.upload(1);
	} else {
		o.segments.remove();
		o.dots.remove();
	}
} });


})();




