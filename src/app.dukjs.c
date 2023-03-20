/*
 * duk-c funcs should return 1 if they wanna do duk_push_* at the end
 * 
 * 
 * */
#include "app.dukjs.h"
#include "sdl.h"
#include "sgl.h"
#define XATAA 0

duk_context *J;
char **environment; u_char *appname; char queuereload = 0;

void dukjs_prop_num(char *prop, double num) {
	duk_push_number(J, num); duk_put_prop_string(J, -2, prop);
}

//{ colors
duk_ret_t composer_colors_to_components(duk_context *J) { // 0xAARRGGBB -> A, R, G, B
	int n = duk_get_top(J);
	if (n) {
		u_int argb = duk_to_number(J, 0);

		u_char a = 255, r = 0, g = 0, b = 0;
		argb_to_components( argb, &a, &r, &g, &b );

		duk_push_object(J);
		dukjs_prop_num("a", (float) a / 255.0);
		dukjs_prop_num("r", (float) r / 255.0);
		dukjs_prop_num("g", (float) g / 255.0);
		dukjs_prop_num("b", (float) b / 255.0);
		return 1;
	} else return 0;
}
duk_ret_t composer_colors_to_integer(duk_context *J) { // A, R, G, B -> 0xAARRGGBB
	int n = duk_get_top(J);
	if (n > 3) {
		u_char a = duk_to_number(J, 0) * 255;
		u_char r = duk_to_number(J, 1) * 255;
		u_char g = duk_to_number(J, 2) * 255;
		u_char b = duk_to_number(J, 3) * 255;

		u_int argb = 0;
		components_to_argb( a, r, g, b, &argb );

		duk_push_number(J, argb);
		return 1;
	} else return 0;
}
//}

//{ matrix
duk_ret_t composer_msfoof_to_radians(duk_context *J) {
	int n = duk_get_top(J);
	if (n) {
		duk_push_number(J, toradians( duk_get_number(J, 0) ));
		return 1;
	}
	return 0;
}
duk_ret_t composer_msfoof_to_degrees(duk_context *J) {
	int n = duk_get_top(J);
	if (n) {
		duk_push_number(J, todegrees( duk_get_number(J, 0) ));
		return 1;
	}
	return 0;
}
duk_ret_t composer_msfoof_angle2coord(duk_context *J) { // x, y, r, a
	int n = duk_get_top(J);
	if (n > 3) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		float r = duk_get_number(J, 2);
		float a = duk_get_number(J, 3);

		nuqtahf p = { 0 };
		angletocoord(x, y, r, a, &p);

		duk_push_object(J); {
			dukjs_prop_num("x", p.x);
			dukjs_prop_num("y", p.y);
		}
		return 1;
	} else return 0;
}
duk_ret_t composer_msfoof_coord2angle(duk_context *J) { // x, y, a, b
	int n = duk_get_top(J);
	if (n > 3) {
		float x0 = duk_get_number(J, 0);
		float y0 = duk_get_number(J, 1);
		float x1 = duk_get_number(J, 2);
		float y1 = duk_get_number(J, 3);

		duk_push_number(J, coordtoangle(x0, y0, x1, y1));
		return 1;
	} else return 0;
}
duk_ret_t composer_msfoof_distance(duk_context *J) { // x, y, a, b
	int n = duk_get_top(J);
	if (n > 3) {
		float x0 = duk_get_number(J, 0);
		float y0 = duk_get_number(J, 1);
		float x1 = duk_get_number(J, 2);
		float y1 = duk_get_number(J, 3);

		duk_push_number(J, distancef(x0, y0, x1, y1));
		return 1;
	} else return 0;
}
duk_ret_t composer_msfoof_contains(duk_context *J) { // a, a, x, y, w, h
	int n = duk_get_top(J);
	if (n > 5) {
		float x0 = duk_get_number(J, 0);
		float y0 = duk_get_number(J, 1);
		float x1 = duk_get_number(J, 2);
		float y1 = duk_get_number(J, 3);
		float x2 = duk_get_number(J, 4);
		float y2 = duk_get_number(J, 5);

		ihaatahf ih = {x1, y1, x2, y2};

		duk_push_number(J, mushtamilf(x0, y0, &ih));
		return 1;
	} else return 0;
}
duk_ret_t composer_msfoof_overlap(duk_context *J) { // 2 rects, return bool
	int n = duk_get_top(J);
	if (n > 7) {
		float x1 = duk_get_number(J, 0);
		float y1 = duk_get_number(J, 1);
		float w1 = duk_get_number(J, 2);
		float h1 = duk_get_number(J, 3);
		
		float x2 = duk_get_number(J, 4);
		float y2 = duk_get_number(J, 5);
		float w2 = duk_get_number(J, 6);
		float h2 = duk_get_number(J, 7);

		ihaatah ih = {x1, y1, w1, h1};
		ihaatah ih2 = {x2, y2, w2, h2};

		duk_push_number(J, do_rects_overlap(&ih, &ih2));
		return 1;
	} else return 0;
}
duk_ret_t composer_msfoof_clear(duk_context *J) {
	msfoof_clear( current_matrix, duk_get_top(J) ? duk_get_number(J, 0) : 0 );
	return 0;
}
//}

