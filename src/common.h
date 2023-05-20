#ifndef _common_h
#define _common_h

#include "duktape.h"
#include "malaf.h"
//#include "nuk-canvas.h"

/* this file should be included in any module that depends on duktape
 * it contains the main duk context *J and all the macros for fast bridging
 * */

#define bindjs(n) duk_ret_t n (duk_context *J)
#define bindjspropx(c, n) dukjs_prop_num_x(x, #n, n);
#define bindjsprop(n) dukjs_prop_num(#n, n);
duk_context *J;
char canvas_open, window_open;

//bindjs(nuk_end_canvas);
//bindjs(nuk_end);

void print_duk_value(duk_context *ctx, duk_idx_t i);
void print_duk_stack(duk_context *ctx);
void dukjs_prop_num_obj(duk_idx_t obj, char *prop, double num);
void dukjs_prop_buf(duk_context *, duk_idx_t, char *, void *, long, duk_uint_t);
void dukjs_prop_num_x(duk_context *ctx, char *prop, double num);
void dukjs_prop_num(char *prop, double num);
void load_js_file_x(duk_context *ctx, char *path);
void load_js_file(char *path);
void push_c_function_x(duk_context *ctx, char *name, void *function);
void push_c_method_x(duk_context *ctx, char *name, void *function);
void push_c_function(char *name, void *function);
void push_c_method(char *name, void *function);
int if_js_function(char *name);
int call_js_function_with_stack(char *name, int args);

#endif