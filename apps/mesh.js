/* TODO
 * 
 * */
var mesh, MESH = scene.type('mesh');
;(function(){
	menu.add(0, 'mesh');
	menu.add('mesh_cube', 0, function () {
		var o = mesh();
		scene.select(o.uid);
	});

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
	var aggregate_verts = function (obj) {
		var uid = obj.recent.uid;
		var verts = [];
		if (!isundef(uid)) {
			obj.selection.forEach(function (o) {
				var tri = sgl.tri_get(uid, o);
				push_if_not_there(verts, tri.x);
				push_if_not_there(verts, tri.y);
				push_if_not_there(verts, tri.z);
			});
		}
		return verts;
	};
	var mesh_uid = 0;
	var add_plane = function (m) {
		var uid = sgl.add(); // TODO err checking
		this.sgl_uids.push( uid );
		this.recent = {};
		this.recent.uid = uid;
		m = m || 100;
		var verts =
		  // 0 front, bottom, right
		[ m/2,		 0,	 -m/2,
		  // 1 front, bottom, left
		  -m/2,		 0,	 -m/2,
		  // 4 back,  bottom, right
		  m/2,		 0,	  m/2,
		  // 5 back,  bottom, left
		  -m/2,		 0,	  m/2 ];
		var tris = []
		.concat(1, 2, 0, 3, 2, 1)
		;
		this.recent.all = [ 0, 1 ];
		for (var i = 0; i < verts.length; i+=3) {
			sgl.vert_add(uid, verts[i+0], verts[i+1], verts[i+2]);
		}
		for (var i = 0; i < tris.length; i+=3) {
			sgl.tri_add(uid, tris[i+0], tris[i+1], tris[i+2]);
		}
		this.update();
		return this;
	};
	var add_cube = function (m) {
		var uid = sgl.add(); // TODO err checking
		this.sgl_uids.push( uid );
		this.recent = {};
		this.recent.uid = uid;
		m = m || 100;
		var verts = // 0 front, bottom, right
		[ m/2, -m/2, -m/2,
		  // 1 front, bottom, left
		  -m/2, -m/2, -m/2,
		  // 2 front, top,    right
		  m/2,  m/2, -m/2,
		  // 3 front, top,    left
		  -m/2,  m/2, -m/2,
		  // 4 back,  bottom, right
		  m/2, -m/2,  m/2,
		  // 5 back,  bottom, left
		  -m/2, -m/2,  m/2,
		  // 6 back,  top,    right
		  m/2,  m/2,  m/2,
		  // 7 back,  top,    left
		  -m/2,  m/2,  m/2 ];
		this.recent.front  = [  0,  1 ];
		this.recent.right  = [  2,  3 ];
		this.recent.back   = [  4,  5 ];
		this.recent.left   = [  6,  7 ];
		this.recent.top    = [  8,  9 ];
		this.recent.bottom = [ 10, 11 ];
		var tris = []
		.concat(3, 0, 2, 1, 0, 3, // front
				0, 4, 2, 2, 4, 6, // right
				4, 5, 6, 7, 6, 5, // back
				3, 7, 1, 1, 7, 5, // left
				6, 3, 2, 7, 3, 6, // top
				1, 4, 0, 5, 4, 1) // bottom
		;
		this.recent.all = [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ];
		for (var i = 0; i < verts.length; i+=3) {
			sgl.vert_add(uid, verts[i+0], verts[i+1], verts[i+2]);
		}
		for (var i = 0; i < tris.length; i+=3) {
			sgl.tri_add(uid, tris[i+0], tris[i+1], tris[i+2]);
		}
		this.update();
		return this;
	};
	var apply_material = function (material) {
		var uid = this.recent.uid;
		if (!isundef(uid)) this.selection.forEach(function (o, i) {
			sgl.tri_material(uid, o   , material);
		});
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
//			var loc = this.location;
//			var loc = centroid(uid, verts);
//			_translate(uid, verts, -loc.x, -loc.y, -loc.z);
			verts.forEach(function (o) {
				var v = sgl.vert_get(uid, o);
				if (v) {
					v.x *= x||0;
					v.y *= y||0;
					v.z *= z||0;
					sgl.vert_set(uid, o, v.x, v.y, v.z);
				}
			});
//			_translate(uid, verts, loc.x, loc.y, loc.z);
		}
		return this;
	};
	var _update = function () {
		mesh.update(this);
	};
	mesh = function (name, location, rotation) {
		var o = {
			name     : name      || ++mesh_uid	,
			location : location  || v3(0)		,
			rotation : rotation  || v3(0)		,
			scale	 : v3(512)					,
			sgl_uids : []						,
			recent	 : {}						,

			selection		: []				,
			select			: select			,
			deselect		: deselect			,

			add_plane		: add_plane			,
			add_cube		: add_cube			,
			apply_material	: apply_material	,
			translate		: translate			,
			scale			: scale				,
			
			update			: _update			,
		};
		o = scene.add('mesh.'+o.name, MESH, o);
		o.update();
		return o;
	};
	mesh.update = function (o) {
		var s = o.get('location');
		var r = o.get('rotation');
		o.sgl_uids.forEach(function (o, i) {
			sgl.translation(o, s.x, s.y, s.z);
//			sgl.rotation(o, deg2sgl(r.x), deg2sgl(r.y), deg2sgl(r.z));
			sgl.rotation(o, r.x, r.y, r.z);
		});
	};
})();

hooks.set('scene.pressed', function (o, k, m) { if (o.type == MESH) {
} });
hooks.set('scene.info', function (o) { if (o.type == MESH) {
	var str = '';
	o.sgl_uids.forEach(function (o) {
		if (str) str += '\n';
		var l = sgl.translation(o);
		var r = sgl.rotation(o);
		str += o+'  loc x'+sgl2unit(l.x)+' y'+sgl2unit(l.y)+' z'+sgl2unit(l.z)
			+';  rot x'+sgl2deg(r.x)+' y'+sgl2deg(r.y)+' z'+sgl2deg(r.z);
	});
	return str;
} });
hooks.set('scene.draw', function (o, selected) { if (o.type == MESH) {
//	canvas.save();
//	o.update();
//	canvas.restore();
} });
hooks.set('scene.selector', function (o, selector) { if (o.type == MESH) {
} });
hooks.set('scene.move', function (o, transform) { if (o.type == MESH) {
	o.location.transform = v3.add(o.posed('location'), transform);
	o.update();
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == MESH) {
	o.rotation.transform = v3.add(o.posed('rotation'), transform);
	o.update();
} });
hooks.set('scene.scale', function (o, transform) { if (o.type == MESH) {
	o.scale.transform = v3.add(o.posed('scale'), transform);
	o.update();
} });
hooks.set('scene.apply', function (o) { if (o.type == MESH) {
	if (o.location.transform) {
		o.location.pose = o.location.transform;
		o.location.transform = 0;
		o.update();
	}
	if (o.rotation.transform) {
		o.rotation.pose = o.rotation.transform;
		o.rotation.transform = 0;
		o.update();
	}
} });
hooks.set('scene.cancel', function (o) { if (o.type == MESH) {
	o.location.transform = 0;
	o.rotation.transform = 0;
	o.update();
} });
hooks.set('scene.reset', function (o) { if (o.type == MESH) {
	o.location.pose = 0;
	o.rotation.pose = 0;
	o.update();
} });
hooks.set('scene.remove', function (o) { if (o.type == MESH) {
	if (o.sgl_uids) {
		o.sgl_uids.forEach(function (o) {
			sgl.remove(o);
		});
	}
} });






