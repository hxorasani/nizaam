/* TODO
 * 
 * */
var mesh, Mesh, MESH = scene.type('mesh');
;(function(){
var uni = {
	vertex_outline	: sgl.get_uniform("vertex_outline",	glsl.pre),
	vertex_fill		: sgl.get_uniform("vertex_fill",	glsl.pre),
	vertex_size		: sgl.get_uniform("vertex_size",	glsl.pre),
};
hooks.set('menu.ready', function () {
	menu.add(0, 'mesh');
	menu.add('mesh_cube', 0, function () {
		var o = mesh();
		o.add_cube();
		scene.select(o.guid);
	});
});
var mesh_uid = 0;

var push_if_not_there = function (arr, i) { if (arr.indexOf(i) == -1) arr.push(i) };
var centroid = function (uid, verts) {
	var x = y = z = 0;
	verts.forEach(function (o) {
		var v = sgl.vert_get(uid, o);
		if (v) {
			x += v.x;
			y += v.y;
			z += v.z;
		}
	});
	return {x:x/verts.length, y:y/verts.length, z:z/verts.length};
};
var add = function (verts, colors_arr, tris) {
	var _ = this;
	_.mb.verts = Batch(verts);
	_.mb.colors = Batch(colors_arr);
	_.mb.tris = Batch(tris);
	_.mb.upload();
	return _;
};
var set_image = function (image) {
	var _ = this, tex;
	if (image) {
		_.mb.uv = Batch([
			0.0,  0.0, // bl
			1.0,  0.0, // br
			1.0,  -1.0, // tr
			0.0,  -1.0, // tl
		]);
		
		_.mb.set_image(image);

		if (_.recent) {
			_.recent.size = image.data.length;
			_.recent.w = image.w;
			_.recent.h = image.h;
			_.recent.n = image.n;
		}
		
		_.mb.upload(1);
	}
	return _;
};
var add_plane = function (m, color) {
	var _ = this, verts, tris, colors_arr = [];
	m = m || 1;
	verts = [
	//	 x		, y		, 	z,
		-m		,-m		, 	0, // bl
		 m		,-m		, 	0, // br
		 m		, m		, 	0, // tr
		-m		, m		, 	0, // tl
	];
	for (var i = 0; i < verts.length/3; ++i) {
		if (color) colors_arr = colors_arr.concat(color);
		else colors_arr.push(.9,.9,.9,1);
	}
	tris = [
		0,  1,  2,
		2,  3,  0,
	];

	_.recent = {};
	_.buffer = verts;
	
	_.recent.v1 = vec3.fromValues(verts[ 0], verts[ 1], verts[ 2]);
	_.recent.v2 = vec3.fromValues(verts[ 3], verts[ 4], verts[ 5]);
	_.recent.v3 = vec3.fromValues(verts[ 6], verts[ 7], verts[ 8]);
	_.recent.v4 = vec3.fromValues(verts[ 9], verts[10], verts[11]);
	
	_.recent.all= [0,1, 2, 3];
	_.recent.tr = [2];
	_.recent.tl = [3];
	_.recent.br = [1];
	_.recent.bl = [0];

	_.add(verts, colors_arr, tris);
	return _;
};
var add_cube = function (m, color) {
	var _ = this;
	m = m || 1;
	var verts = [
		// front
		-m, -m,  m,
		 m, -m,  m,
		 m,  m,  m,
		-m,  m,  m,
		// top
		-m,  m,  m,
		 m,  m,  m,
		 m,  m, -m,
		-m,  m, -m,
		// back
		 m, -m, -m,
		-m, -m, -m,
		-m,  m, -m,
		 m,  m, -m,
		// bottom
		-m, -m, -m,
		 m, -m, -m,
		 m, -m,  m,
		-m, -m,  m,
		// left
		-m, -m, -m,
		-m, -m,  m,
		-m,  m,  m,
		-m,  m, -m,
		// right
		 m, -m,  m,
		 m, -m, -m,
		 m,  m, -m,
		 m,  m,  m,
	];
	var colors_arr = [];
	for (var i = 0; i < verts.length/3; ++i) {
		if (color) colors_arr = colors_arr.concat(color);
		else colors_arr.push(.9,.9,.9,1);
	}
	_.recent = {};
	_.recent.front  = [  0,  1, 2, 3 ];
	_.recent.top    = [  4,  5, 6, 7 ];
	_.recent.back   = [  8,  9,10,11 ];
	_.recent.bottom = [ 12, 13,14,15 ];
	_.recent.left   = [ 16, 17,18,19 ];
	_.recent.right  = [ 20, 21,22,23 ];
	var tris = [
		// front
		0,  1,  2,
		2,  3,  0,
		// top
		4,  5,  6,
		6,  7,  4,
		// back
		8,  9, 10,
		10, 11,  8,
		// bottom
		12, 13, 14,
		14, 15, 12,
		// left
		16, 17, 18,
		18, 19, 16,
		// right
		20, 21, 22,
		22, 23, 20,
	];
	_.recent.all = verts;
	_.add(verts, colors_arr, tris);
	return _;
};
var add_sphere = function (radius, slices, stacks) {
	var _ = this;
	var n = 2 * (slices + 1) * stacks;
	var i = 0;
	var points = [];
	var sin = Math.sin, cos = Math.cos;
  
	for (var theta = -M_PI / 2; theta < Math.PI / 2 - 0.0001; theta += Math.PI / stacks) {
		for (var phi = -M_PI; phi <= Math.PI + 0.0001; phi += 2 * Math.PI / slices) {
			points[i++] = vec3.clone( [cos(theta) * sin(phi), -sin(theta), cos(theta) * cos(phi)] );
			points[i++] = vec3.clone( [cos(theta + M_PI / stacks) * sin(phi), -sin(theta + M_PI / stacks), cos(theta + M_PI / stacks) * cos(phi)] );
		}
	}
  
	return _;
};
var apply_material = function (material) {
	var uid = this.recent.uid;
	if (!isundef(uid)) this.selection.forEach(function (o, i) {
		sgl.tri_material(uid, o, material);
	});
	return this;
};
var apply_color = function (c) {
	var colors = this.mb.colors;
	this.selection.forEach(function (o, i) {
		var index = o*4;
		colors[ index+0 ] = c.r;
		colors[ index+1 ] = c.g;
		colors[ index+2 ] = c.b;
		colors[ index+4 ] = c.a;
	});
	this.mb.upload();
	return this;
};
var set_vert = function (x, y, z) {
	if (isobj(x)) z = x.z, y = x.y, x = x.x;
	var verts = this.mb.verts;
	this.selection.forEach(function (o, i) {
		var index = o*3;
		verts[ index+0 ] = x;
		verts[ index+1 ] = y;
		verts[ index+2 ] = z;
	});
	this.mb.upload();
	return this;
};
var select = function (arr, add) { // [tri0, tri1, ...]
	if (!add) this.deselect();
	for (var i in arr) {
		this.selection.push(arr[i]);
	}
	return this;
};
var deselect = function () {
	this.selection = [];
	return this;
};
var _translate = function (uid, verts, x, y, z) {
	verts.forEach(function (o) {
		var v = sgl.vert_get(uid, o);
		if (v) {
			v.x += x||0;
			v.y += y||0;
			v.z += z||0;
			sgl.vert_set(uid, o, v.x, v.y, v.z);
		}
	});
};
var translate = function (x, y, z) {
	var uid = this.recent.uid;
	var verts = aggregate_verts(this);
	if (verts.length && (x||y||z)) {
		_translate(uid, verts, x, y, z);
	}
	return this;
};
var scale = function (x, y, z) {
	var uid = this.recent.uid;
	var verts = aggregate_verts(this);
	if (verts.length && (x||y||z)) {
//		var loc = this.location;
//		var loc = centroid(uid, verts);
//		_translate(uid, verts, -loc.x, -loc.y, -loc.z);
		verts.forEach(function (o) {
			var v = sgl.vert_get(uid, o);
			if (v) {
				v.x *= x||0;
				v.y *= y||0;
				v.z *= z||0;
				sgl.vert_set(uid, o, v.x, v.y, v.z);
			}
		});
//		_translate(uid, verts, loc.x, loc.y, loc.z);
	}
	return this;
};
mesh = function (name, loc, rot, sca) {
	var o = {
		name     : name      || ++mesh_uid	,
		location : v3(0)					,
		rotation : v3(0)					,
		scale	 : v3(1,1,1)				,
		mb		 : MeshBuffer()				,
		recent	 : {}						,
		wire: 0,

		selection		: []				,
		select			: select			,
		deselect		: deselect			,

		add				: add				,
		set_image		: set_image			,
		add_plane		: add_plane			,
		add_cube		: add_cube			,
		apply_material	: apply_material	,
		apply_color		: apply_color		,
		set_vert		: set_vert			,
		translate		: translate			,
	};
	if (loc) o.location = v3(loc.x, loc.y, loc.z);
	if (rot) o.rotation = v3(rot.x, rot.y, rot.z);
	if (sca) o.scale	= v3(sca.x, sca.y, sca.z);
	o = scene.add('mesh.'+o.name, MESH, o);
	return o;
};
Mesh = mesh;

hooks.set('scene.pressed', function (o, k, m) { if (o.type == MESH) {
} });
hooks.set('scene.info', function (o) { if (o.type == MESH) {
	var str = '';
	var l = o.get('location');
	var r = o.get('rotation');
	var s = o.get('scale');
	str += 'loc x'+l.x+' y'+l.y+' z'+l.z
		+'\nrot x'+r.x+' y'+r.y+' z'+r.z
		+'\nsca x'+s.x+' y'+s.y+' z'+s.z;
	var size = o.recent.size;
	if (size) str += '\nsize '+parsefloat(size / 1000 / 1000, 2)+'MB';
	return str;
} });
hooks.set('scene.draw', function (o, selected) { if (o.type == MESH) {
	var mat = scene.to_mat4(o); // TODO only calc once per frame
	var wire_mode = scene.wire_mode || o.wire;
	sgl.uniform_matrix_4fv(uniforms.transform, mat);

	if (scene.picking_mode) {
		wire_mode = 0;
		sgl.uniform_1ui(uniforms.draw_id, o.guid);
		sgl.uniform_1ui(uniforms.object_id, o.guid);
	} else {
		sgl.uniform_1i(uniforms.selected, selected ? 1 : 0);
	}
	if (scene.two_dee) wire_mode = 0;

	if (wire_mode) sgl.polygon_mode( sgl.GL_FRONT_AND_BACK, sgl.GL_LINE );

	o.mb.draw();
	sgl.uniform_1i(uniforms.selected, 0);
	
	if (wire_mode) sgl.polygon_mode( sgl.GL_FRONT_AND_BACK, sgl.GL_FILL );

	if (scene.picking_mode) {
		sgl.uniform_1ui(uniforms.draw_id, 0);
		sgl.uniform_1ui(uniforms.object_id, 0);
	}

	if (o.show_vertices) {
		sgl.uniform_1f(uni.vertex_size, 12*camera.zoom);
		sgl.uniform_4f(uni.vertex_fill, 1, .7, 0, 1);
		sgl.uniform_4f(uni.vertex_outline, .9, .6, .2, 1);
		sgl.uniform_1i(uniforms.show_vertices, 1);
		sgl.bind(sgl.GL_VERTEX_ARRAY, o.vert_arr);
		sgl.draw_elements(sgl.GL_POINTS, o.count);
		sgl.bind(sgl.GL_VERTEX_ARRAY, 0);
		sgl.uniform_1i(uniforms.show_vertices, 0);
	}

	var frames = 0, a = o.animate;
	if (a && !scene.picking_mode) // dont animate in picking mode
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
hooks.set('scene.selector', function (o, selector) { if (o.type == MESH) {
} });
hooks.set('scene.move', function (o, transform) { if (o.type == MESH) {
	o.location.transform = v3.add(o.posed('location'), transform);
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == MESH) {
	o.rotation.transform = v3.add(o.posed('rotation'), transform);
} });
hooks.set('scene.scale', function (o, transform) { if (o.type == MESH) {
	o.scale.transform = v3.add(o.posed('scale'), transform);
} });
hooks.set('scene.apply', function (o) { if (o.type == MESH) {
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
hooks.set('scene.cancel', function (o) { if (o.type == MESH) {
	o.location.transform = 0;
	o.rotation.transform = 0;
	o.scale.transform = 0;
} });
hooks.set('scene.reset', function (o) { if (o.type == MESH) {
	o.location.pose = 0;
	o.rotation.pose = 0;
	o.scale.pose = 0;
} });
hooks.set('scene.remove', function (o) { if (o.type == MESH) {
	if (o.mb) o.mb.remove();
} });


})();