//{ canvas
duk_ret_t composer_canvas_flush(duk_context *J) {
	if (current_canvas->flush) {
		current_canvas->flush();
	}
	return 0;
}
duk_ret_t composer_canvas_blur(duk_context *J) {
	int n = duk_get_top(J);
	if (current_canvas->blur && n) {
		double r = duk_get_number(J, 0);
		current_canvas->blur(r);
	}
	return 0;
}
duk_ret_t composer_canvas_user_to_device(duk_context *J) {
	if (current_canvas->user_to_device) {
		double	x = duk_to_number(J, 0),
				y = duk_to_number(J, 1);
		current_canvas->user_to_device(&x, &y);
		duk_idx_t obj_idx = duk_push_object(J);
		{
			duk_push_number(J, x), duk_put_prop_string(J, obj_idx, "x");
			duk_push_number(J, y), duk_put_prop_string(J, obj_idx, "y");
		}
		return 1;
	}
	return 0;
}
duk_ret_t composer_canvas_device_to_user(duk_context *J) {
	if (current_canvas->device_to_user) {
		double	x = duk_to_number(J, 0),
				y = duk_to_number(J, 1);
		current_canvas->device_to_user(&x, &y);
		duk_idx_t obj_idx = duk_push_object(J);
		{
			duk_push_number(J, x), duk_put_prop_string(J, obj_idx, "x");
			duk_push_number(J, y), duk_put_prop_string(J, obj_idx, "y");
		}
		return 1;
	}
	return 0;
}
duk_ret_t composer_canvas_move_to(duk_context *J) {
	float	x0 = duk_to_number(J, 0),
			y0 = duk_to_number(J, 1);

	if (current_canvas->move_to) current_canvas->move_to(x0, y0);
	return 0;
}
void composer_canvas_bezier(duk_context *J) { // arc
	float	x  = duk_to_number(J, 0),
			y  = duk_to_number(J, 1),
			r  = duk_to_number(J, 2),
			sa = duk_to_number(J, 3),
			ea = duk_to_number(J, 4);

//		cairo_bezier (syq, x, y, r, toradians(sa), toradians(ea));
}
void composer_canvas_rect(duk_context *J) { // rect
	float	x = duk_to_number(J, 0),
			y = duk_to_number(J, 1),
			w = duk_to_number(J, 2),
			h = duk_to_number(J, 3);

	ihaatahf ih = {x, y, w, h};
	if (current_canvas->mustateel) current_canvas->mustateel(ih);
}
duk_ret_t composer_canvas_circle(duk_context *J) {
	float	x = duk_to_number(J, 0),
			y = duk_to_number(J, 1),
			r = duk_to_number(J, 2),
			s = duk_to_number(J, 3),
			e = duk_to_number(J, 4);

	if (current_canvas->circle) current_canvas->circle(x, y, r, s, e);
	
	return 0;
}
void composer_canvas_line(duk_context *J) { // line
	float	x0 = duk_to_number(J, 0),
			y0 = duk_to_number(J, 1);

	if (current_canvas->satar) current_canvas->satar(x0, y0);
}
void composer_canvas_stroke(duk_context *J) { // stroke / strike
	int	preserve = duk_get_top(J) ? duk_to_number(J, 0) : 0;
	if (current_canvas->stroke) current_canvas->stroke(preserve);
}
void composer_canvas_fill(duk_context *J) { // fill / spill
	int	preserve = duk_get_top(J) ? duk_to_number(J, 0) : 0;
	if (current_canvas->fill) current_canvas->fill(preserve);
}
void composer_canvas_lwn(duk_context *J) {
	int n = duk_get_top(J);
	float r = duk_to_number(J, 0);
	float g = duk_to_number(J, 1);
	float b = duk_to_number(J, 2);
	float a = n >= 4 ? duk_to_number(J, 3) : 1;

	if (!isnan(r) && !isnan(g) && !isnan(b)) {
		r = haddqanaatf(r);
		g = haddqanaatf(g);
		b = haddqanaatf(b);

		if (n >= 4 && !isnan(a)) {
			a = haddqanaatf(a);
			if (current_canvas->lwn_rgba) current_canvas->lwn_rgba (r, g, b, a);
		} else
			if (current_canvas->lwn_rgb) current_canvas->lwn_rgb (r, g, b);
	}
}
void composer_canvas_idhan(duk_context *J) { // paint
	if (current_canvas->paint) current_canvas->paint();
}
void composer_canvas_renew_path(duk_context *J) { // renew path
	if (current_canvas->renew_path) current_canvas->renew_path();
}
duk_ret_t composer_canvas_set_dash(duk_context *J) {
	int n = duk_get_top(J);
	double dashes = n > 0 ? duk_to_number(J, 0) : 0;
	int num_dashes = n > 1 ? duk_to_number(J, 1) : 0;
	double offset = n > 2 ? duk_to_number(J, 2) : 0;

	if (current_canvas->set_dash) current_canvas->set_dash(&dashes, num_dashes, offset);

	return 0;
}
duk_ret_t composer_canvas_line_width(duk_context *J) {
	float l = duk_to_number(J, 0);

	if (current_canvas->line_width && !isnan(l)) {
		if (l < 0) l = 0;
		else if (l > 100) l = 100;

		current_canvas->line_width(l);
	}

	duk_push_number(J, l);
	return 1;
}
duk_ret_t composer_canvas_xat(duk_context *J) { // font face
	const char *str = duk_safe_to_string(J, 0);

	if (current_canvas->xat_wijhah && str) {
		current_canvas->xat_wijhah(str);
	}
	return 0;
}
void composer_canvas_font_size(duk_context *J) { // font size
	float l = duk_to_number(J, 0);

	if (current_canvas->font_size && !isnan(l)) {
		current_canvas->font_size(l);
	}
}
duk_ret_t composer_canvas_font_bounds(duk_context *J) { // font extents
	if (current_canvas->font_bounds) {
		ihaatahf ih = { 0 };
		if (current_canvas->font_bounds) current_canvas->font_bounds (&ih);

		duk_idx_t obj_idx = duk_push_object(J);
		{
			duk_push_number(J, ih.x), duk_put_prop_string(J, obj_idx, "x");
			duk_push_number(J, ih.y), duk_put_prop_string(J, obj_idx, "y");
			duk_push_number(J, ih.s), duk_put_prop_string(J, obj_idx, "a");
			duk_push_number(J, ih.w), duk_put_prop_string(J, obj_idx, "d");
			duk_push_number(J, ih.h), duk_put_prop_string(J, obj_idx, "h");
		}
		return 1;
	} else return 0;
}
duk_ret_t composer_canvas_text(duk_context *J) { // text (returns extents!)
	if (duk_get_type(J, 0) == DUK_TYPE_STRING) {
		int n = duk_get_top(J);
		const char *str = duk_safe_to_string(J, 0);
		int dry = n > 1 ? duk_to_number(J, 1) : 0; // return extents also

		if (dry != 2) {
			if (current_canvas->show_text) current_canvas->show_text ((char *) str);
		}

		if (dry) {
			ihaatahf ih;
			if (current_canvas->text_bounds) current_canvas->text_bounds (&ih, (char *) str);
		
			duk_idx_t obj_idx = duk_push_object(J);
			{
				duk_push_number(J, ih.x), duk_put_prop_string(J, obj_idx, "a");
				duk_push_number(J, ih.s), duk_put_prop_string(J, obj_idx, "x");
				duk_push_number(J, ih.y), duk_put_prop_string(J, obj_idx, "y");
				duk_push_number(J, ih.w), duk_put_prop_string(J, obj_idx, "w");
				duk_push_number(J, ih.h), duk_put_prop_string(J, obj_idx, "h");
			}
			return 1;
		}
		return 0;
	} else {
		int n = duk_get_top(J);
		int dry = n >= 2 ? duk_to_number(J, 1) : 0; // only return extents
		float uni = duk_to_number(J, 0);
		uni = fushatarmeez.tofusha[(int)uni];
		
		ihaatahf ih;
		if (current_canvas->harf_hajm) current_canvas->harf_hajm (&ih, uni);
		
		if (dry != 1 && current_canvas->matn_harf) current_canvas->matn_harf(uni);
		
		duk_idx_t obj_idx = duk_push_object(J);
		{
			duk_push_number(J, ih.x), duk_put_prop_string(J, obj_idx, "a");
			duk_push_number(J, ih.s), duk_put_prop_string(J, obj_idx, "x");
			duk_push_number(J, ih.y), duk_put_prop_string(J, obj_idx, "y");
			duk_push_number(J, ih.w), duk_put_prop_string(J, obj_idx, "w");
			duk_push_number(J, ih.h), duk_put_prop_string(J, obj_idx, "h");
		}
		return 1;
	}
}
void composer_canvas_state(duk_context *J) { // canvas on/off
	int n = duk_get_top(J);
	composer_canvas( n ? duk_get_int(J, 0) : 0 );
}
void composer_canvas_restore(duk_context *J) { // restore
	if (current_canvas->restore) current_canvas->restore();
}
void composer_canvas_clip(duk_context *J) {
	if (current_canvas->clip) current_canvas->clip();
}
void composer_canvas_unclip(duk_context *J) {
	if (current_canvas->unclip) current_canvas->unclip();
}
void composer_canvas_save(duk_context *J) { // save
	if (current_canvas->save) current_canvas->save();
}
void composer_canvas_scale(duk_context *J) { // scale
	if (current_canvas->scale) {
		int n = duk_get_top(J);
		float x = n > 0 ? duk_to_number(J, 0) : 1;
		float y = n > 1 ? duk_to_number(J, 1) : 1;
		current_canvas->scale(x, y);
	}
}
void composer_canvas_rotate(duk_context *J) { // rotate
	int n = duk_get_top(J);
	float z = n > 0 ? duk_to_number(J, 0) : 0;
	if (current_canvas->rotate) current_canvas->rotate(z);
}
void composer_canvas_translate(duk_context *J) {
	if (current_canvas->translate) {
		int n = duk_get_top(J);
		float x = n > 0 ? duk_to_number(J, 0) : 1;
		float y = n > 1 ? duk_to_number(J, 1) : 1;
		current_canvas->translate(x, y);
	}
}
//}

