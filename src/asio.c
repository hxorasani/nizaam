#include "asio.h"
#define XATAA 0

static uv_loop_t loop;
uv_loop_t *asio_loop() {
	return &loop;
}

// TODO integrate uv_* based require, load_dll etc from dukluv

static duk_ret_t duv_stash_argv(duk_context *ctx, void *udata) {
	char **argv = (char **) duk_require_pointer(ctx, 0);
	int argc = (int) duk_require_int(ctx, 1);
	int i;
	
	(void) udata;
	
	duk_push_global_stash(ctx);
	duk_push_array(ctx);
	for (i = 0; i < argc; i++) {
		duk_push_string(ctx, argv[i]);
		duk_put_prop_index(ctx, -2, i);
	}
	duk_put_prop_string(ctx, -2, "argv");
	duk_pop(ctx);
	return 0;
}
static void duv_dump_error(duk_context *ctx, duk_idx_t idx) {
	fprintf(stderr, "\nUncaught Exception:\n");
	if (duk_is_object(ctx, idx)) {
		duk_get_prop_string(ctx, -1, "stack");
		fprintf(stderr, "\n%s\n\n", duk_get_string(ctx, -1));
		duk_pop(ctx);
	}
	else {
		fprintf(stderr, "\nThrown Value: %s\n\n", duk_json_encode(ctx, idx));
	}
}

void asio_run() {
	if (duv_thread_env.alive) uv_run(&loop, UV_RUN_NOWAIT);
}
void asio_bind_js(duk_context *J) {
	duv_bind_js(J);
	duk_put_global_string(J, "asio");
}
void asio_set_binder(duk_ret_t (*bind)(duk_context *)) {
	duv_thread_env.bind = bind;
}
char asio_start(duk_context *J, int argc, void *argv) {
	if (XATAA) printf("asio_start %d\n", duv_thread_env.alive);
	uv_loop_init(&loop);
	uv_setup_args(argc, argv);
	loop.data = J;
	
	duv_thread_env.alive = 1;
	
	// Stash argv for later access
	duk_push_pointer(J, (void *) argv);
	duk_push_int(J, argc);
	if (duk_safe_call(J, duv_stash_argv, NULL, 2, 1) != 0) {
		duv_dump_error(J, -1);
		uv_loop_close(&loop);
		duv_thread_env.alive = 0;
		duk_destroy_heap(J);
		exit(EXIT_FAILURE);
	}
	duk_pop(J);
	
	duv_ref_setup(J);
	return 1;
}
void asio_destroy() {
	if (XATAA) printf("asio_destroy %d\n", duv_thread_env.alive);
	if (duv_thread_env.alive) {
		// TODO uv_cancel all reqs in refs stash -_-
		duv_thread_env.alive = 0;
		
		duv_stop_all_workers(J);
		
		int ret = uv_run(&loop, UV_RUN_DEFAULT);
		if (ret < 0) duv_error(J, ret);

		uv_loop_close(&loop); // call after all reqs are done
		
		if (XATAA) printf("asio destroyed\n");
	}
}

