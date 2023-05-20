#include "codec.h"
#include "sgl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//{ codec
bindjs (codec_decode_image) {
	int n = duk_get_top(J);
	if (n && duk_is_buffer_data(J, 0)) {
		int x, y, n;
		u_char *data = NULL;
		duk_size_t size; // for auto sizing
		data = duk_get_buffer_data(J, 0, &size);
		data = stbi_load_from_memory(data, size, &x, &y, &n, 0);
		if (data) {
			long len = x*y*n;
			void *p = duk_push_fixed_buffer(J, len);
			if (p) {
				duk_push_object(J); // -1

				memcpy(p, data, len);
				duk_push_buffer_object(J, -2, 0, len, DUK_BUFOBJ_UINT8ARRAY);
				duk_put_prop_string(J, -2, "data");

				dukjs_prop_num_x(J, "w", x);
				dukjs_prop_num_x(J, "h", y);
				dukjs_prop_num_x(J, "n", n);
			}
			stbi_image_free(data);
		} else {
			duk_push_error_object(J, DUK_ERR_ERROR, "%s", stbi_failure_reason());
		}
		return 1;
	}
	return 0;
}
bindjs (codec_write_jpg) {
	int n = duk_get_top(J);
	if (n > 5) {
		const char *filename = duk_get_string(J, 0);
		int w = duk_get_number(J, 1);
		int h = duk_get_number(J, 2);
		int comp = duk_get_number(J, 3);
		int quality = duk_get_number(J, 5);

		void *data;
		if (duk_is_buffer_data(J, 4)) {
			data = duk_get_buffer_data(J, 4, NULL);
		} else {
			data = sgl_pixels_to_pointer( 0, 0, w, h, 0, 0, NULL, &comp, NULL );
		}

		int result = stbi_write_jpg(filename, w, h, comp, data, quality);
		duk_push_number(J, result);

		if ( !duk_is_buffer_data(J, 4) && data ) free( data );
		return 1;
	}
	return 0;
}
//}

void codec_bind_js(duk_context *J) {
	duk_push_object(J); {
		push_c_method_x(J, "decode_image", codec_decode_image);
		push_c_method_x(J, "write_jpg", codec_write_jpg);
	}
	duk_put_global_string(J, "codec");
}
char codec_start(duk_context *J, int argc, void *argv) {
	stbi_flip_vertically_on_write(1);
}
void codec_destroy() {
	stbi_flip_vertically_on_write(0);
}