//{ files
duk_ret_t composer_files_list	(duk_context *J) {
	int n = duk_get_top(J);
	if (n) {
		const char *path = duk_safe_to_string(J, 0);
		files_list_t list = { 0 };
		files_list(path, &list);
		if (list.total) {
			duk_push_array(J);

			for (int i = 0; i < list.total; ++i) {
				duk_push_object(J);

				duk_push_string(J, list.files[i].name);
				duk_put_prop_string(J, -2, "name");

				duk_push_string(J, list.files[i].type);
				duk_put_prop_string(J, -2, "type");

				duk_put_prop_index(J, -2, i);
			}

			files_list_free(&list);
			return 1;
		}
	}
	return 0;
}
//}

//{ globals
duk_ret_t composer_exit	(duk_context *J) {
	exit(0);
	return 0;
}
duk_ret_t composer_reload(duk_context *J) {
	queuereload = 1;
	return 0;
}
duk_ret_t composer_require(duk_context *J) {
	int n = duk_get_top(J);
	if (n && duk_get_type(J, 0) == DUK_TYPE_STRING) {
		const char *str = duk_safe_to_string(J, 0);
		load_js_file((char *) str);
	}
	return 0;
}
duk_ret_t composer_console_clear(duk_context *J) {
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
	return 0;
}
duk_ret_t composer_process_run(duk_context *J) {
	int n = duk_get_top(J);
	if (n && duk_get_type(J, 0) == DUK_TYPE_STRING) {
		// TODO move to amr/task
		pid_t p = fork(); // remember this somewhere
		const char *str = duk_safe_to_string(J, 0);
		char *args = helper_stringify("%s %s", "./bin/app.dukjs", str);
		int ret = system(args);
		if (ret == -1) perror("system in child");
		free(args);
	}
	return 0;
}
//}

