/*
 * provide 3d text boxes to all mods
 * */
/* TODO
 * make high quality dynamic texture atlases for fonts for fast 2d rendering
 * make visiblity calculations for 3d camera as well
 * */
require(Nizaam.scripts_path+'scripts/core/es6-shim.min.js');
require(Nizaam.scripts_path+'scripts/text/arabtype.js');
require(Nizaam.scripts_path+'scripts/text/bidi.js');
var bidi = bidi_js();
var is_harakah;
ttf.font("UbuntuMono-R*"); // DejaVuSans*|Ubuntu*|FreeSerif*|segoeui*|Cour*
var text3d, Text, TEXT3D = scene.type('text3d');
;(function(){

String.prototype.splice = function(startIndex,length,insertString){
	return this.substring(0,startIndex) + insertString + this.substring(startIndex + length);
};
hooks.set('menu.ready', function () {
	menu.add(0, 'text3d');
	menu.add('text3d', 0, function () {
		var o = text3d();
		o.add('Text');
		scene.select(o.guid);
	});
});
var font_cache = {}, stride = 3;
function load_char (c) {
	var cp = c.codePointAt(0), ch = font_cache[cp];
	if (!ch) {
		ch = font_cache[cp] = ttf.char(cp);
		if (ch) {
//			ch.mb = MeshBuffer(ch.verts.length+ch.faces.length, 2);
//			ch.mb.verts = Batch(ch.verts);
//			ch.mb.tris  = Batch(ch.faces);
//			log(ch.mb.tris.length, ch.faces.length);
//			ch.mb.upload(1);
//			log(c, ch.mb.cap);
			var length = ch.verts.length+ch.faces.length;
			if (ch.norms) {
				length += ch.norms.length;
			}
			ch.vao = sgl.vertex_array();
			ch.vbo = sgl.array_buffer  ( new Float32Array( length ) );
			sgl.array_sub_buffer( ch.vbo, 0, ch.faces, stride );
			sgl.array_sub_buffer( ch.vbo, ch.faces.length, ch.verts );

			if (ch.norms) sgl.array_sub_buffer( ch.vbo, ch.faces.length+ch.verts.length, ch.norms );

			sgl.bind(sgl.GL_ELEMENT_ARRAY_BUFFER, ch.vbo);
			sgl.enable_array_buffer(attributes.coord3d, ch.vbo, stride, stride, ch.faces.length);

			if (ch.norms) sgl.enable_array_buffer(attributes.v_normal, ch.vbo, stride, stride, ch.norms.length);

			sgl.bind(sgl.GL_VERTEX_ARRAY, 0);
			sgl.bind(sgl.GL_ARRAY_BUFFER, 0);
			sgl.bind(sgl.GL_ELEMENT_ARRAY_BUFFER, 0);
			sgl.disable_array_buffer(attributes.coord3d);

			if (ch.norms) sgl.disable_array_buffer(attributes.v_normal);
		} else { // not found
			ch = font_cache[cp] = {};
		}
		ch.c   = c;
		ch.cp  = cp;
	}
	return ch;
}
var text3d_uid = 0;
function update() {
	var fe = ttf.font();
	var t = this, ordered = t.string, perf = performance.now();
	var loc = t.get('location');
	var sca = t.get('scale');
	var line_height = fe.ascender-fe.descender;
	t.lines = [];
	ordered = arabtype( ordered );
	t.levels = []; // directions
	t.line_height = line_height;
	t.widths = [];
	t.indices = ordered.split('\n');
	t.bounds.w = 0; t.bounds.h = 0;
	for (var line = 0; line < t.indices.length; ++line) {
		var o = t.indices[ line ];
		var levels = bidi.getEmbeddingLevels( o );
		o = bidi.getReorderedString( o, levels );
		t.indices[ line ] = bidi.getReorderedIndices( o, levels );
		t.levels[ line ] = levels.levels;
		t.lines[ line ] = [];
		var w = 0, h = 0;
		o.split('').forEach(function (e, i) {
			var chr = load_char(e);
			chr.w = chr.w || 0;
			if (chr.c == ' ') chr.w = .3, chr.h = line_height;
			if (chr.c == '\t') chr.w = 1, chr.h = line_height;
			if (is_harakah(chr.cp)) chr.w = 0;
			w += chr.w;
			h = Math.max(h, chr.h);
			t.lines[ line ].push( chr );
		});
		t.widths[ line ] = w;
		t.bounds.w = Math.max(t.bounds.w, w);
//		if (line == 0) t.bounds.h -= line_height;
		t.bounds.h -= line_height;
	}
	t.bounds.w = parsefloat(t.bounds.w, 2)*sca.x;
	t.bounds.h = parsefloat(t.bounds.h, 2)*sca.y;
	t.perf = performance.now() - perf;
	if (isfun(t.on_update)) t.on_update();
	composer.damage(); // TODO optimize
	return t;
};
function add(text, replace) {
	var t = this;
	if (replace) t.string = '', t.cursor = -1;
	if (isstr(text) && text.length) {
		t.string = t.string.splice(t.cursor+1, 0, text);
		t.cursor += text.length;
	}
	t.update();
	return t;
};
Text = text3d = function (name, loc, rot, sca) {
	var _ = {
		name     : name      || ++text3d_uid,
		bounds	 : {x: 0, y: 0, _w: 0, _h: 0},
		color	 : color4(c_dwhite),
		location : v3(0),
		rotation : v3(0),
		scale	 : v3(1),

		string			: ''				,
		lines			: []				,
		selection		: []				,
		cursor			: -1				,
		add				: add				,
		update			: update			,
	};
	const handler = {
		set: function (o, p, v) {
			o[p] = v;
			if (p == 'string') _.update();
			return 1;
		}
	};
	_ = scene.add('text3d.'+_.name, TEXT3D, _);
	var proxy = new Proxy(_, handler);
	if (loc) proxy.location = v3(loc.x, loc.y, loc.z); else proxy.location = v3(0);
	if (rot) proxy.rotation = v3(rot.x, rot.y, rot.z); else proxy.rotation = v3(0);
	if (sca) proxy.scale	= v3(sca.x, sca.y, sca.z); else proxy.scale	   = v3(1);
	return proxy;
};

var render_char = function (c) {
	var ch = load_char(c);
	if (ch && ch.vao) {
		var wire_mode = scene.wire_mode;
		if (scene.two_dee || scene.picking_mode) wire_mode = 0;
		if (wire_mode) sgl.polygon_mode( sgl.GL_FRONT_AND_BACK, sgl.GL_LINE );
		sgl.bind(sgl.GL_VERTEX_ARRAY, ch.vao);
		sgl.draw_elements(sgl.GL_TRIANGLES, ch.faces_n, sgl.GL_UNSIGNED_INT);
		sgl.bind(sgl.GL_VERTEX_ARRAY, 0);
//		ch.mb.draw();
		if (wire_mode) sgl.polygon_mode( sgl.GL_FRONT_AND_BACK, sgl.GL_FILL );
	}
};
var render_cursor = function (mat, w, selected) {
	if (!scene.edit_mode || !selected) return;
	mat4.translate(mat, mat, [w,0,-.001]);
	sgl.uniform_matrix_4fv (uniforms.model, mat);
	render_char('|');
	mat4.translate(mat, mat, [-1*w,0,.02]);
};
var harakaat = [1614, 1616, 1615, 1617, 1648, 1611, 1613, 1767, 2292, 1618,
1619, 1755, 1754, 1751, 1750, 1620, 1753, 1612, 1759];
is_harakah = function (h) {
	return harakaat.indexOf(h) > -1;
};

hooks.set('scene.pressed', function (o, k, m) { if (o.type == TEXT3D) {
	if (scene.edit_mode) {
		var yes;
		if (!m.state || m.state == 2 || m.text.length) {
			if (k == 'delete') {
				if (o.cursor < o.string.length-1) {
					o.string = o.string.splice(o.cursor+1, 1, '');
					o.update(); yes = 1;
				}
			} else if (k == 'backspace') {
				if (o.cursor > -1) {
					o.string = o.string.splice(o.cursor, 1, '');
					o.cursor--;
					o.update(); yes = 1;
				}
			} else if (k == 'home') {
				while (o.cursor > -1 && o.string[o.cursor] != '\n') {
					o.cursor--;
				}
				if (o.cursor < -1) o.cursor = -1;
			} else if (k == 'end') {
				while (o.cursor < o.string.length -1 && o.string[o.cursor+1] != '\n') {
					o.cursor++;
				}
				if (o.cursor > o.string.length -1) o.cursor = o.string.length -1;
			} else if (k == 'left') {
				o.cursor--; if (o.cursor < -1) o.cursor = -1;
			} else if (k == 'right') {
				o.cursor++; if (o.cursor > o.string.length -1) o.cursor = o.string.length -1;
			} else if (k == 'up') {
				for (var i = o.cursor-1; i > -2; --i) {
					if (i == -1 || o.string[i] == '\n') { o.cursor = i; break; }
				}
				if (o.cursor < -1) o.cursor = -1;
			} else if (k == 'down') {
				for (var i = o.cursor+1; i < o.string.length; ++i) {
					if (o.string[i] == '\n') { o.cursor = i; break; }
				}
				if (o.cursor > o.string.length -1) o.cursor = o.string.length -1;
			} else if (k == 'tab') {
				// TODO shift+tab, delete one tab in the beginning of cur line
				o.add('\t'); yes = 1;
			} else if (k == 'enter') {
				o.add('\n'); yes = 1;
			} else if (m.text) {
				o.add(m.text); yes = 1;
			}
		}
		return yes;
	}
} });
hooks.set('scene.info', function (o) { if (o.type == TEXT3D) {
	var str = '';
	if (str) str += '\n';
	var l = o.get('location');
	var r = o.get('rotation');
	var s = o.get('scale');
	str += 'perf '+o.perf+'\n'
		+'loc x'+(l.x)+' y'+(l.y)+' z'+(l.z)
		+';  rot x'+(r.x)+' y'+(r.y)+' z'+(r.z)
		+';  sca x'+(s.x)+' y'+(s.y)+' z'+(s.z)
		+';  length '+(o.string.length)
		+';  cursor '+(o.cursor);
	return str;
} });
hooks.set('scene.frame', function (o, collection) { if (o.type == TEXT3D) {
	if ( collection && collection.guid == Collections.get('2d').guid ) {
		var min_win = two_dee_cam.bbox.min;
		var max_win = two_dee_cam.bbox.max;
		if (isnum(min_win[1])) {
			var l = o.get('location');
			var b = o.bounds;
			if (l.x > max_win[0] || l.x+b.w < min_win[0] || l.y < max_win[1] || l.y > min_win[1]+1) {
				if (o.visible != 0) o.visible = 0;
			} else {
				if (o.visible != 1) o.visible = 1;
			}
		}
	}
} });
var transform_mat4 = mat4.create();
hooks.set('scene.draw', function (o, selected) { if (o.type == TEXT3D) {
	var mat = scene.to_mat4(o);
	sgl.uniform_matrix_4fv(uniforms.model, mat);
	sgl.uniform_matrix_4fv(uniforms.transform, transform_mat4);

	if (scene.picking_mode && o.raycastable == 0) return;

	if (scene.picking_mode) {
		sgl.uniform_1ui(uniforms.object_id, to32bits(o.guid, 0));
	} else {
		sgl.uniform_1i(uniforms.selected, selected ? 1 : 0);
	}

	sgl.uniform_1f(uniforms.has_color, 0);
	var c4 = o.color;
	sgl.uniform_4f(uniforms.single_color, c4.r, c4.g, c4.b, c4.a);

	mat4.translate(mat, mat, [0,-o.line_height,0]);
	
	if (o.cursor == -1) render_cursor(mat, -.1, selected);
	var last_char = last_char_ori = { w: 0 }, line_width = 0, count = 0;
	o.lines.forEach(function (c, line) {
		var is_rtl = o.levels[ line ][ 0 ] % 2 !== 0; // odd
		line_width = o.widths[line];
		c.forEach(function (e, i) {
			var real_index = o.indices[ line ][ i ];
			var ori = c[real_index];
			
			is_rtl = o.levels[ line ][ real_index ] % 2 !== 0; // odd

			mat4.translate(mat, mat, [last_char.w, 0, 0]);
			sgl.uniform_matrix_4fv (uniforms.model, mat);
			render_char(e.c);
			
			if (real_index+count == o.cursor) {
				if (is_rtl) render_cursor(mat, -ori.w/2, selected);
				else render_cursor(mat, ori.w, selected);
			}

			last_char = e;
			last_char_ori = ori;
		});
		count += c.length+1;

		mat4.translate(mat, mat, [-line_width, -o.line_height, 0]);
		var real_index = o.indices[ line ][ 0 ] || 0;
		if (real_index+count == o.cursor+1) render_cursor(mat, last_char.w-.1, selected);
	});
	if (o.cursor >= o.string.length) render_cursor(mat, last_char.w, selected);

	sgl.uniform_1i(uniforms.selected, 0);

	var frames = 0;
	if (!scene.picking_mode) { // dont animate in picking mode
		frames += animate_key_frames(o);
	}
	return frames ? 1 : 0;
} });
hooks.set('scene.selector', function (o, selector) { if (o.type == TEXT3D) {
} });
hooks.set('scene.raycast', function (o, ray) { if (o.type == TEXT3D) {
	if (o.bounds) {
		var camera = get_active_camera();
		var mat = scene.to_mat4(o);
		var d = v3( camera.project(ray.x, ray.y, mat) );
		var bb = o.bounds;
		var yes = v3.inside_rect(d.x, d.y, bb.x, bb.y, bb.w, bb.h);
		return yes;
	}
} });
hooks.set('scene.move', function (o, transform) { if (o.type == TEXT3D) {
	o.location.transform = v3.add(o.posed('location'), transform);
} });
hooks.set('scene.rotate', function (o, transform) { if (o.type == TEXT3D) {
	o.rotation.transform = v3.add(o.posed('rotation'), transform);
} });
hooks.set('scene.scale', function (o, transform) { if (o.type == TEXT3D) {
	o.scale.transform = v3.add(o.posed('scale'), transform);
} });
hooks.set('scene.apply', function (o) { if (o.type == TEXT3D) {
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
hooks.set('scene.cancel', function (o) { if (o.type == TEXT3D) {
	o.location.transform = 0;
	o.rotation.transform = 0;
	o.scale.transform = 0;
} });
hooks.set('scene.reset', function (o) { if (o.type == TEXT3D) {
	o.location.pose = 0;
	o.rotation.pose = 0;
	o.scale.pose = 0;
} });
hooks.set('scene.remove', function (o) { if (o.type == TEXT3D) {
} });



})();



