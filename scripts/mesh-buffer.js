var MeshBuffer, Batch;
;(function(){
// TODO Range, rangify as the indices come, prolly be faster
Batch = function (pre_arr) { // track what indices of modded vals in an array
	var dirty = [], arr = [], proxy;
	if (pre_arr) {
		for (var i = 0; i < pre_arr.length; ++i) {
			arr[i] = pre_arr[i];
			dirty = 1;
		}
	}
	arr.get_vec3 = function (i) {
		i *= 3;
		return vec3.fromValues(
			arr[i + 0],
			arr[i + 1],
			arr[i + 2]
		);
	};
	arr.set_vec3 = function (i, v) {
		i *= 3;
		arr[i + 0] = v[0];
		arr[i + 1] = v[1];
		arr[i + 2] = v[2];
		dirty = 1;
	};
	arr.push_array = function (a) {
		for (var i = 0; i < a.length; ++i) {
			proxy.push(a[i]);
		}
		return proxy;
	};
	// TODO .push messes up the range
	arr.rangify = function () { // returns slicable pairs [5, 6], [10, 12]
		// if all indices are dirty, just return the full range
		if (dirty == 1 || dirty.length == arr.length) {
			dirty = [];
			return [[0, arr.length]];
		}
		
		var ranges = [], start, end;
		if (isarr(dirty)) {
			dirty.sort(function (a, b) {
				return a - b;
			});
			for (var i = 0; i < dirty.length; i++) {
				start = dirty[i];
				end = start;
				while (dirty[i + 1] - dirty[i] == 1) {
					end = dirty[i + 1]; // increment the index if the numbers sequential
					i++;
				}
//				if (end == dirty.length-1) end++; // fix to include the last index
				ranges.push(start == end ? [start, start+1] : [start, end]);
			}
			dirty = [];
		}
		return ranges;
	};
	proxy = new Proxy(arr, {
		// takes unordered indices, returns ordered ranges
		set: function (o, p, v) {
			if (['dirty', 'rangify', 'push_array', 'get_vec3', 'set_vec3'].indexOf(p) > -1) return 0;
			if (!isarr(dirty)) dirty = [];
			if (dirty.indexOf(p) == -1 && ['length'].indexOf(p) == -1) {
				dirty.push(p);
			}
			o[p] = v;
			return 1;
		}
	});
	return proxy;
}
MeshBuffer = function (cap, stride) {
	cap = cap || 1024;
	var ready = 0;
	var uids = {
		array: sgl.vertex_array(),
		verts: sgl.array_buffer( new Float32Array(cap) ),
	};
	var mb = {
		cap   : cap,
		tris  : Batch(),
		verts : Batch(),
		colors: Batch(),
		norms : Batch(),
		uv    : Batch(),
		upload: upload,
		stride: stride || 3,
		renew		: renew,
		draw		: draw,
		draw_range  : draw_range,
		draw_arrays : draw_arrays,
		set_image: set_image,
		remove: remove,
	};
	function set_image(image) {
		if (isundef(image)) return;
		sgl.bind(sgl.GL_VERTEX_ARRAY, uids.array);

		var format = sgl.GL_RGB;
		if (image.n == 1) format = sgl.GL_RED;
		if (image.n == 2) format = sgl.GL_RG;
		if (image.n == 4) format = sgl.GL_RGBA;

		uids.image = sgl.texture(uids.image, sgl.GL_TEXTURE_2D, 0, format,
				image.w, image.h, format, sgl.GL_UNSIGNED_BYTE, image.data);

		sgl.generate_texture_mipmap(uids.image);
		sgl.bind(sgl.GL_VERTEX_ARRAY, 0);
	}
	function renew() {
		mb.tris  = Batch();
		mb.verts = Batch();
		mb.colors= Batch();
		mb.norms = Batch();
		mb.uv    = Batch();
	}
	function remove() {
		if (!isundef(uids.image)) sgl.delete_textures(new Float32Array([uids.image]));
		sgl.delete_vertex_arrays(new Uint32Array( [uids.array] ));
		sgl.delete_buffers(new Uint32Array( [uids.verts] ));
	}
	function draw(start, end, type) { if (!ready) return;
		if (uids.image) {
			sgl.active_texture(sgl.GL_TEXTURE0);
			sgl.bind(sgl.GL_TEXTURE_2D, uids.image);
			sgl.uniform_1f(uniforms.has_color, 2);
		} else if (mb.diffuse || mb.diffuse_override) {
			var d = mb.diffuse_override || mb.diffuse;
			sgl.uniform_1f(uniforms.has_color, 0);
			sgl.uniform_4f(uniforms.single_color, d[0], d[1], d[2], d[3]);
		} else if (mb.colors.length) {
			sgl.uniform_1f(uniforms.has_color, 1);
		}

		sgl.bind(sgl.GL_VERTEX_ARRAY, uids.array);
		if (mb.tris.length) {
			sgl.draw_elements(sgl.GL_TRIANGLES, end || mb.tris.length, 0, start || 0);
		} else if (mb.verts.length) {
				 if (mb.line_strip) type = sgl.GL_LINE_STRIP;
			else if (mb.line_strip_adjacency ) type = sgl.GL_LINE_STRIP_ADJACENCY	;
			else if (mb.line_loop ) type = sgl.GL_LINE_LOOP	;
			else if (mb.lines	  ) type = sgl.GL_LINES		;
			sgl.draw_arrays(type || sgl.GL_TRIANGLES, mb.verts.length / mb.stride || num);
		}
		sgl.bind(sgl.GL_VERTEX_ARRAY, 0);

		if (mb.colors.length || uids.image) sgl.uniform_1f(uniforms.has_color, 0);
	}
	function draw_range(start, end) {
		if (ready && mb.tris.length) {
			sgl.bind(sgl.GL_VERTEX_ARRAY, uids.array);
			sgl.draw_range(sgl.GL_TRIANGLES, start, end || mb.tris.length);
			sgl.bind(sgl.GL_VERTEX_ARRAY, 0);
		}
	}
	function draw_arrays(type, num) { if (!ready) return;
		sgl.bind(sgl.GL_VERTEX_ARRAY, uids.array);
		sgl.draw_arrays(type || sgl.GL_LINES, mb.verts.length / mb.stride || num);
		sgl.bind(sgl.GL_VERTEX_ARRAY, 0);
	}
	function update_capacity() {
		ready = 1;
		var size_current = mb.tris.length + mb.verts.length + mb.colors.length
						 + mb.norms.length+ mb.uv.length;
		if (size_current > mb.cap) {
			sgl.delete_buffers( new Float32Array( [uids.verts] ) );
			mb.cap = size_current + 1024;
			uids.verts = sgl.array_buffer( new Float32Array( mb.cap ) );
		}
	}
	function upload(debug) {
		// BUG updating norms apparently unsets everything else :?/
		update_capacity();
		
		sgl.bind(sgl.GL_VERTEX_ARRAY, uids.array);

		if (mb.tris.length)
		mb.tris.rangify().forEach(function (range) {
			if (range.length == 0) return;
			var start = range[0],
				end   = range[1]+1;
			if (debug) log('tris', end-start-1);
			sgl.array_sub_buffer( uids.verts, start, new Uint16Array( mb.tris.slice(start, end) ), 2 );
		});

		mb.verts.rangify().forEach(function (range) {
			if (range.length == 0) return;
			var start = range[0],
				end   = range[1]+1;
			if (debug) log('verts', end-start-1);
			sgl.array_sub_buffer( uids.verts, mb.tris.length + start, new Float32Array( mb.verts.slice(start, end) ) );
		});

		if (mb.colors.length)
		mb.colors.rangify().forEach(function (range) {
			if (range.length == 0) return;
			var start = range[0],
				end   = range[1]+1;
			if (debug) log('colors', end-start-1);
			sgl.array_sub_buffer( uids.verts, mb.tris.length + mb.verts.length + start, new Float32Array( mb.colors.slice(start, end) ) );
		});

		if (mb.uv.length)
		mb.uv.rangify().forEach(function (range) {
			if (range.length == 0) return;
			var start = range[0],
				end   = range[1]+1;
			if (debug) log('uv', end-start-1);
			sgl.array_sub_buffer( uids.verts, mb.tris.length + mb.verts.length + mb.colors.length + start, new Float32Array( mb.uv.slice(start, end) ) );
		});

		if (mb.norms.length)
		mb.norms.rangify().forEach(function (range) {
			if (range.length == 0) return;
			var start = range[0],
				end   = range[1]+1;
			if (debug) log('norms', end-start-1);
			sgl.array_sub_buffer( uids.verts, mb.tris.length + mb.verts.length + mb.colors.length + mb.uv.length + start, new Float32Array( mb.norms.slice(start, end) ) );
		});

		if (mb.verts.length)
		sgl.enable_array_buffer(attributes.coord3d, uids.verts, mb.stride, mb.stride, mb.tris.length);
		
		if (mb.colors.length)
		sgl.enable_array_buffer(attributes.v_color, uids.verts, 4, 4, mb.tris.length + mb.verts.length);

		if (mb.uv.length)
		sgl.enable_array_buffer(attributes.textu2dpre, uids.verts, 2, 2, mb.tris.length + mb.verts.length + mb.colors.length);
		
		if (mb.norms.length)
		sgl.enable_array_buffer(attributes.v_normal, uids.verts, 3, 3, mb.tris.length + mb.verts.length + mb.colors.length + mb.uv.length);

		if (mb.tris.length) sgl.bind(sgl.GL_ELEMENT_ARRAY_BUFFER, uids.verts);

		sgl.bind(sgl.GL_VERTEX_ARRAY, 0);
		sgl.bind(sgl.GL_ARRAY_BUFFER, 0);

		if (mb.tris.length) sgl.bind(sgl.GL_ELEMENT_ARRAY_BUFFER, 0);

		sgl.disable_array_buffer(attributes.coord3d);
		sgl.disable_array_buffer(attributes.v_color);
		sgl.disable_array_buffer(attributes.textu2dpre);
		sgl.disable_array_buffer(attributes.v_normal);
		
		if (mb.colors.length) sgl.disable_array_buffer(attributes.v_color);
		
		if (mb.uv.length) sgl.disable_array_buffer(attributes.textu2dpre);
	}
	return new Proxy(mb, {
		set: function (o, p, v) {
			var change = o[p] != v;
			o[p] = v;
			if (change) {
				if (p == 'cap') update_capacity();
			}
			return 1;
		}
	});
}
})();