/*{ sgl
duk_ret_t composer_sgl_tri_set(duk_context *J) { // model, t, x, y, z
	int n = duk_get_top(J);
	if (n > 4) {
		int m = duk_get_number(J, 0);
		int t = duk_get_number(J, 1);
		int x = duk_get_number(J, 2);
		int y = duk_get_number(J, 3);
		int z = duk_get_number(J, 4);

		S3L_Index p[3] = { x, y, z };

		duk_push_number(J, s3l_tri_set(m, t, p));
		return 1;
	} else return 0;
}
duk_ret_t composer_sgl_tri_material(duk_context *J) { // model, t, m
	int n = duk_get_top(J);
	if (n > 2) {
		int m = duk_get_number(J, 0);
		int t = duk_get_number(J, 1);
		u_int x = duk_get_number(J, 2);

		duk_push_number(J, s3l_tri_material(m, t, x));
		return 1;
	} else return 0;
}
duk_ret_t composer_sgl_tri_add(duk_context *J) { // model, x, y, z
	int n = duk_get_top(J);
	if (n > 3) {
		int m = duk_get_number(J, 0);
		int x = duk_get_number(J, 1);
		int y = duk_get_number(J, 2);
		int z = duk_get_number(J, 3);

		S3L_Index p[3] = { x, y, z };

		duk_push_number(J, s3l_tri_add(m, p));
		return 1;
	} else return 0;
}
duk_ret_t composer_sgl_tri_get(duk_context *J) { // model, t
	int n = duk_get_top(J);
	if (n > 1) {
		int m = duk_get_number(J, 0);
		int v = duk_get_number(J, 1);

		S3L_Index p[3] = { 0 };

		s3l_tri_get(m, v, p);
		u_int material = s3l_tri_get_material(m, v);
		
		duk_push_object(J);
		dukjs_prop_num("x", p[0]);
		dukjs_prop_num("y", p[1]);
		dukjs_prop_num("z", p[2]);
		dukjs_prop_num("material", material);
		return 1;
	} else return 0;
}
duk_ret_t composer_sgl_tri_remove(duk_context *J) { // model, t
	int n = duk_get_top(J);
	if (n > 1) {
		int m = duk_get_number(J, 0);
		int v = duk_get_number(J, 1);

		duk_push_number(J, s3l_tri_remove(m, v));
		return 1;
	} else return 0;
}

duk_ret_t composer_sgl_vert_set(duk_context *J) { // model, v, x, y, z
	int n = duk_get_top(J);
	if (n > 4) {
		int m = duk_get_number(J, 0);
		int v = duk_get_number(J, 1);
		S3L_Unit x = duk_get_number(J, 2);
		S3L_Unit y = duk_get_number(J, 3);
		S3L_Unit z = duk_get_number(J, 4);

		S3L_Unit p[3] = { x, y, z };

		duk_push_number(J, s3l_vert_set(m, v, p));
		return 1;
	} else return 0;
}
duk_ret_t composer_sgl_vert_add(duk_context *J) { // model, x, y, z
	int n = duk_get_top(J);
	if (n > 3) {
		int m = duk_get_number(J, 0);
		S3L_Unit x = duk_get_number(J, 1);
		S3L_Unit y = duk_get_number(J, 2);
		S3L_Unit z = duk_get_number(J, 3);

		S3L_Unit p[3] = { x, y, z };

		duk_push_number(J, s3l_vert_add(m, p));
		return 1;
	} else return 0;
}
duk_ret_t composer_sgl_vert_get(duk_context *J) { // model, v
	int n = duk_get_top(J);
	if (n > 1) {
		int m = duk_get_number(J, 0);
		int v = duk_get_number(J, 1);

		S3L_Unit p[3] = { 0 };

		s3l_vert_get(m, v, p);
		
		duk_push_object(J);
		dukjs_prop_num("x", p[0]);
		dukjs_prop_num("y", p[1]);
		dukjs_prop_num("z", p[2]);
		return 1;
	} else return 0;
}
duk_ret_t composer_sgl_vert_to_screen(duk_context *J) { // model, v
	int n = duk_get_top(J);
	if (n > 1) {
		int m = duk_get_number(J, 0);
		int v = duk_get_number(J, 1);

		S3L_Vec4 result = { 0 };

		s3l_vert_to_screen(m, v, &result);
		
		duk_push_object(J);
		dukjs_prop_num("x", result.x);
		dukjs_prop_num("y", result.y);
		dukjs_prop_num("z", result.z);
		dukjs_prop_num("w", result.w);
		return 1;
	} else return 0;
}
duk_ret_t composer_sgl_vert_remove(duk_context *J) { // model, v
	int n = duk_get_top(J);
	if (n > 1) {
		int m = duk_get_number(J, 0);
		int v = duk_get_number(J, 1);

		duk_push_number(J, s3l_vert_remove(m, v));
		return 1;
	} else return 0;
}

duk_ret_t composer_sgl_models	(duk_context *J) {
	int count = s3l_models(), found = 0;
	duk_push_array(J);
	for (int i = 0; i < MAX_MODELS && found < count; ++i) {
		sgl_model_t *m = s3l_get(i);
		if (m) {
			duk_push_number(J, i);
			duk_put_prop_index(J, -2, found);
			found++;
		}
	}
	return 1;
}
duk_ret_t composer_sgl_add	(duk_context *J) {
	duk_push_number(J, s3l_add());
	return 1;
}
duk_ret_t composer_sgl_get	(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	int j = n > 0 ? duk_to_number(J, 0) : -1;
	sgl_model_t *m = s3l_get(j);
	if (m) {
		duk_push_object(J);
		dukjs_prop_num("verts", m->model->vertexCount  );
		dukjs_prop_num("tris" , m->model->triangleCount);
		dukjs_prop_num("cull" , m->model->config.backfaceCulling);
		return 1;
	}
	return 0;
}
duk_ret_t composer_sgl_remove(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	int j = n > 0 ? duk_to_number(J, 0) : -1;
	duk_push_number(J, s3l_remove(j));
	return 1;
}

duk_ret_t composer_sgl_translation (duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	int j = n ? duk_get_number(J, 0) : -1;
	sgl_model_t *m = s3l_get(j);
	if (m) {
		if (n > 3) {
			m->model->transform.translation.x = duk_get_number(J, 1);
			m->model->transform.translation.y = duk_get_number(J, 2);
			m->model->transform.translation.z = duk_get_number(J, 3);
		} else {
			duk_push_object(J);
			dukjs_prop_num("x", m->model->transform.translation.x);
			dukjs_prop_num("y", m->model->transform.translation.y);
			dukjs_prop_num("z", m->model->transform.translation.z);
			return 1;
		}
	}
	return 0;
}
duk_ret_t composer_sgl_rotation (duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	int j = n ? duk_to_number(J, 0) : -1;
	sgl_model_t *m = s3l_get(j);
	if (m) {
		if (n > 3) {
			m->model->transform.rotation.x = duk_to_number(J, 1);
			m->model->transform.rotation.y = duk_to_number(J, 2);
			m->model->transform.rotation.z = duk_to_number(J, 3);
		} else {
			duk_push_object(J);
			dukjs_prop_num("x", m->model->transform.rotation.x);
			dukjs_prop_num("y", m->model->transform.rotation.y);
			dukjs_prop_num("z", m->model->transform.rotation.z);
			return 1;
		}
	}
	return 0;
}
duk_ret_t composer_sgl_scale (duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	int j = n ? duk_to_number(J, 0) : -1;
	sgl_model_t *m = s3l_get(j);
	if (m) {
		if (n > 3) {
			m->model->transform.scale.x = duk_to_number(J, 1);
			m->model->transform.scale.y = duk_to_number(J, 2);
			m->model->transform.scale.z = duk_to_number(J, 3);
		} else {
			duk_push_object(J);
			dukjs_prop_num("x", m->model->transform.scale.x);
			dukjs_prop_num("y", m->model->transform.scale.y);
			dukjs_prop_num("z", m->model->transform.scale.z);
			return 1;
		}
	}
	return 0;
}
duk_ret_t composer_sgl_backface(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	int j = n ? duk_to_number(J, 0) : -1;
	sgl_model_t *m = s3l_get(j);
	if (m) {
		if (n > 1) m->model->config.backfaceCulling = duk_to_number(J, 1);
		else m->model->config.backfaceCulling++;

		if (m->model->config.backfaceCulling > 2)
			m->model->config.backfaceCulling = 0;
	}
	return 0;
}

duk_ret_t composer_sgl_get_mode(duk_context *J) {
	duk_push_number(J, mode);
	return 1;
}
duk_ret_t composer_sgl_set_mode(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	mode = n ? duk_to_number(J, 0) : ++mode;
	if (mode > 5) mode = 0;
	duk_push_number(J, mode);
	return 1;
}
duk_ret_t composer_sgl_wire(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		int l = duk_to_number(J, 0);
		wire = l == -1 ? !wire : l;
	}
	duk_push_number(J, wire);
	return 1;
}
duk_ret_t composer_sgl_light(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		int l = duk_to_number(J, 0);
		light = l == -1 ? !light : l;
	}
	duk_push_number(J, light);
	return 1;
}
duk_ret_t composer_sgl_fog(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		int l = duk_to_number(J, 0);
		fog = l == -1 ? !fog : l;
	}
	duk_push_number(J, fog);
	return 1;
}
duk_ret_t composer_sgl_noise(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		int l = duk_to_number(J, 0);
		noise = l == -1 ? !noise : l;
	}
	duk_push_number(J, noise);
	return 1;
}
duk_ret_t composer_sgl_render(duk_context *J) {
	S3L_newFrame();
	S3L_drawScene(scene);
	return 0;
}

duk_ret_t composer_sgl_raycast(duk_context *J) {
	int n = duk_get_top(J);
	if (n > 1) {
		u_int x = duk_get_number(J, 0);
		u_int y = duk_get_number(J, 1);

		S3L_PixelInfo *p = sgl_raycast(x, y);
		if (p) {
			int uid = sgl_rid_to_uid(p->modelIndex);
			if (uid > -1) {
				duk_push_object(J);
				dukjs_prop_num("model", uid);
				dukjs_prop_num("tri"  , p->triangleIndex);
				return 1;
			}
		}
	}

	return 0;
}
duk_ret_t composer_sgl_to_screen(duk_context *J) {
	int n = duk_get_top(J);
	if (n > 2) {
		u_int x = duk_get_number(J, 0);
		u_int y = duk_get_number(J, 1);
		u_int z = duk_get_number(J, 2);

		S3L_Vec4 point = { x, y, z };
		S3L_Vec4 result = { 0 };
		sgl_project_point_to_screen(point, &result);

		duk_push_object(J);
		dukjs_prop_num("x", result.x);
		dukjs_prop_num("y", result.y);
		dukjs_prop_num("z", result.z);
		dukjs_prop_num("w", result.w);
		return 1;
	}

	return 0;
}
duk_ret_t composer_sgl_look_at(duk_context *J) {
	int n = duk_get_top(J);
	if (n > 2) {
		u_int x = duk_get_number(J, 0);
		u_int y = duk_get_number(J, 1);
		u_int z = duk_get_number(J, 2);

		S3L_Vec4 point = { x, y, z };
		sgl_look_at(point);
	}

	return 0;
}
duk_ret_t composer_sgl_point_at(duk_context *J) {
	int n = duk_get_top(J);
	if (n > 5) {
		u_int x = duk_get_number(J, 0); // target
		u_int y = duk_get_number(J, 1);
		u_int z = duk_get_number(J, 2);

		u_int a = duk_get_number(J, 3); // source
		u_int b = duk_get_number(J, 4);
		u_int c = duk_get_number(J, 5);

		S3L_Vec4 point = { x, y, z };
		S3L_Transform3D t = { 0 };
		S3L_transform3DInit(&t);
		t.translation.x = a;
		t.translation.y = b;
		t.translation.z = c;
		sgl_point_at(point, &t);

		duk_push_object(J);
		dukjs_prop_num("x", t.rotation.x);
		dukjs_prop_num("y", t.rotation.y);
		dukjs_prop_num("z", t.rotation.z);
		return 1;
	}

	return 0;
}

duk_ret_t composer_sgl_materials_count	(duk_context *J) {
	duk_push_number(J, sgl_get_total_materials());
	return 1;
}
duk_ret_t composer_sgl_materials_add	(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	u_int color = n > 0 ? duk_to_number(J, 0) : 0;
	duk_push_number(J, sgl_add_material(color));
	return 1;
}
duk_ret_t composer_sgl_materials_get	(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	u_int j = n > 0 ? duk_to_number(J, 0) : -1;
	sgl_material_t *mat = sgl_get_material(j);
	if (mat) {
		duk_push_object(J);
		dukjs_prop_num("uid"  , j			);
		dukjs_prop_num("color", mat->color	);
		return 1;
	}
	return 0;
}
duk_ret_t composer_sgl_materials_set	(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	u_int j = n > 0 ? duk_to_number(J, 0) : -1;
	u_int color = n > 1 ? duk_to_number(J, 1) : 0;
	sgl_set_material(j, color);
	return 0;
}
duk_ret_t composer_sgl_materials_remove	(duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	u_int j = n > 0 ? duk_to_number(J, 0) : -1;
	duk_push_number(J, sgl_remove_material(j));
	return 1;
}
*/}

