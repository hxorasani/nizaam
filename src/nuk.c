#include "nuk.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_BUTTON_TRIGGER_ON_RELEASE

#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024
#include "nuklear.min.h"
#include "nuklear_sdl_gl3.h"

struct nk_canvas {
    struct nk_command_buffer *painter;
    struct nk_vec2 item_spacing;
    struct nk_vec2 panel_padding;
    struct nk_style_item window_background;
};

struct nk_canvas prime_canvas;

nk_bool canvas_begin(struct nk_context *, const char *, struct nk_canvas *, nk_flags,
    int, int, int, int, struct nk_color);
void canvas_end(struct nk_context *, struct nk_canvas *);

struct nk_context *nuk_ctx;
const struct nk_baked_font *nuk_default_font_info;
const struct nk_user_font *nuk_default_font_handle;

struct media {
    struct nk_font *font_s;
    struct nk_font *font_m;
    struct nk_font *font_l;
    struct nk_font *font_x;
};
struct media fonts;

nk_bool canvas_begin(struct nk_context *ctx, const char *name, struct nk_canvas *canvas, nk_flags flags,
    int x, int y, int width, int height, struct nk_color background_color) {
    /* save style properties which will be overwritten */
    canvas->panel_padding = ctx->style.window.padding;
    canvas->item_spacing = ctx->style.window.spacing;
    canvas->window_background = ctx->style.window.fixed_background;

    /* use the complete window space and set background */
    ctx->style.window.spacing = nk_vec2(0,0);
    ctx->style.window.padding = nk_vec2(0,0);
    ctx->style.window.fixed_background = nk_style_item_color(background_color);

    /* create/update window and set position + size */
    if (!nk_begin(ctx, name, nk_rect(x, y, width, height), NK_WINDOW_NO_SCROLLBAR|flags))
        return nk_false;

    /* allocate the complete window space for drawing */
    {
        struct nk_rect total_space;
        total_space = nk_window_get_content_region(ctx);
        nk_layout_row_dynamic(ctx, total_space.h, 1);
        nk_widget(&total_space, ctx);
        canvas->painter = nk_window_get_canvas(ctx);
    }

    return nk_true;
}
void canvas_end(struct nk_context *ctx, struct nk_canvas *canvas) {
    nk_end(ctx);
    ctx->style.window.spacing = canvas->panel_padding;
    ctx->style.window.padding = canvas->item_spacing;
    ctx->style.window.fixed_background = canvas->window_background;
}
void nuk_destroy() {
	nk_sdl_shutdown();
	nuk_ctx = NULL;
};
void nuk_input_start() { nk_input_begin(nuk_ctx); };
void nuk_input_event(SDL_Event *ev) { nk_sdl_handle_event(ev); };
void nuk_input_end() { nk_input_end(nuk_ctx); };
void nuk_render() {
	nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
}
void nuk_start(SDL_Window *sdl_window) {
    nuk_ctx = nk_sdl_init(sdl_window);

	struct nk_font_atlas *atlas;
	nk_sdl_font_stash_begin(&atlas);

	struct nk_font_config cfg = nk_font_config(0);
	cfg.oversample_h = 3; cfg.oversample_v = 2;

	u_char *font_path = "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf";
	fonts.font_s = nk_font_atlas_add_from_file(atlas, font_path, 18, &cfg);
//	fonts.font_m = nk_font_atlas_add_from_file(atlas, font_path, 24, &cfg);
//	fonts.font_l = nk_font_atlas_add_from_file(atlas, font_path, 32, &cfg);
//	fonts.font_x = nk_font_atlas_add_from_file(atlas, font_path, 64, &cfg);

	nk_sdl_font_stash_end();

	nuk_default_font_info = &fonts.font_s->info;
	nuk_default_font_handle = &fonts.font_s->handle;
	nk_init_default(nuk_ctx, nuk_default_font_handle);
}

