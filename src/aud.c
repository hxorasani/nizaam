#include "aud.h"

#define XATAA 0
static ma_engine engine;

bindjs (aud_play) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		const char *str = duk_to_string(J, 0);
		ma_engine_play_sound(&engine, str, NULL);
	}
	return 0;
}

void aud_bind_js(duk_context *J) {
	duk_push_object(J); {
		push_c_method_x(J, "play", aud_play);
	}
	duk_put_global_string(J, "aud");
}

void aud_start() {
	ma_result result;

	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		perror("failed to initialize the engine"); return;
	}
}
void aud_destroy() {
	ma_engine_uninit(&engine);
}