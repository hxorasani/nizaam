#ifndef _asio_h
#define _asio_h

#include "uv.h"
#include "duktape.h"
#include "duv/duv.h"
#include "common.h"

void asio_bind_js(duk_context *);

uv_loop_t *asio_loop();
void asio_run();
char asio_start(duk_context *, int argc, void *argv);
void asio_set_binder(duk_ret_t (*bind)(duk_context *));
void asio_destroy();

#endif