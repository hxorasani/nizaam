#include "common.h"
#define XATAA 0

void print_duk_value(duk_context *ctx, duk_idx_t i) {
	char str[48] = { 0 };
	switch (duk_get_type(ctx, i)) {
		case DUK_TYPE_NONE       :
			printf("none\n"); break;
		case DUK_TYPE_UNDEFINED  :
			printf("undefined\n"); break;
		case DUK_TYPE_NULL       :
			printf("null\n"); break;
		case DUK_TYPE_BOOLEAN    :
			printf("bool %d\n", duk_get_boolean(ctx, i)); break;
		case DUK_TYPE_NUMBER     :
			printf("number %lf\n", duk_get_number(ctx, i)); break;
		case DUK_TYPE_STRING     :
			strncpy(str, duk_get_string(ctx, i), 48);
			printf("string %s\n", str);
			break;
		case DUK_TYPE_OBJECT     :
			if (duk_is_array(ctx, i))
				printf("array object\n"); 
			else if (duk_is_buffer_data(ctx, i))
				printf("buffer object\n");
			else if (duk_is_function(ctx, i))
				printf("function object\n");
			else
				printf("object\n");
			break;
		case DUK_TYPE_BUFFER     :
			printf("buffer\n"); break;
		case DUK_TYPE_POINTER    :
			printf("pointer\n"); break;
		case DUK_TYPE_LIGHTFUNC  :
			printf("lightfunc\n"); break;
	}
}
void print_duk_stack(duk_context *ctx) {
	duk_int_t n = duk_get_top(ctx);
	printf("duk stack size %d\n", n);
	for (int i = 0; i < n; ++i) {
		printf("%d : ", i);
		print_duk_value(ctx, i);
	}
}
void dukjs_prop_num_obj(duk_idx_t obj, char *prop, double num) {
	duk_push_number(J, num); duk_put_prop_string(J, obj, prop);
}
void dukjs_prop_buf(duk_context *ctx, duk_idx_t obj, char *prop, void *data, long len, duk_uint_t type) { // { prop: buf }
	void *p = duk_push_fixed_buffer(ctx, len);
	if (p) {
		memcpy(p, data, len);
		duk_push_buffer_object(ctx, -1, 0, len, type);
		duk_put_prop_string(ctx, obj, prop);
		duk_pop(ctx);
	}
}
void dukjs_prop_num_x(duk_context *ctx, char *prop, double num) { // { prop: num }
	// -2 because the number is -1, expects object at -2
	duk_push_number(ctx, num);
	duk_put_prop_string(ctx, -2, prop);
}
void dukjs_prop_num(char *prop, double num) { // { prop: num }
	dukjs_prop_num_x(J, prop, num);
}
void load_js_file_x(duk_context *J, char *path) {
	/* Use duk_safe_call() to wrap all unsafe code into a separate C function.
	* This approach has the advantage of covering all API calls automatically
	* but is a bit more verbose.
	*/
	
	duk_ret_t unsafe_code(duk_context *J, void *udata) {
		// Here we can use unprotected calls freely
	
		(void) udata;
		malaf mlf;
		if ( malaf_init(&mlf, path) == 1 ) {
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
void load_js_file(char *path) {
	load_js_file_x(J, path);
}
void push_c_function_x(duk_context *ctx, char *name, void *function) {
	duk_push_c_function(ctx, function, DUK_VARARGS);
	duk_put_global_string(ctx, name);
}
void push_c_method_x(duk_context *ctx, char *name, void *function) {
	duk_push_c_function(ctx, function, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, name); // AND what about this -2
}
void push_c_function(char *name, void *function) {
	push_c_function_x(J, name, function);
}
void push_c_method(char *name, void *function) {
	push_c_method_x(J, name, function);
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
	if (XATAA > 2) print_duk_stack(J);
	duk_int_t rc = duk_pcall(J, args);  /* [ ... func 2 3 ] -> [ 5 ] */
	int retval = 0;
	if (rc == DUK_EXEC_SUCCESS) {
		retval = duk_get_int(J, -1);
		duk_pop(J);
	} else {
//		if (canvas_open) nuk_end_canvas(J);
//		if (window_open) nuk_end(J);
		
		if (duk_is_error(J, -1)) duk_safe_to_stacktrace(J, -1);
		printf("error: %s\n", duk_safe_to_string(J, -1));
		duk_pop(J);
	}
	return retval;
}