bindjs (nuk_font_extents) {
	duk_push_object(J);
	dukjs_prop_num("a",  nuk_default_font_info->ascent );
	dukjs_prop_num("h",  nuk_default_font_info->height );
	dukjs_prop_num("d", -nuk_default_font_info->descent);
	return 1;
}
bindjs (nuk_text_extents) {
	int n = duk_get_top(J);
	if (n) {
		const char *str = duk_to_string(J, 0);
		float text_width = nuk_default_font_handle->width(
			nuk_default_font_handle->userdata,
			nuk_default_font_handle->height,
			str, strlen(str)
		);
		duk_push_object(J);
		dukjs_prop_num("w", text_width );
		dukjs_prop_num("h", nuk_default_font_handle->height );
		return 1;
	}
	return 0;
}
bindjs (nuk_begin_canvas) {
	duk_idx_t n = duk_get_top(J);
	if (n > 5 && !canvas_open) {
		const char *name = duk_to_string(J, 0);
		float x = duk_get_number(J, 1);
		float y = duk_get_number(J, 2);
		float w = duk_get_number(J, 3);
		float h = duk_get_number(J, 4);
		int opts = duk_get_number(J, 5);
		canvas_open = 1;
		if (canvas_begin(nuk_ctx, name, &prime_canvas, opts, x, y, w, h, nk_rgba(0,0,0,0))) {
			duk_push_number(J, 1);
			return 1;
		}
	}
	return 0;
}
bindjs (nuk_end_canvas) {
	if (canvas_open) {
		canvas_open = 0;
		canvas_end(nuk_ctx, &prime_canvas);
	}
	return 0;
}
bindjs (nuk_fill_circle) {
	duk_idx_t n = duk_get_top(J);
	if (n > 7 && canvas_open) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		float w = duk_get_number(J, 2);
		float h = duk_get_number(J, 3);

		float r = duk_get_number(J, 4) * 255;
		float g = duk_get_number(J, 5) * 255;
		float b = duk_get_number(J, 6) * 255;
		float a = duk_get_number(J, 7) * 255;

		float cx = prime_canvas.painter->clip.x, cy = prime_canvas.painter->clip.y;
		nk_fill_circle(prime_canvas.painter, nk_rect(cx+x, cy+y, w, h), nk_rgba(r,g,b,a));
	}
	return 0;
}
bindjs (nuk_stroke_circle) {
	duk_idx_t n = duk_get_top(J);
	if (n > 8 && canvas_open) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		float w = duk_get_number(J, 2);
		float h = duk_get_number(J, 3);
		float th = duk_get_number(J, 4);

		float r = duk_get_number(J, 5) * 255;
		float g = duk_get_number(J, 6) * 255;
		float b = duk_get_number(J, 7) * 255;
		float a = duk_get_number(J, 8) * 255;

		float cx = prime_canvas.painter->clip.x, cy = prime_canvas.painter->clip.y;
		nk_stroke_circle(prime_canvas.painter, nk_rect(cx+x, cy+y, w, h), th, nk_rgba(r,g,b,a));
	}
	return 0;
}
bindjs (nuk_stroke_line) {
	duk_idx_t n = duk_get_top(J);
	if (n > 8 && canvas_open) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		float w = duk_get_number(J, 2);
		float h = duk_get_number(J, 3);
		float th = duk_get_number(J, 4);

		float r = duk_get_number(J, 5) * 255;
		float g = duk_get_number(J, 6) * 255;
		float b = duk_get_number(J, 7) * 255;
		float a = duk_get_number(J, 8) * 255;

		float cx = prime_canvas.painter->clip.x, cy = prime_canvas.painter->clip.y;
		nk_stroke_line(prime_canvas.painter, cx+x, cy+y, w, h, th, nk_rgba(r,g,b,a));
	}
	return 0;
}
bindjs (nuk_stroke_rect) {
	duk_idx_t n = duk_get_top(J);
	if (n > 9 && canvas_open) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		float w = duk_get_number(J, 2);
		float h = duk_get_number(J, 3);

		float ra = duk_get_number(J, 4);
		float th = duk_get_number(J, 5);

		float r = duk_get_number(J, 6) * 255;
		float g = duk_get_number(J, 7) * 255;
		float b = duk_get_number(J, 8) * 255;
		float a = duk_get_number(J, 9) * 255;

		float cx = prime_canvas.painter->clip.x, cy = prime_canvas.painter->clip.y;
		nk_stroke_rect(prime_canvas.painter, nk_rect(cx+x, cy+y, w, h), ra, th, nk_rgba(r,g,b,a));
	}
	return 0;
}
bindjs (nuk_fill_rect) {
	duk_idx_t n = duk_get_top(J);
	if (n > 8 && canvas_open) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		float w = duk_get_number(J, 2);
		float h = duk_get_number(J, 3);

		float ra = duk_get_number(J, 4);

		float r = duk_get_number(J, 5) * 255;
		float g = duk_get_number(J, 6) * 255;
		float b = duk_get_number(J, 7) * 255;
		float a = duk_get_number(J, 8) * 255;

		float cx = prime_canvas.painter->clip.x, cy = prime_canvas.painter->clip.y;
		nk_fill_rect(prime_canvas.painter, nk_rect(cx+x, cy+y, w, h), ra, nk_rgba(r,g,b,a));
	}
	return 0;
}
bindjs (nuk_draw_text) {
	duk_idx_t n = duk_get_top(J);
	if (n > 6 && nuk_default_font_handle && canvas_open) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);

		const char *str = duk_to_string(J, 2);

		float r = duk_get_number(J, 3) * 255;
		float g = duk_get_number(J, 4) * 255;
		float b = duk_get_number(J, 5) * 255;
		float a = duk_get_number(J, 6) * 255;

		float cx = prime_canvas.painter->clip.x, cy = prime_canvas.painter->clip.y;
		float cw = prime_canvas.painter->clip.w, ch = prime_canvas.painter->clip.h;

		nk_draw_text(
			prime_canvas.painter, nk_rect(cx+x, cy+y, cw, ch),
			str, strlen(str), nuk_default_font_handle,
			nk_rgb(0,0,0), nk_rgba(r,g,b,a)
		);
	}
	return 0;
}
bindjs (nuk_begin) {
	duk_idx_t n = duk_get_top(J);
	if (n > 6 && !window_open) {
		float x = duk_get_number(J, 2);
		float y = duk_get_number(J, 3);
		float w = duk_get_number(J, 4);
		float h = duk_get_number(J, 5);
		int opts = duk_get_number(J, 6);
		window_open = 1;
		if (nk_begin_titled(nuk_ctx, duk_to_string(J, 0), duk_to_string(J, 1), nk_rect(x, y, w, h), opts)) {
			duk_push_number(J, 1);
			return 1;
		}
	} else
	if (n > 5 && !window_open) {
		float x = duk_get_number(J, 1);
		float y = duk_get_number(J, 2);
		float w = duk_get_number(J, 3);
		float h = duk_get_number(J, 4);
		int opts = duk_get_number(J, 5);
		window_open = 1;
		if (nk_begin(nuk_ctx, duk_to_string(J, 0), nk_rect(x, y, w, h), opts)) {
			duk_push_number(J, 1);
			return 1;
		}
	}
	return 0;
}
bindjs (nuk_window_get_position) {
	struct nk_vec2 p = nk_window_get_position(nuk_ctx);
	duk_push_object(J); {
		dukjs_prop_num("x", p.x);
		dukjs_prop_num("y", p.y);
	}
	return 1;
}
bindjs (nuk_window_set_position) {
	duk_idx_t n = duk_get_top(J);
	if (n > 2) {
		float x = duk_get_number(J, 1);
		float y = duk_get_number(J, 2);
		nk_window_set_position(nuk_ctx, duk_to_string(J, 0), nk_vec2(x, y));
	}
	return 0;
}
bindjs (nuk_button_label) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		if ( nk_button_label(nuk_ctx, duk_to_string(J, 0)) ) {
			duk_push_number(J, 1);
			return 1;
		}
	}
	return 0;
}
bindjs (nuk_property) { // prefix # to gen uid
	duk_idx_t n = duk_get_top(J);
	if (n > 5) {
		double min = duk_get_number(J, 1);
		double def = duk_get_number(J, 2);
		double max = duk_get_number(J, 3);
		double step = duk_get_number(J, 4);
		double inc_pxl = duk_get_number(J, 5);
		nk_property_double(nuk_ctx, duk_to_string(J, 0), min, &def, max, step, inc_pxl);
		duk_push_number(J, def);
		return 1;
	}
	return 0;
}
bindjs (nuk_color_picker) {
	duk_idx_t n = duk_get_top(J);
	if (n > 3) {
		struct nk_colorf color;
		color.r = duk_get_number(J, 0);
		color.g = duk_get_number(J, 1);
		color.b = duk_get_number(J, 2);
		color.a = duk_get_number(J, 3);
		color = nk_color_picker(nuk_ctx, color, NK_RGBA);
		duk_push_object(J);
			dukjs_prop_num("r", color.r);
			dukjs_prop_num("g", color.g);
			dukjs_prop_num("b", color.b);
			dukjs_prop_num("a", color.a);
		return 1;
	}
	return 0;
}
bindjs (nuk_label) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) nk_label(nuk_ctx, duk_to_string(J, 0), duk_get_number(J, 1));
	return 0;
}
bindjs (nuk_selectable_text) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1 && duk_get_type(J, 0) == DUK_TYPE_STRING) {
		const char *str = duk_get_string(J, 0);
		int val = 1;
		val = nk_selectable_text(nuk_ctx, str, strlen(str), duk_get_number(J, 1), &val);
		duk_push_number(J, val);
		return 1;
	}
	return 0;
}
bindjs (nuk_layout_row_dynamic) { // height, widget count
	duk_idx_t n = duk_get_top(J);
	if (n > 1)
		nk_layout_row_dynamic(nuk_ctx, duk_get_number(J, 0), duk_get_number(J, 1));
	return 0;
}
bindjs (nuk_set_style) {
	duk_idx_t n = duk_get_top(J);
//	if (n) set_style(nuk_ctx, duk_get_number(J, 0));
	return 0;
}
bindjs (nuk_end) {
	if (window_open) {
		window_open = 0;
		nk_end(nuk_ctx);
	}
	return 0;
}