/*{ camera
duk_ret_t composer_camera_translation (duk_context *J) {
	int n = duk_get_top(J);
	if (n > 3) {
		scene.camera.transform.translation.x = duk_get_number(J, 0);
		scene.camera.transform.translation.y = duk_get_number(J, 1);
		scene.camera.transform.translation.z = duk_get_number(J, 2);
		scene.camera.transform.translation.w = duk_get_number(J, 3);
		return 0;
	} else {
		duk_push_object(J);
		dukjs_prop_num("x", scene.camera.transform.translation.x);
		dukjs_prop_num("y", scene.camera.transform.translation.y);
		dukjs_prop_num("z", scene.camera.transform.translation.z);
		dukjs_prop_num("w", scene.camera.transform.translation.w);
		return 1;
	}
}
duk_ret_t composer_camera_rotation (duk_context *J) {
	int n = duk_get_top(J);
	if (n > 3) {
		scene.camera.transform.rotation.x = duk_get_number(J, 0);
		scene.camera.transform.rotation.y = duk_get_number(J, 1);
		scene.camera.transform.rotation.z = duk_get_number(J, 2);
		scene.camera.transform.rotation.w = duk_get_number(J, 3);
		return 0;
	} else {
		duk_push_object(J);
		dukjs_prop_num("x", scene.camera.transform.rotation.x);
		dukjs_prop_num("y", scene.camera.transform.rotation.y);
		dukjs_prop_num("z", scene.camera.transform.rotation.z);
		dukjs_prop_num("w", scene.camera.transform.rotation.w);
		return 1;
	}
}
duk_ret_t composer_camera_scale (duk_context *J) {
	int n = duk_get_top(J);
	if (n > 3) {
		scene.camera.transform.scale.x = S3L_max(0, duk_get_number(J, 0));
		scene.camera.transform.scale.y = S3L_max(0, duk_get_number(J, 1));
		scene.camera.transform.scale.z = S3L_max(0, duk_get_number(J, 2));
		scene.camera.transform.scale.w = S3L_max(0, duk_get_number(J, 3));
		return 0;
	} else {
		duk_push_object(J);
		dukjs_prop_num("x", scene.camera.transform.scale.x);
		dukjs_prop_num("y", scene.camera.transform.scale.y);
		dukjs_prop_num("z", scene.camera.transform.scale.z);
		dukjs_prop_num("w", scene.camera.transform.scale.w);
		return 1;
	}
}
duk_ret_t composer_camera_focal (duk_context *J) {
	int n = duk_get_top(J);
	if (n) {
		int foc = duk_get_number(J, 0);
		if (foc < 0) foc = 0;
		if (foc > 2048) foc = 2048;
		scene.camera.focalLength = foc;
		return 0;
	} else {
		duk_push_number(J, scene.camera.focalLength);
		return 1;
	}
}
*/}

