/*
 * duk-c funcs should return 1 if they wanna do duk_push_* at the end
 * 
 * 
 * */
#include <SDL2/SDL.h>
#include "duktape.h"

// TODO fixme
#define COMPOSER_SDL
#include "composer.h"
#include "common.h"

#include "map.h"
#include "shader.h"
#include "sdl.h"
#include "sgl.h"
#include "ttf.h"
#include "nuk.h"
#include "aud.h"
#include "asio.h"
#include "codec.h"

#include <sys/prctl.h>

#define XATAA 0

char **environment;
u_char *appname = NULL;
u_char *scripts_path = NULL;
char queuereload = 0;
int argumentcount = 0;
char **argumentvalues = NULL;

composer duk_composer;

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;
typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;
typedef Vector4 Quaternion;
typedef struct Matrix {
    float m0, m4, m8, m12;
    float m1, m5, m9, m13;
    float m2, m6, m10, m14;
    float m3, m7, m11, m15;
} Matrix;
Matrix MatrixIdentity(void) {
    Matrix result = { 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f };

    return result;
}
Vector3 Vector3Unproject(Vector3 source, Matrix projection, Matrix view) {
    Vector3 result = { 0 };

    // Calculate unprojected matrix (multiply view matrix by projection matrix) and invert it
    Matrix matViewProj = {      // MatrixMultiply(view, projection);
        view.m0*projection.m0 + view.m1*projection.m4 + view.m2*projection.m8 + view.m3*projection.m12,
        view.m0*projection.m1 + view.m1*projection.m5 + view.m2*projection.m9 + view.m3*projection.m13,
        view.m0*projection.m2 + view.m1*projection.m6 + view.m2*projection.m10 + view.m3*projection.m14,
        view.m0*projection.m3 + view.m1*projection.m7 + view.m2*projection.m11 + view.m3*projection.m15,
        view.m4*projection.m0 + view.m5*projection.m4 + view.m6*projection.m8 + view.m7*projection.m12,
        view.m4*projection.m1 + view.m5*projection.m5 + view.m6*projection.m9 + view.m7*projection.m13,
        view.m4*projection.m2 + view.m5*projection.m6 + view.m6*projection.m10 + view.m7*projection.m14,
        view.m4*projection.m3 + view.m5*projection.m7 + view.m6*projection.m11 + view.m7*projection.m15,
        view.m8*projection.m0 + view.m9*projection.m4 + view.m10*projection.m8 + view.m11*projection.m12,
        view.m8*projection.m1 + view.m9*projection.m5 + view.m10*projection.m9 + view.m11*projection.m13,
        view.m8*projection.m2 + view.m9*projection.m6 + view.m10*projection.m10 + view.m11*projection.m14,
        view.m8*projection.m3 + view.m9*projection.m7 + view.m10*projection.m11 + view.m11*projection.m15,
        view.m12*projection.m0 + view.m13*projection.m4 + view.m14*projection.m8 + view.m15*projection.m12,
        view.m12*projection.m1 + view.m13*projection.m5 + view.m14*projection.m9 + view.m15*projection.m13,
        view.m12*projection.m2 + view.m13*projection.m6 + view.m14*projection.m10 + view.m15*projection.m14,
        view.m12*projection.m3 + view.m13*projection.m7 + view.m14*projection.m11 + view.m15*projection.m15 };

    // Calculate inverted matrix -> MatrixInvert(matViewProj);
    // Cache the matrix values (speed optimization)
    float a00 = matViewProj.m0, a01 = matViewProj.m1, a02 = matViewProj.m2, a03 = matViewProj.m3;
    float a10 = matViewProj.m4, a11 = matViewProj.m5, a12 = matViewProj.m6, a13 = matViewProj.m7;
    float a20 = matViewProj.m8, a21 = matViewProj.m9, a22 = matViewProj.m10, a23 = matViewProj.m11;
    float a30 = matViewProj.m12, a31 = matViewProj.m13, a32 = matViewProj.m14, a33 = matViewProj.m15;

    float b00 = a00*a11 - a01*a10;
    float b01 = a00*a12 - a02*a10;
    float b02 = a00*a13 - a03*a10;
    float b03 = a01*a12 - a02*a11;
    float b04 = a01*a13 - a03*a11;
    float b05 = a02*a13 - a03*a12;
    float b06 = a20*a31 - a21*a30;
    float b07 = a20*a32 - a22*a30;
    float b08 = a20*a33 - a23*a30;
    float b09 = a21*a32 - a22*a31;
    float b10 = a21*a33 - a23*a31;
    float b11 = a22*a33 - a23*a32;

    // Calculate the invert determinant (inlined to avoid double-caching)
    float invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

    Matrix matViewProjInv = {
        (a11*b11 - a12*b10 + a13*b09)*invDet,
        (-a01*b11 + a02*b10 - a03*b09)*invDet,
        (a31*b05 - a32*b04 + a33*b03)*invDet,
        (-a21*b05 + a22*b04 - a23*b03)*invDet,
        (-a10*b11 + a12*b08 - a13*b07)*invDet,
        (a00*b11 - a02*b08 + a03*b07)*invDet,
        (-a30*b05 + a32*b02 - a33*b01)*invDet,
        (a20*b05 - a22*b02 + a23*b01)*invDet,
        (a10*b10 - a11*b08 + a13*b06)*invDet,
        (-a00*b10 + a01*b08 - a03*b06)*invDet,
        (a30*b04 - a31*b02 + a33*b00)*invDet,
        (-a20*b04 + a21*b02 - a23*b00)*invDet,
        (-a10*b09 + a11*b07 - a12*b06)*invDet,
        (a00*b09 - a01*b07 + a02*b06)*invDet,
        (-a30*b03 + a31*b01 - a32*b00)*invDet,
        (a20*b03 - a21*b01 + a22*b00)*invDet };

    // Create quaternion from source point
    Quaternion quat = { source.x, source.y, source.z, 1.0f };

    // Multiply quat point by unprojecte matrix
    Quaternion qtransformed = {     // QuaternionTransform(quat, matViewProjInv)
        matViewProjInv.m0*quat.x + matViewProjInv.m4*quat.y + matViewProjInv.m8*quat.z + matViewProjInv.m12*quat.w,
        matViewProjInv.m1*quat.x + matViewProjInv.m5*quat.y + matViewProjInv.m9*quat.z + matViewProjInv.m13*quat.w,
        matViewProjInv.m2*quat.x + matViewProjInv.m6*quat.y + matViewProjInv.m10*quat.z + matViewProjInv.m14*quat.w,
        matViewProjInv.m3*quat.x + matViewProjInv.m7*quat.y + matViewProjInv.m11*quat.z + matViewProjInv.m15*quat.w };

    // Normalized world points in vectors
    result.x = qtransformed.x/qtransformed.w;
    result.y = qtransformed.y/qtransformed.w;
    result.z = qtransformed.z/qtransformed.w;

    return result;
}
static void print_vec3(Vector3 m) {
	printf("%.3f %.3f %.3f\n",
		m.x, m.y, m.z
	);
}
static void print_mat4(Matrix m) {
    printf("%.3f %.3f %.3f %.3f\n", m.m0, m.m4, m.m8 , m.m12 );
    printf("%.3f %.3f %.3f %.3f\n", m.m1, m.m5, m.m9 , m.m13 );
    printf("%.3f %.3f %.3f %.3f\n", m.m2, m.m6, m.m10, m.m14 );
    printf("%.3f %.3f %.3f %.3f\n", m.m3, m.m7, m.m11, m.m15 );
}

