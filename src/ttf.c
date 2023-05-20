#include "ttf.h"

static char view_mode = 0;
static ttf_t *font = NULL; // TODO make a sys_font to load missing chars
#define SGL_CHAR_NONE	0x25AD

void ttf_free_cache() {
	ttf_free(font);
	font = NULL;
}

void ttf_destroy() {
	ttf_free_cache();
}
void ttf_start() {}

bindjs (ttf_select_font) { // no args returns extents
	duk_idx_t n = duk_get_top(J);
	if (n) {
		const char *mask = duk_to_string(J, 0);
		ttf_t **list = ttf_list_system_fonts(mask);
		if (list == NULL) return 0; // no memory in system
		if (list[0] == NULL) return 0; // no fonts were found
		ttf_free_cache();
		ttf_load_from_file(list[0]->filename, &font, 0);
		ttf_free_list(list);
		if (font == NULL) return 0;
		duk_push_string(J, font->names.full_name);
		return 1;
	} else if (font) {
		duk_push_object(J);
		dukjs_prop_num("ascender", font->hhea.ascender);
		dukjs_prop_num("descender", font->hhea.descender);
		dukjs_prop_num("line_gap", font->hhea.lineGap);
		dukjs_prop_num("advance_max", font->hhea.advanceWidthMax);
		dukjs_prop_num("min_lsb", font->hhea.minLSideBearing);
		dukjs_prop_num("min_rsb", font->hhea.minRSideBearing);
		dukjs_prop_num("x_max", font->hhea.xMaxExtent);
		dukjs_prop_num("caret_slope", font->hhea.caretSlope);
		return 1;
	}
	return 0;
}
bindjs (ttf_get_char_2d) {
	duk_idx_t n = duk_get_top(J);
	if (n && font) {
		wchar_t symbol = duk_get_number(J, 0);
		int index = ttf_find_glyph(font, symbol);
		if (index < 0) return 0;

		ttf_glyph_t glyph = font->glyphs[index];
		ttf_mesh_t *out;

		int err = 0;
		err = ttf_glyph2mesh(&glyph, &out, TTF_QUALITY_LOW, TTF_FEATURE_IGN_ERR);

		if (err != TTF_DONE && err != TTF_ERR_NO_OUTLINE) return 0;
		if (out) {
			int length  = out->nvert * 2 * sizeof(GLfloat);
			int length2 = out->nfaces * 3 * sizeof(GLint);
			void *p  = duk_push_fixed_buffer(J, length ); // -3
			void *p2 = duk_push_fixed_buffer(J, length2); // -2
			
			duk_push_object(J); // -1

			float *first = &out->vert->x;
			if (p) {
				memcpy(p, first, length);
				duk_push_buffer_object(J, -3, 0, length, DUK_BUFOBJ_FLOAT32ARRAY);
			}
			duk_put_prop_string(J, -2, "verts");
			
			int *first2 = &out->faces->v1;
			if (p2) {
				memcpy(p2, first2, length2);
				duk_push_buffer_object(J, -2, 0, length2, DUK_BUFOBJ_UINT16ARRAY);
			}
			duk_put_prop_string(J, -2, "faces");

			dukjs_prop_num("verts_n", length );
			dukjs_prop_num("faces_n", length2);

			dukjs_prop_num("x", glyph.xbounds[0]);
			dukjs_prop_num("y", glyph.ybounds[0]);
			dukjs_prop_num("w", glyph.xbounds[1]);
			dukjs_prop_num("h", glyph.ybounds[1]);
			dukjs_prop_num("a", glyph.advance);

			ttf_free_mesh(out);

			return 1;
		}
	}
	return 0;
}
bindjs (ttf_get_char) {
	duk_idx_t n = duk_get_top(J);
	if (n && font) {
		wchar_t symbol = duk_get_number(J, 0);
		double depth = n > 1 ? duk_get_number(J, 1) : .01;
		int index = ttf_find_glyph(font, symbol);
		if (index < 0) return 0;

		ttf_glyph_t glyph = font->glyphs[index];
		ttf_mesh3d_t *out;

		int err = 0;
		err = ttf_glyph2mesh3d(&glyph, &out, TTF_QUALITY_LOW, TTF_FEATURE_IGN_ERR, depth);

		if (err != TTF_DONE && err != TTF_ERR_NO_OUTLINE) return 0;
		if (out) {
			int length  = out->nvert * 3 * sizeof(GLfloat);
			int length2 = out->nfaces * 3 * sizeof(GLint);
			int length3 = out->nfaces * 3 * 3 * sizeof(GLfloat);
			void *p  = duk_push_fixed_buffer(J, length ); // -4
			void *p2 = duk_push_fixed_buffer(J, length2); // -3
			void *p3 = duk_push_fixed_buffer(J, length3); // -2
			
			duk_push_object(J); // -1

			float *first = &out->vert->x;
			if (p) {
				memcpy(p, first, length);
				duk_push_buffer_object(J, -4, 0, length, DUK_BUFOBJ_FLOAT32ARRAY);
			}
			duk_put_prop_string(J, -2, "verts");
			
			int *first2 = &out->faces->v1;
			if (p2) {
				memcpy(p2, first2, length2);
				duk_push_buffer_object(J, -3, 0, length2, DUK_BUFOBJ_UINT16ARRAY);
			}
			duk_put_prop_string(J, -2, "faces");

			float *first3 = &out->normals->x;
			if (p3) {
				memcpy(p3, first3, length3);
				duk_push_buffer_object(J, -2, 0, length3, DUK_BUFOBJ_FLOAT32ARRAY);
			}
			duk_put_prop_string(J, -2, "norms");

			dukjs_prop_num("verts_n", length );
			dukjs_prop_num("faces_n", length2);
			dukjs_prop_num("norms_n", length3);

			dukjs_prop_num("x", glyph.xbounds[0]);
			dukjs_prop_num("y", glyph.ybounds[0]);
			dukjs_prop_num("w", glyph.xbounds[1]);
			dukjs_prop_num("h", glyph.ybounds[1]);
			dukjs_prop_num("a", glyph.advance);

			ttf_free_mesh3d(out);

			return 1;
		}
	}
	return 0;
}

void ttf_bind_js(duk_context *J) {
	duk_push_object(J); {
		push_c_method_x(J, "font", ttf_select_font);
		push_c_method_x(J, "char", ttf_get_char);
		push_c_method_x(J, "char2d", ttf_get_char_2d);
	}
	duk_put_global_string(J, "ttf");
}