//{ ease
duk_ret_t composer_ease (duk_context *J) {
	duk_idx_t n = duk_get_top(J);
	if (n > 4) {
		int index = duk_get_number(J, 0);
		float t = duk_get_number(J, 1);
		float b = duk_get_number(J, 2);
		float c = duk_get_number(J, 3);
		float d = duk_get_number(J, 4);
		
		duk_push_number(J, composer_ease_by_name(index, t, b, c, d));
		return 1;
	}
	return 0;
}
//}

static duk_ret_t native_print() {
	duk_push_string(J, " ");
	duk_insert(J, 0);
	duk_join(J, duk_get_top(J) - 1);
	taba3_waqt((char *) duk_safe_to_string(J, -1));
	return 0;
}
void print_duk_stack() {
	duk_int_t n = duk_get_top(J);
	printf("duk stack size %d\n", n);
	for (int i = 0; i < n; ++i) {
		switch (duk_get_type(J, i)) {
			case DUK_TYPE_NONE       :
				printf("%d none\n", i); break;
			case DUK_TYPE_UNDEFINED  :
				printf("%d undefined\n", i); break;
			case DUK_TYPE_NULL       :
				printf("%d null\n", i); break;
			case DUK_TYPE_BOOLEAN    :
				printf("%d bool %d\n", i, duk_get_boolean(J, i)); break;
			case DUK_TYPE_NUMBER     :
				printf("%d number %lf\n", i, duk_get_number(J, i)); break;
			case DUK_TYPE_STRING     :
				printf("%d string %s\n", i, duk_get_string(J, i)); break;
			case DUK_TYPE_OBJECT     :
				if (duk_is_array(J, i))
					printf("%d array\n", i); 
				else if (duk_is_function(J, i))
					printf("%d function\n", i);
				else
					printf("%d object\n", i);
				break;
			case DUK_TYPE_BUFFER     :
				printf("%d buffer\n", i); break;
			case DUK_TYPE_POINTER    :
				printf("%d pointer\n", i); break;
			case DUK_TYPE_LIGHTFUNC  :
				printf("%d lightfunc\n", i); break;
				break;
		}
	}
}
void load_js_file(char *path) {
	/* Use duk_safe_call() to wrap all unsafe code into a separate C function.
	* This approach has the advantage of covering all API calls automatically
	* but is a bit more verbose.
	*/
	
	duk_ret_t unsafe_code(duk_context *J, void *udata) {
		// Here we can use unprotected calls freely
	
		(void) udata;
		malaf mlf;
		if ( malaf_init(&mlf, path) == EXIT_SUCCESS ) {
			duk_push_string(J, (char *) mlf.barzax);

			duk_push_string(J, path);
			duk_compile(J, 0);  // [ ... source filename ] -> [ function ]
			duk_call(J, 0);

			duk_pop(J); // pop eval result

			malaf_destroy(&mlf);
		}
	
		return 0;  // success return, no return value
	}
	
	if (duk_safe_call(J, unsafe_code, NULL /*udata*/, 0 /*nargs*/, 1 /*nrets */) != 0) {
		/* The 'nrets' argument should be at least 1 so that an error value
		* is left on the stack if an error occurs.  To avoid further errors,
		* use duk_safe_to_string() for safe error printing.
		*/
		duk_safe_to_stacktrace(J, -1); // TODO TEST more
		printf("Unexpected error: %s\n", duk_safe_to_string(J, -1));
	}
	
	duk_pop(J); // pop eval result
}
void push_c_function(char *name, void *function) {
	duk_push_c_function(J, function, DUK_VARARGS);
	duk_put_global_string(J, name);
}
void push_c_method(char *name, void *function) {
	duk_push_c_function(J, function, DUK_VARARGS);
	duk_put_prop_string(J, -2, name); // AND what about this -2
}
int if_js_function(char *name) {
	if (XATAA > 1) printf("if_js_function %s\n", name);
	duk_idx_t func_idx = duk_get_global_string(J, name);
	if (func_idx) {
		// leaves string on stack, either pop it manually OR
		// call js_func_with_stack, and that does it for you
		return 1;
	} else {
		if (XATAA > 1) printf("if_js_function %s is undefined\n", name);
		duk_pop(J);
		return 0;
	}
}
int call_js_function_with_stack(char *name, int args) {
	if (XATAA > 1) printf("call_js_function_with_stack %s %d\n", name, args);
	if (XATAA > 2) print_duk_stack();
	duk_int_t rc = duk_pcall(J, args);  /* [ ... func 2 3 ] -> [ 5 ] */
	int retval = 0;
	if (rc == DUK_EXEC_SUCCESS) {
		retval = duk_get_int(J, -1);
		duk_pop(J);
	} else {
		if (duk_is_error(J, -1)) duk_safe_to_stacktrace(J, -1);
		printf("error: %s\n", duk_safe_to_string(J, -1));
		duk_pop(J);
	}
	return retval;
}
void on_bound_window(ihaatahf ih) {
	s3l_set_resolution(ih.w, ih.h);
	duk_idx_t obj_idx = duk_push_object(J);
	{
		duk_push_number(J, ih.x  ), duk_put_prop_string(J, obj_idx, "x");
		duk_push_number(J, ih.y  ), duk_put_prop_string(J, obj_idx, "y");
		duk_push_number(J, ih.w  ), duk_put_prop_string(J, obj_idx, "w");
		duk_push_number(J, ih.h  ), duk_put_prop_string(J, obj_idx, "h");
	}
	duk_put_global_string(J, "window");
	if (if_js_function("on_bound_window")) {
		duk_idx_t obj_idx = duk_push_object(J);
		{
			duk_push_number(J, ih.x  ), duk_put_prop_string(J, obj_idx, "x");
			duk_push_number(J, ih.y  ), duk_put_prop_string(J, obj_idx, "y");
			duk_push_number(J, ih.w  ), duk_put_prop_string(J, obj_idx, "w");
			duk_push_number(J, ih.h  ), duk_put_prop_string(J, obj_idx, "h");
		}
		call_js_function_with_stack("on_bound_window", 1);
	}
}
void on_bound_indicator(ihaatahf ih) {
	duk_idx_t obj_idx = duk_push_object(J);
	{
		duk_push_number(J, ih.x  ), duk_put_prop_string(J, obj_idx, "x");
		duk_push_number(J, ih.y  ), duk_put_prop_string(J, obj_idx, "y");
		duk_push_number(J, ih.w  ), duk_put_prop_string(J, obj_idx, "w");
		duk_push_number(J, ih.h  ), duk_put_prop_string(J, obj_idx, "h");
	}
	duk_put_global_string(J, "indicator");
	if (if_js_function("on_bound_indicator")) {
		duk_idx_t obj_idx = duk_push_object(J);
		{
			duk_push_number(J, ih.x  ), duk_put_prop_string(J, obj_idx, "x");
			duk_push_number(J, ih.y  ), duk_put_prop_string(J, obj_idx, "y");
			duk_push_number(J, ih.w  ), duk_put_prop_string(J, obj_idx, "w");
			duk_push_number(J, ih.h  ), duk_put_prop_string(J, obj_idx, "h");
		}
		call_js_function_with_stack("on_bound_indicator", 1);
	}
}
char on_paint_window() {
	if (!if_js_function("on_paint_window")) return 0;
	return call_js_function_with_stack("on_paint_window", 0);
}
char on_paint_indicator() {
	if (!if_js_function("on_paint_indicator")) return 0;
	return call_js_function_with_stack("on_paint_indicator", 0);
}
void on_focus(char yes) {
	if (!if_js_function("on_focus")) return;
	duk_push_number(J, yes);
	call_js_function_with_stack("on_focus", 1);
}
char on_pointer(mafateeh wm) {
	pointer_x = wm.x; pointer_y = wm.y;
	
	if (!if_js_function("on_pointer")) return 0;
	duk_idx_t obj_idx = duk_push_object(J);
	{
		if (wm.ctrl)
			duk_push_number(J, wm.ctrl ), duk_put_prop_string(J, obj_idx, "ctrl");
		if (wm.shift)
			duk_push_number(J, wm.shift), duk_put_prop_string(J, obj_idx, "shift");
		if (wm.alt)
			duk_push_number(J, wm.alt  ), duk_put_prop_string(J, obj_idx, "alt");
		if (wm.meta)
			duk_push_number(J, wm.meta ), duk_put_prop_string(J, obj_idx, "meta");
		if (wm.state)
			duk_push_number(J, wm.state), duk_put_prop_string(J, obj_idx, "state");
		
		duk_push_number(J, wm.x  ), duk_put_prop_string(J, obj_idx, "x");
		duk_push_number(J, wm.y  ), duk_put_prop_string(J, obj_idx, "y");
		duk_push_number(J, wm.w  ), duk_put_prop_string(J, obj_idx, "w");
		duk_push_number(J, wm.key), duk_put_prop_string(J, obj_idx, "n"); // num of points

//		duk_push_string(J, wm.ism   ); duk_put_prop_string(J, obj_idx, "name");
//		duk_push_string(J, wm.huroof); duk_put_prop_string(J, obj_idx, "text");
	}
	return call_js_function_with_stack("on_pointer", 1);
}
char on_keyboard(mafateeh wm) {
	if (!if_js_function("on_keyboard")) return 0;
	duk_idx_t obj_idx = duk_push_object(J);
	{
		if (wm.ctrl)
			duk_push_number(J, wm.ctrl ), duk_put_prop_string(J, obj_idx, "ctrl");
		if (wm.shift)
			duk_push_number(J, wm.shift), duk_put_prop_string(J, obj_idx, "shift");
		if (wm.alt)
			duk_push_number(J, wm.alt  ), duk_put_prop_string(J, obj_idx, "alt");
		if (wm.meta)
			duk_push_number(J, wm.meta ), duk_put_prop_string(J, obj_idx, "meta");
		if (wm.state)
			duk_push_number(J, wm.state), duk_put_prop_string(J, obj_idx, "state");
		
		duk_push_number(J, wm.key), duk_put_prop_string(J, obj_idx, "key");

		duk_push_string(J, wm.ism   ); duk_put_prop_string(J, obj_idx, "name");
		duk_push_string(J, wm.huroof); duk_put_prop_string(J, obj_idx, "text");
	}
	return call_js_function_with_stack("on_keyboard", 1);
}
void after_canvas_raees(msfoof *m) {
	if (queuereload) {
		queuereload = 0;
		composer_load_app(1);
	}
}
void on_reload() {
	physics_init();
	sgl_init();

	if (J) duk_destroy_heap(J);
	J = duk_create_heap_default();
	if (!J) { printf("%s\n", "cannot create heap"); exit(1); }

	duk_push_object(J); {
		push_c_method("list",	composer_files_list			);
	}
	duk_put_global_string(J, "files");

	duk_push_object(J); {
		push_c_method("coord2angle",	composer_msfoof_coord2angle		);
		push_c_method("angle2coord",	composer_msfoof_angle2coord		);

		push_c_method("to_radians" ,	composer_msfoof_to_radians		);
		push_c_method("to_degrees" ,	composer_msfoof_to_degrees		);

		push_c_method("contains",		composer_msfoof_contains		);
		push_c_method("distance",		composer_msfoof_distance		);
		push_c_method("overlap",		composer_msfoof_overlap			);
		push_c_method("clear",			composer_msfoof_clear			);
	}
	duk_put_global_string(J, "matrix");

	duk_push_object(J); {
		push_c_method("to_components",	composer_colors_to_components	);
		push_c_method("to_integer",		composer_colors_to_integer		);
	}
	duk_put_global_string(J, "colors");

	duk_push_object(J); {
		push_c_method("with_name",	composer_ease	);
	}
	duk_put_global_string(J, "ease");

	duk_push_object(J); {
		push_c_method("flush",		composer_canvas_flush			);
		push_c_method("blur",		composer_canvas_blur			);
		push_c_method("move_to",	composer_canvas_move_to			);
		push_c_method("xat",		composer_canvas_xat				); // font face
		push_c_method("font_size",	composer_canvas_font_size		);
		push_c_method("font_bounds",composer_canvas_font_bounds		);
		push_c_method("text",		composer_canvas_text			);
		push_c_method("rect",		composer_canvas_rect			);
		push_c_method("circle",		composer_canvas_circle			);
		push_c_method("line",		composer_canvas_line			);
		push_c_method("bezier",		composer_canvas_bezier			);
		push_c_method("stroke",		composer_canvas_stroke			);
		push_c_method("fill",		composer_canvas_fill			);
		push_c_method("lwn",		composer_canvas_lwn				); // color
		push_c_method("paint",		composer_canvas_idhan			);
		push_c_method("line_width",	composer_canvas_line_width		);
		push_c_method("set_dash",	composer_canvas_set_dash		);
		push_c_method("renew_path",	composer_canvas_renew_path		);
		push_c_method("clip",		composer_canvas_clip			);
		push_c_method("unclip",		composer_canvas_unclip			);
		push_c_method("restore",	composer_canvas_restore			);
		push_c_method("save",		composer_canvas_save			);
		push_c_method("state",		composer_canvas_state			); // canvas on/off
		push_c_method("scale",		composer_canvas_scale			);
		push_c_method("rotate",		composer_canvas_rotate			);
		push_c_method("translate",	composer_canvas_translate		);
		push_c_method("device2user",composer_canvas_device_to_user	);
		push_c_method("user2device",composer_canvas_user_to_device	);
	}
	duk_put_global_string(J, "canvas");

	duk_push_object(J); {
	}
	duk_put_global_string(J, "sgl");

	duk_push_object(J); {
	}
	duk_put_global_string(J, "camera");

	duk_push_object(J); {
		push_c_method("run",			composer_process_run);
	}
	duk_put_global_string(J, "process");

	push_c_function("log", native_print);
	push_c_function("reload", composer_reload);
	push_c_function("exit", composer_exit);
	push_c_function("clear", composer_console_clear);
	push_c_function("require", composer_require);

	load_js_file("apps/eventcodes.js");
	load_js_file("apps/core/ease.js");

	load_js_file(appname);
}
int main(int argc, char **argv, char *envp[]) {
	if (argc >= 2) {
		prctl(PR_SET_NAME, (unsigned long) argv[1], 0, 0, 0);

		environment = envp;

		appname = helper_stringify("apps/%s.js", argv[1]);

		composer dukcmpsr = {
			.ism = argv[1],
			.xitaab = appname,
			.on_bound_window = &on_bound_window,
			.on_bound_indicator = &on_bound_indicator,
			.on_paint_window = &on_paint_window,
			.on_paint_indicator = &on_paint_indicator,
			.on_pointer = &on_pointer,
			.b_lowhah = &on_keyboard,
			.on_reload = &on_reload,
			.after_canvas_raees = &after_canvas_raees,
			.on_focus = &on_focus,
		};

		sdl_composer_bridge(&dukcmpsr);

		composer_init(dukcmpsr);

		if (J) duk_destroy_heap(J);

		composer_tamaam();
		free(appname);
	} else {
		printf("could not find app\n");
	}
}

