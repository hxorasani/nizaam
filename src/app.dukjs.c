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
#include "naett.h"

#include <sys/prctl.h>

#define XATAA 0

char **environment; u_char *appname; char queuereload = 0;
int argumentcount = 0;
char **argumentvalues = NULL;

composer duk_composer;

char fetch_pending = 0;
naettReq *fetch_req = NULL; naettRes *fetch_res = NULL;
void check_fetch_status() {
	if (!fetch_res || !fetch_req) return;
	if (!naettComplete(fetch_res)) return;

	int status = naettGetStatus(fetch_res);

	if (status < 0) {
		printf("Request failed: %d\n", status);
		fetch_pending = 0;
		return;
	}

	int bodyLength = 0;
	const char *body = naettGetBody(fetch_res, &bodyLength);
	printf("Got a %d, %d bytes of type '%s':\n\n", naettGetStatus(fetch_res), bodyLength, naettGetHeader(fetch_res, "Content-Type"));

	if (!if_js_function("on_response")) return;
	duk_push_string(J, body);
	call_js_function_with_stack("on_response", 1);

	naettClose(fetch_res);
	naettFree(fetch_req);
	fetch_res = NULL;
	fetch_req = NULL;
	fetch_pending = 0;
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
bindjs (composer_fetch) {
	int n = duk_get_top(J);
	if (n && duk_get_type(J, 0) == DUK_TYPE_STRING && !fetch_pending) {
		const char *str = duk_safe_to_string(J, 0);
		printf("fetch %s\n", str);

		fetch_req = naettRequest(str, naettMethod("GET"), naettHeader("accept", "*/*"));
		fetch_res = naettMake(fetch_req);
		
		fetch_pending = 1;
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
	check_fetch_status();
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

	push_c_function_x(J, "log", composer_print);
	push_c_function_x(J, "reload", composer_reload);
	push_c_function_x(J, "frame_rate", composer_frame_rate);
	push_c_function_x(J, "exit", composer_exit);
	push_c_function_x(J, "clear", composer_console_clear);
	push_c_function_x(J, "require", composer_require);
	push_c_function_x(J, "fetch", composer_fetch);
	return 0;
}
void on_reload() {
	asio_destroy();
	
	if (J) { on_destroy(); duk_destroy_heap(J); }
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

	load_js_file(appname);
}
int main(int argc, char **argv, char *envp[]) {
	if (argc >= 2) {
		J = NULL; // just to be safe
		prctl(PR_SET_NAME, (unsigned long) argv[1], 0, 0, 0);

		argumentcount = argc;
		argumentvalues = argv;
		environment = envp;
		naettInit(NULL);
		asio_set_binder(app_duk_js_bind);

		appname = helper_stringify("apps/%s.js", argv[1]);

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

		composer_destroy();
		free(appname);
	} else {
		printf("could not find app\n");
	}
}