duk_ret_t raymath_unproject(duk_context *J) {
	int n = duk_get_top(J);
	if (n > 2) {
		Vector3 result = {0};
		Vector3 source = {0};
		Matrix projection = MatrixIdentity();
		Matrix view = MatrixIdentity();

		float float3[3] = {0};
		for (int i = 0; i < 3; ++i) {
			duk_get_prop_index(J, 0, i);
			float3[i] = duk_get_number(J, -1);
			duk_pop(J);
		}

		source.x = float3[0];
		source.y = float3[1];
		source.z = float3[2];

		float ff[16] = {0};
		for (int i = 0; i < 16; ++i) { // proj
			duk_get_prop_index(J, 1, i);
			ff[i] = duk_get_number(J, -1);
			duk_pop(J);
		}
		projection = (Matrix) {
			ff[ 0], ff[ 4], ff[ 8], ff[12],
			ff[ 1], ff[ 5], ff[ 9], ff[13],
			ff[ 2], ff[ 6], ff[10], ff[14],
			ff[ 3], ff[ 7], ff[11], ff[15],
		};

		for (int i = 0; i < 16; ++i) { // proj
			duk_get_prop_index(J, 2, i);
			ff[i] = duk_get_number(J, -1);
			duk_pop(J);
		}
		view = (Matrix) {
			ff[ 0], ff[ 4], ff[ 8], ff[12],
			ff[ 1], ff[ 5], ff[ 9], ff[13],
			ff[ 2], ff[ 6], ff[10], ff[14],
			ff[ 3], ff[ 7], ff[11], ff[15],
		};

		result = Vector3Unproject(source, projection, view);
		
		duk_push_array(J);
		dukjs_prop_num("0", result.x);
		dukjs_prop_num("1", result.y);
		dukjs_prop_num("2", result.z);
		return 1;
	}
	return 0;
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

//{ globals
bindjs (composer_exit) {
	exit(0);
	return 0;
}
bindjs (composer_reload) {
	queuereload = 1;
	return 0;
}
bindjs (composer_frame_rate) {
	int n = duk_get_top(J);
	if (n && duk_get_type(J, 0) == DUK_TYPE_NUMBER) {
		int fps = duk_get_number(J, 0);
		sdl_set_frame_rate(fps);
	} else {
		duk_push_number(J, sdl_get_frame_rate());
		return 1;
	}
	return 0;
}
bindjs (composer_require) {
	int n = duk_get_top(J);
	if (n && duk_get_type(J, 0) == DUK_TYPE_STRING) {
		const char *str = duk_safe_to_string(J, 0);
		load_js_file_x(J, (char *) str);
	}
	return 0;
}
bindjs (composer_console_clear) {
	composer_clear();
	return 0;
}
bindjs (composer_process_run) {
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

//{ ease
bindjs (composer_ease) {
	duk_idx_t n = duk_get_top(J);
	if (n > 4) {
		int index = duk_get_number(J, 0);
		double t = duk_get_number(J, 1);
		double b = duk_get_number(J, 2);
		double c = duk_get_number(J, 3);
		double d = duk_get_number(J, 4);
		
		duk_push_number(J, composer_ease_by_name(index, t, b, c, d));
		return 1;
	}
	return 0;
}
//}

//{ composer
bindjs (dukjs_composer_cursor) {
	duk_idx_t n = duk_get_top(J);
	if (n) composer_cursor(duk_get_number(J, 0));
	else {
		duk_push_number(J, composer_cursor(-1));
		return 1;
	}
	return 0;
}
bindjs (dukjs_composer_title) {
	duk_idx_t n = duk_get_top(J);
	if (n) duk_composer.title(duk_to_string(J, 0));
	return 0;
}
bindjs (dukjs_composer_damage) {
	WJHH.raees.mutadarrar = 1;
	return 0;
}
//}

bindjs (composer_print) {
	duk_push_string(J, " ");
	duk_insert(J, 0);
	duk_join(J, duk_get_top(J) - 1);
	taba3_waqt((char *) duk_safe_to_string(J, -1));
	return 0;
}
void on_bound_window(ihaatahf ih) {
	duk_idx_t obj_idx = duk_push_object(J);
	{
		duk_push_number(J, ih.x  ), duk_put_prop_string(J, obj_idx, "x");
		duk_push_number(J, ih.y  ), duk_put_prop_string(J, obj_idx, "y");
		duk_push_number(J, ih.w  ), duk_put_prop_string(J, obj_idx, "w");
		duk_push_number(J, ih.h  ), duk_put_prop_string(J, obj_idx, "h");
	}
	duk_put_global_string(J, "Window");
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
char on_focus(char yes) {
	if (!if_js_function("on_focus")) return 0;
	duk_push_number(J, yes);
	return call_js_function_with_stack("on_focus", 1);
}
char on_destroy() {
	if (!if_js_function("on_destroy")) return 0;
	return call_js_function_with_stack("on_destroy", 0);
}
char on_touch(mafateeh wm) {
	if (!if_js_function("on_touch")) return 0;
	duk_push_object(J);
	if (wm.ctrl ) dukjs_prop_num("ctrl" , wm.ctrl );
	if (wm.shift) dukjs_prop_num("shift", wm.shift);
	if (wm.alt  ) dukjs_prop_num("alt",   wm.alt  );
	if (wm.meta ) dukjs_prop_num("meta",  wm.meta );
	if (wm.state) dukjs_prop_num("state", wm.state);
	dukjs_prop_num("x", wm.x );
	dukjs_prop_num("y", wm.y );
	dukjs_prop_num("w", wm.w ); // touch id
	dukjs_prop_num("wx", wm.wx );
	dukjs_prop_num("wy", wm.wy );
	return call_js_function_with_stack("on_touch", 1);
}
char on_rotate(mafateeh wm) {
	if (!if_js_function("on_rotate")) return 0;
	duk_push_object(J);
	if (wm.ctrl ) dukjs_prop_num("ctrl" , wm.ctrl );
	if (wm.shift) dukjs_prop_num("shift", wm.shift);
	if (wm.alt  ) dukjs_prop_num("alt",   wm.alt  );
	if (wm.meta ) dukjs_prop_num("meta",  wm.meta );
	if (wm.state) dukjs_prop_num("state", wm.state);
	dukjs_prop_num("x", wm.x  );
	dukjs_prop_num("y", wm.y  );
	dukjs_prop_num("w", wm.w  ); // rotation in radians
	return call_js_function_with_stack("on_rotate", 1);
}
void on_tick() { // IMPORTANT apps misbehave without this
	asio_run();
	if (!if_js_function("on_tick")) return;
	call_js_function_with_stack("on_tick", 0);
}
void on_drop(char *file) {
	if (!if_js_function("on_drop")) return;
	duk_push_string(J, file);
	call_js_function_with_stack("on_drop", 1);
}
char on_pinch(mafateeh wm) {
	if (!if_js_function("on_pinch")) return 0;
	duk_push_object(J);
	if (wm.ctrl ) dukjs_prop_num("ctrl" , wm.ctrl );
	if (wm.shift) dukjs_prop_num("shift", wm.shift);
	if (wm.alt  ) dukjs_prop_num("alt",   wm.alt  );
	if (wm.meta ) dukjs_prop_num("meta",  wm.meta );
	if (wm.state) dukjs_prop_num("state", wm.state);
	dukjs_prop_num("x", wm.x  );
	dukjs_prop_num("y", wm.y  );
	dukjs_prop_num("w", wm.w  );
	dukjs_prop_num("state", wm.state); // open close
	return call_js_function_with_stack("on_pinch", 1);
}
char on_pointer(mafateeh wm) {
	if (!if_js_function("on_pointer")) return 0;
	duk_push_object(J);
	if (wm.ctrl ) dukjs_prop_num("ctrl" , wm.ctrl );
	if (wm.shift) dukjs_prop_num("shift", wm.shift);
	if (wm.alt  ) dukjs_prop_num("alt",   wm.alt  );
	if (wm.meta ) dukjs_prop_num("meta",  wm.meta );
	if (wm.state) dukjs_prop_num("state", wm.state);
	dukjs_prop_num("x", wm.x  );
	dukjs_prop_num("y", wm.y  );
	dukjs_prop_num("w", wm.w  );
	dukjs_prop_num("n", wm.key); // num of points
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
static void duk_js_fatal(void *udata, const char *msg) {
	// without this, errs inside callbacks are not reported
	(void) udata;
	printf("fatal in main %s\n", (msg ? msg : "no message"));
	abort();
}
duk_ret_t app_duk_js_bind(duk_context *J) {
	duk_push_global_object(J);
	duk_compact(J, -1); // reduces mem usage

	duk_dup(J, -1);
	duk_put_prop_string(J, -2, "global");
	duk_pop(J);

	msfoof *m = &WJHH.raees.mtrx;
	duk_idx_t obj_idx = duk_push_object(J);
	{
		duk_push_number(J,    0  ), duk_put_prop_string(J, obj_idx, "x");
		duk_push_number(J,    0  ), duk_put_prop_string(J, obj_idx, "y");
		duk_push_number(J, m->w  ), duk_put_prop_string(J, obj_idx, "w");
		duk_push_number(J, m->h  ), duk_put_prop_string(J, obj_idx, "h");
	}
	duk_put_global_string(J, "Window");
	
	aud_bind_js(J);
	ttf_bind_js(J);
	sgl_bind_js(J);
	asio_bind_js(J);
	codec_bind_js(J);

	duk_push_object(J); {
		push_c_method_x(J, "coord2angle",	composer_msfoof_coord2angle		);
		push_c_method_x(J, "angle2coord",	composer_msfoof_angle2coord		);

		push_c_method_x(J, "to_radians" ,	composer_msfoof_to_radians		);
		push_c_method_x(J, "to_degrees" ,	composer_msfoof_to_degrees		);

		push_c_method_x(J, "contains",		composer_msfoof_contains		);
		push_c_method_x(J, "distance",		composer_msfoof_distance		);
		push_c_method_x(J, "overlap",		composer_msfoof_overlap			);
	}
	duk_put_global_string(J, "matrix");

	duk_push_object(J); {
		push_c_method_x(J, "to_components",	composer_colors_to_components	);
		push_c_method_x(J, "to_integer",		composer_colors_to_integer		);
	}
	duk_put_global_string(J, "colors");

	duk_push_object(J); {
		push_c_method_x(J, "with_name",	composer_ease	);
	}
	duk_put_global_string(J, "ease");

	duk_push_object(J); {
		push_c_method_x(J, "title", dukjs_composer_title);
		push_c_method_x(J, "cursor", dukjs_composer_cursor);
		push_c_method_x(J, "damage", dukjs_composer_damage);
	}
	duk_put_global_string(J, "composer");

	nuk_bind_js(J);

	duk_push_object(J); {
		push_c_method_x(J, "run", composer_process_run);
	}
	duk_put_global_string(J, "process");

	duk_push_object(J); {
		duk_push_string(J, scripts_path);
		duk_put_prop_string(J, -2, "scripts_path");
	}
	duk_put_global_string(J, "Nizaam");

	push_c_function_x(J, "log", composer_print);
	push_c_function_x(J, "reload", composer_reload);
	push_c_function_x(J, "frame_rate", composer_frame_rate);
	push_c_function_x(J, "exit", composer_exit);
	push_c_function_x(J, "clear", composer_console_clear);
	push_c_function_x(J, "require", composer_require);
	push_c_function_x(J, "raymath_unproject", raymath_unproject);
	return 0;
}
void on_reload() {
	asio_destroy();
	
	if (J) {
		on_destroy();
		duk_push_global_object(J);
		duk_compact(J, -1);
		duk_pop(J);
		duk_destroy_heap(J);
	}
	J = duk_create_heap(NULL, NULL, NULL, asio_loop(), duk_js_fatal);
	if (!J) { printf("cannot create heap\n"); exit(1); }

	asio_start(J, argumentcount, argumentvalues);
	
	// UGLY hack
	canvas_open = 0; window_open = 0;
	
	aud_destroy();
	aud_start();
	
	ttf_destroy();
	ttf_start();
	
	// TODO this should be just one composer_* call
	sdl_destroy_gl();
	if ( !sdl_init_gl() ) { printf("cannot init opengl\n"); exit(1); }
	
	sgl_destroy();
	if ( !sgl_init() ) { printf("cannot init sgl\n"); }

	codec_destroy();
	codec_start();

	app_duk_js_bind(J);

	u_char *final_path = helper_stringify("%s%s", scripts_path, "scripts/main.js");

	load_js_file(final_path);

	free(final_path);
}
void nizaam_guess_scripts_path() {
	size_t size = 2*PATH_MAX;
	char exe_path[2*PATH_MAX];
	uv_exepath(exe_path, &size);
	
	int slashes = 0, letters = 0;
	for (int i = size; i > -1; --i) {
		if (exe_path[i] == '\\' || exe_path[i]  == '/')
			slashes++;

		if (slashes >= 2) break;
		letters++;
	}

	scripts_path = malloc( (size - letters + 1) * sizeof(u_char) );
	strncpy(scripts_path, exe_path, size - letters + 1);
	scripts_path[size - letters + 1] = '\0';
}
int main(int argc, char **argv, char *envp[]) {
	if (argc >= 2) {
		J = NULL; // just to be safe
//		prctl(PR_SET_NAME, (unsigned long) argv[0], 0, 0, 0);

		argumentcount = argc;
		argumentvalues = argv;
		environment = envp;

		nizaam_guess_scripts_path();

		asio_set_binder(app_duk_js_bind);

		appname = argv[1];

		duk_composer = (composer) {
			.ism = argv[1],
			.xitaab = appname,
			.on_bound_window = &on_bound_window,
			.on_bound_indicator = &on_bound_indicator,
			.on_paint_window = &on_paint_window,
			.on_paint_indicator = &on_paint_indicator,
			.on_tick = &on_tick,
			.on_drop = &on_drop,
			.on_touch = &on_touch,
			.on_pinch = &on_pinch,
			.on_rotate = &on_rotate,
			.on_pointer = &on_pointer,
			.b_lowhah = &on_keyboard,
			.on_reload = &on_reload,
			.after_canvas_raees = &after_canvas_raees,
			.on_focus = &on_focus,
		};

		sdl_composer_bridge(&duk_composer);

		composer_init(duk_composer);

		if (J) duk_destroy_heap(J);
		if (scripts_path) free(scripts_path);

		composer_destroy();
	} else {
		printf("could not find app\n");
	}
}