void nuk_bind_js(duk_context *J) {
	duk_push_object(J); {
		dukjs_prop_num("LEFT",		NK_TEXT_ALIGN_LEFT		);
		dukjs_prop_num("CENTERED",	NK_TEXT_ALIGN_CENTERED	);
		dukjs_prop_num("RIGHT",		NK_TEXT_ALIGN_RIGHT		);
		dukjs_prop_num("TOP",		NK_TEXT_ALIGN_TOP		);
		dukjs_prop_num("MIDDLE",	NK_TEXT_ALIGN_MIDDLE	);
		dukjs_prop_num("BOTTOM",	NK_TEXT_ALIGN_BOTTOM	);

		dukjs_prop_num("BORDER",			NK_WINDOW_BORDER			);
		dukjs_prop_num("MOVABLE",			NK_WINDOW_MOVABLE			);
		dukjs_prop_num("SCALABLE",			NK_WINDOW_SCALABLE			);
		dukjs_prop_num("CLOSABLE",			NK_WINDOW_CLOSABLE			);
		dukjs_prop_num("MINIMIZABLE",		NK_WINDOW_MINIMIZABLE		);
		dukjs_prop_num("NO_SCROLLBAR",		NK_WINDOW_NO_SCROLLBAR		);
		dukjs_prop_num("TITLE",				NK_WINDOW_TITLE				);
		dukjs_prop_num("SCROLL_AUTO_HIDE",	NK_WINDOW_SCROLL_AUTO_HIDE	);
		dukjs_prop_num("BACKGROUND",		NK_WINDOW_BACKGROUND		);
		dukjs_prop_num("SCALE_LEFT",		NK_WINDOW_SCALE_LEFT		);
		dukjs_prop_num("NO_INPUT",			NK_WINDOW_NO_INPUT			);

		push_c_method("layout_row_dynamic",		nuk_layout_row_dynamic	);
		push_c_method("window_get_position",	nuk_window_get_position	);
		push_c_method("window_set_position",	nuk_window_set_position	);
		push_c_method("button_label",			nuk_button_label		);
		push_c_method("color_picker",			nuk_color_picker		);
		push_c_method("label",					nuk_label				);
		push_c_method("selectable_text",		nuk_selectable_text		);
		push_c_method("property",				nuk_property			);
		push_c_method("begin",					nuk_begin				);
		push_c_method("end",					nuk_end					);
		
		push_c_method("begin_canvas",			nuk_begin_canvas		);
		push_c_method("end_canvas",				nuk_end_canvas			);
		push_c_method("fill_circle",			nuk_fill_circle			);
		push_c_method("stroke_circle",			nuk_stroke_circle		);
		push_c_method("stroke_line",			nuk_stroke_line			);
		push_c_method("fill_rect",				nuk_fill_rect			);
		push_c_method("stroke_rect",			nuk_stroke_rect			);
		push_c_method("font_extents",			nuk_font_extents		);
		push_c_method("text_extents",			nuk_text_extents		);
		push_c_method("draw_text",				nuk_draw_text			);
		
		push_c_method("set_style",				nuk_set_style			);
	}
	duk_put_global_string(J, "nuk");
}