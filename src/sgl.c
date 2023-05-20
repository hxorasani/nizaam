#include "sgl.h"

#define enum_push(n) if (s == n) duk_push_string(J, #n);

static char sgl_ready = 0;

static GLuint get_array_buffer(void *arr, GLsizeiptr size) {
	GLuint uid;
	glGenBuffers(1, &uid);
	glBindBuffer(GL_ARRAY_BUFFER, uid);
	glBufferData(GL_ARRAY_BUFFER, size, arr, GL_STATIC_DRAW);
	return uid;
}
static GLuint get_vertex_array() {
	GLuint uid;
    glGenVertexArrays(1, &uid);
    glBindVertexArray(uid);
	return uid;
}
static GLuint get_element_buffer(void *arr, GLsizeiptr size) {
	GLuint uid;
	glGenBuffers(1, &uid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, arr, GL_STATIC_DRAW);
	return uid;
}
static GLuint get_ssbo(void *arr, GLsizeiptr size) {
    GLuint uid;
    glGenBuffers(1, &uid);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, uid );
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, arr, GL_DYNAMIC_COPY);
    return uid;
}
static void sgl_draw_elements_by_type(GLuint elements, int type) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);
	GLint size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	int width = sizeof(GLuint);
	if (type == GL_UNSIGNED_SHORT) width = sizeof(GLushort);
	glDrawElements(GL_TRIANGLES, size/width, type, 0);
}
void * sgl_pixels_to_pointer(float x, float y, float w, float h, GLenum format,
					GLenum type, int *out_len, int *out_comps, int *out_bpc) {
	if (format == 0) format = GL_RGB;
	if (type == 0) type = GL_UNSIGNED_BYTE;
	
	int comps = 3, bpc = 1; // bytes per comp
	if (format == GL_RED) comps = 1;
	if (format == GL_RG) comps = 2;
	if (format == GL_RGBA) comps = 4;

	if (type == GL_FLOAT) bpc = 4;
	if (type == GL_UNSIGNED_SHORT) bpc = 2;
	if (type == GL_UNSIGNED_INT) bpc = 4;

	void *data = NULL;
	int len = (w-x) * (h-y) * comps * bpc;

	if (out_len != NULL) *out_len = len;
	if (out_comps != NULL) *out_comps = comps;
	if (out_bpc != NULL) *out_bpc = bpc;
	
	data = malloc( len );

	glReadPixels(x, y, w, h, format, type, data);

	return data;
}

bindjs (sgl_get_string) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		const u_char *str = glGetString(duk_get_number(J, 0));
		duk_push_string(J, str);
		return 1;
	}
	return 0;
}
bindjs (sgl_draw_buffer) {
	duk_idx_t n = duk_get_top(J);
	if (n) glDrawBuffer( duk_get_number(J, 0) );
	return 0;
}
bindjs (sgl_read_buffer) {
	duk_idx_t n = duk_get_top(J);
	if (n) glReadBuffer( duk_get_number(J, 0) );
	return 0;
}
bindjs (sgl_viewport) {
	duk_idx_t n = duk_get_top(J);
	if (n > 3) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		float w = duk_get_number(J, 2);
		float h = duk_get_number(J, 3);
		glViewport(x, y, w, h);
	}
	return 0;
}
bindjs (sgl_clear_color) {
	duk_idx_t n = duk_get_top(J);
	if (n > 3) {
		float r = duk_get_number(J, 0);
		float g = duk_get_number(J, 1);
		float b = duk_get_number(J, 2);
		float a = duk_get_number(J, 3);
		glClearColor(r, g, b, a);
	}
	return 0;
}
bindjs (sgl_get_uniform) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		int load_ok = 1;
		GLint i = shader_bind_uniform(duk_get_string(J, 0), duk_get_number(J, 1), &load_ok);
		if (load_ok) {
			duk_push_number(J, i);
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_get_attribute) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		int load_ok = 1;
		GLint i = shader_bind_attribute(duk_get_string(J, 0), duk_get_number(J, 1), &load_ok);
		if (load_ok) {
			duk_push_number(J, i);
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_enable_array_buffer) {
	duk_idx_t n = duk_get_top(J);
	if (n > 4) {
		GLint	attr  = duk_get_number(J, 0);
		GLuint	buf   = duk_get_number(J, 1);
		u_int	num   = duk_get_number(J, 2);
		u_int	skip  = duk_get_number(J, 3);
		u_int	offset= duk_get_number(J, 4);
		
		long	size  = n > 5 ? duk_get_number(J, 5) : sizeof(GLfloat);
		GLenum	type  = n > 6 ? duk_get_number(J, 6) : GL_FLOAT;
		/* for combined buffers that contain [ushorts, floats, ..., ushorts, ..]
		 * specify size = 1, that way you can give skip * custom_size
		 * */

		glEnableVertexAttribArray(attr);
		glBindBuffer(GL_ARRAY_BUFFER, buf); // verts + colors
		glVertexAttribPointer(
			attr,					// attribute
			num,					// of elements per vertex, here (x,y,z)
			type,					// the type of each element
			GL_FALSE,				// take our values as-is
			skip * size,			// next coord appears every skip types
			(GLvoid*)(offset * size)// of the first element
		);
	}
	return 0;
}
bindjs (sgl_attrib_pointer) { // attr buf num skip offset [type]
	duk_idx_t n = duk_get_top(J);
	if (n > 4) {
		GLint	attr  = duk_get_number(J, 0);
		GLuint	buf   = duk_get_number(J, 1);
		u_int	num   = duk_get_number(J, 2);
		u_int	skip  = duk_get_number(J, 3);
		long	offset= duk_get_number(J, 4);
		GLenum	type  = n > 5 ? duk_get_number(J, 5) : GL_FLOAT;

		glEnableVertexAttribArray(attr);
		glBindBuffer(GL_ARRAY_BUFFER, buf); // verts + colors
		glVertexAttribPointer(
			attr,					// attribute
			num,					// of elements per vertex, here (x,y,z)
			type,					// the type of each element
			GL_FALSE,				// take our values as-is
			skip,					// next coord appears every skip types
			(GLvoid*)(offset)		// of the first element
		);
	}
	return 0;
}
bindjs (sgl_disable_array_buffer) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		glDisableVertexAttribArray( duk_get_number(J, 0) );
	}
	return 0;
}
bindjs (sgl_draw_arrays) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		GLenum mode = duk_get_number(J, 0);
		// count is num of indices like arr buf {x,y,z}, {x,y,z} will be 6 / 3 = 2
		glDrawArrays(mode, 0, duk_get_number(J, 1));
	}
	return 0;
}
bindjs (sgl_draw_range) {
	duk_idx_t n = duk_get_top(J);
	if (n > 2) {
		GLenum mode  = duk_get_number(J, 0);
		GLint  start = duk_get_number(J, 1);
		GLint  end   = duk_get_number(J, 2);
		glDrawArrays(mode, start, end);
	}
	return 0;
}
bindjs (sgl_draw_elements) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		GLenum mode = n > 0 ? duk_get_number(J, 0) : GL_TRIANGLES;
		GLint  size = n > 1 ? duk_get_number(J, 1) : 0;
		GLenum type = n > 2 ? duk_get_number(J, 2) : GL_UNSIGNED_SHORT;

		// this is a byte offset into the gpu size elements array
		long start = n > 3 ? duk_get_number(J, 3) : 0;
		if (type == 0) type = GL_UNSIGNED_SHORT;
		if (type == GL_FLOAT) start *= sizeof(GLfloat);
		if (type == GL_UNSIGNED_SHORT) start *= sizeof(GLushort);

		glDrawElements(mode, size, type, (GLvoid*)start);
	}
	return 0;
}
bindjs (sgl_vertex_array) {
	duk_push_number(J, get_vertex_array());
	return 1;
}
bindjs (sgl_array_buffer) {
	duk_idx_t n = duk_get_top(J);
	if (n && duk_is_buffer_data(J, 0)) {
		duk_size_t size;
		GLfloat *arr = duk_get_buffer_data(J, 0, &size);
		if (arr) {
			duk_push_number(J, get_array_buffer(arr, size));
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_array_sub_buffer) {
	duk_idx_t n = duk_get_top(J);
	if (n > 2 && duk_is_buffer_data(J, 2)) {
		duk_size_t size;
		GLuint uid = duk_get_number(J, 0);
		GLintptr offset = duk_get_number(J, 1);
		void *arr = duk_get_buffer_data(J, 2, &size);
		long bytesize = n > 3 ? duk_get_number(J, 3) : sizeof(GLfloat);
		if (arr) {
			glNamedBufferSubData(uid, offset*bytesize, size, arr);
		}
	}
	return 0;
}
bindjs (sgl_element_buffer) {
	duk_idx_t n = duk_get_top(J);
	if (n && duk_is_buffer_data(J, 0)) {
		duk_size_t size;
		GLushort *arr = duk_get_buffer_data(J, 0, &size);
		if (arr) {
			duk_push_number(J, get_element_buffer(arr, size));
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_ssbo) {
	duk_idx_t n = duk_get_top(J);
	if (n && duk_is_buffer_data(J, 0)) {
		duk_size_t size;
		GLfloat *arr = duk_get_buffer_data(J, 0, &size);
		if (arr) {
			duk_push_number(J, get_ssbo(arr, size));
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_update_ssbo) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1 && duk_is_buffer_data(J, 0)) {
		duk_size_t size;
		GLfloat *arr = duk_get_buffer_data(J, 0, &size);
		GLuint ssbo = duk_get_number(J, 1);
		if (arr) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
			memcpy(p, &arr, size);
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_bind_ssbo_base) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		GLuint ssbo = duk_get_number(J, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	}
	return 0;
}
bindjs (sgl_active_texture) {
	duk_idx_t n = duk_get_top(J);
	if (n) glActiveTexture(duk_get_number(J, 0));
	return 0;
}
bindjs (sgl_texture) {
	duk_idx_t n = duk_get_top(J);
	if (n > 5) {
		GLuint uid = duk_get_number(J, 0);
		GLenum target = duk_get_number(J, 1);
		GLint level = duk_get_number(J, 2); // samples in case MSAA
		GLint internal_format = duk_get_number(J, 3);
		GLsizei w = duk_get_number(J, 4);
		GLsizei h = duk_get_number(J, 5);
		// only if uid is not a num, gens a new texture
		if (duk_get_type(J, 0) != DUK_TYPE_NUMBER) glGenTextures(1, &uid);
		if (target == GL_TEXTURE_2D_MULTISAMPLE) {
			glBindTexture(target, uid);
			glTexImage2DMultisample(target, level, internal_format, w, h, GL_TRUE);
			glBindTexture(target, 0);
		} else if (n > 7) {
			GLenum format = duk_get_number(J, 6); // rgb ..
			GLenum type = duk_get_number(J, 7); // u_char ..
			GLushort *arr = NULL;
			if (duk_is_buffer_data(J, 8)) {
				duk_size_t size; // for auto sizing
				arr = duk_get_buffer_data(J, 8, &size);
//				if (w == -1 && h == -1) { w = size; h = 1; }
			}
			glBindTexture(target, uid);
			glTexImage2D(target, level, internal_format, w, h, 0, format, type, arr);

//			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
//			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR

//			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glBindTexture(target, 0);
		}
		duk_push_number(J, uid);
		return 1;
	}
	return 0;
}
bindjs (sgl_generate_texture_mipmap) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		GLuint uid = duk_get_number(J, 0);
		glGenerateTextureMipmap(uid);
	}
	return 0;
}
bindjs (sgl_renderbuffer) {
	duk_idx_t n = duk_get_top(J);
	if (n > 4) {
		GLuint uid = duk_get_number(J, 0);
		GLint level = duk_get_number(J, 1); // samples in case MSAA
		GLint internal_format = duk_get_number(J, 2);
		GLsizei w = duk_get_number(J, 3);
		GLsizei h = duk_get_number(J, 4);
		// only if uid is not a num, gens a new texture
		if (duk_get_type(J, 0) != DUK_TYPE_NUMBER) glGenRenderbuffers(1, &uid);
		glBindRenderbuffer(GL_RENDERBUFFER, uid);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, level, internal_format, w, h);
		duk_push_number(J, uid);
		return 1;
	}
	return 0;
}
bindjs (sgl_bind) {
	GLenum target = duk_get_number(J, 0);
	GLuint uid = duk_get_number(J, 1);
	if (target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER) {
		glBindBuffer(target, uid);
	} else if (target == GL_FRAMEBUFFER) {
		glBindFramebuffer(target, uid);
	} else if (target == GL_RENDERBUFFER) {
		glBindRenderbuffer(target, uid);
	} else if (target == GL_TEXTURE_2D || target == GL_TEXTURE_2D_MULTISAMPLE) {
		glBindTexture(target, uid);
	} else if (target == GL_VERTEX_ARRAY) {
		glBindVertexArray(uid);
	}
	return 0;
}
bindjs (sgl_delete_buffers) {
	duk_idx_t n = duk_get_top(J);
	if (n && duk_is_buffer_data(J, 0)) {
		duk_size_t size;
		GLuint *arr = duk_get_buffer_data(J, 0, &size);
		if (arr) {
			glDeleteBuffers(size/sizeof(GLuint), arr);
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_delete_vertex_arrays) {
	duk_idx_t n = duk_get_top(J);
	if (n && duk_is_buffer_data(J, 0)) {
		duk_size_t size;
		GLuint *arr = duk_get_buffer_data(J, 0, &size);
		if (arr) {
			glDeleteVertexArrays(size/sizeof(GLuint), arr);
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_delete_textures) {
	duk_idx_t n = duk_get_top(J);
	if (n && duk_is_buffer_data(J, 0)) {
		duk_size_t size;
		GLuint *arr = duk_get_buffer_data(J, 0, &size);
		if (arr) {
			glDeleteTextures(size/sizeof(GLuint), arr);
			return 1;
		}
	}
	return 0;
}
bindjs (sgl_cull_face) {
	duk_idx_t n = duk_get_top(J);
	if (n) glCullFace(duk_get_number(J, 0));
	return 0;
}
bindjs (sgl_front_face) {
	duk_idx_t n = duk_get_top(J);
	if (n) glFrontFace(duk_get_number(J, 0));
	return 0;
}
bindjs (sgl_enable) {
	duk_idx_t n = duk_get_top(J);
	if (n) glEnable(duk_get_number(J, 0));
	return 0;
}
bindjs (sgl_disable) {
	duk_idx_t n = duk_get_top(J);
	if (n) glDisable(duk_get_number(J, 0));
	return 0;
}
bindjs (sgl_clear) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		glClear(duk_get_number(J, 0));
	}
	return 0;
}
bindjs (sgl_clear_stencil) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		glClearStencil(duk_get_number(J, 0));
	}
	return 0;
}
bindjs (sgl_stencil_op) {
	duk_idx_t n = duk_get_top(J);
	if (n > 2) {
		glStencilOp(duk_get_number(J, 0), duk_get_number(J, 1), duk_get_number(J, 2));
	}
	return 0;
}
bindjs (sgl_stencil_func) {
	duk_idx_t n = duk_get_top(J);
	if (n > 2) {
		glStencilFunc(duk_get_number(J, 0), duk_get_number(J, 1), duk_get_number(J, 2));
	}
	return 0;
}
bindjs (sgl_stencil_index) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		GLuint index = 0;
		glReadPixels(x, WJHH.raees.mtrx.h - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
		duk_push_number(J, index);
		return 1;
	}
	return 0;
}
bindjs (sgl_depth) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		GLfloat depth;
		glReadPixels(x, WJHH.raees.mtrx.h - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		duk_push_number(J, depth);
		return 1;
	}
	return 0;
}
bindjs (sgl_pixel) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		float x = duk_get_number(J, 0);
		float y = duk_get_number(J, 1);
		GLenum format = n > 2 ? duk_get_number(J, 2) : GL_RGB; // rgb ..
		GLenum type   = n > 3 ? duk_get_number(J, 3) : GL_FLOAT; // u_char ..

		duk_push_object(J);

		void *pixel;
		GLfloat pixelf[3];
		GLuint pixelui[3];
		if (type == GL_FLOAT) {
			glReadPixels(x, WJHH.raees.mtrx.h - y - 1, 1, 1, format, type, &pixelf);
			dukjs_prop_num("r", pixelf[0]);
			dukjs_prop_num("g", pixelf[1]);
			dukjs_prop_num("b", pixelf[2]);
		}
		if (type == GL_UNSIGNED_INT) {
			glReadPixels(x, WJHH.raees.mtrx.h - y - 1, 1, 1, format, type, &pixelui);
			dukjs_prop_num("r", pixelui[0]);
			dukjs_prop_num("g", pixelui[1]);
			dukjs_prop_num("b", pixelui[2]);
		}
		return 1;
	}
	return 0;
}
bindjs (sgl_pixels) {
	duk_idx_t n = duk_get_top(J);
	if (n > 3) {
		float x = duk_get_number(J, 0),
			  y = duk_get_number(J, 1),
			  w = duk_get_number(J, 2),
			  h = duk_get_number(J, 3);

		GLenum format = GL_RGB, type = GL_UNSIGNED_BYTE;
		if (n > 4) format = duk_get_number(J, 4); // rgb ..
		if (n > 5) type   = duk_get_number(J, 5); // u_char ..

		int len, comps, bpc;
		void *data = sgl_pixels_to_pointer(x, y, w, h, format, type, &len, &comps, &bpc);
		void *p = duk_push_fixed_buffer(J, len);
		if (p) {
			duk_push_object(J); // -1

			memcpy(p, data, len);
			duk_push_buffer_object(J, -2, 0, len, DUK_BUFOBJ_UINT8ARRAY);
			duk_put_prop_string(J, -2, "data");

			dukjs_prop_num_x(J, "w", w-x  );
			dukjs_prop_num_x(J, "h", h-y  );
			dukjs_prop_num_x(J, "n", comps);
			dukjs_prop_num_x(J, "b", bpc  );
		}
		
		if (data) free( data );
		return 1;
	}
	return 0;
}
bindjs (sgl_uniform_1ui) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		GLint uniform = duk_get_number(J, 0);
		GLuint val = duk_get_number(J, 1);
		glUniform1ui( uniform, val );
	}
	return 0;
}
bindjs (sgl_uniform_1i) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		GLint uniform = duk_get_number(J, 0);
		GLint val = duk_get_number(J, 1);
		glUniform1i( uniform, val );
	}
	return 0;
}
bindjs (sgl_uniform_1f) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		GLint uniform = duk_get_number(J, 0);
		GLfloat val = duk_get_number(J, 1);
		glUniform1f( uniform, val );
	}
	return 0;
}
bindjs (sgl_uniform_2f) {
	duk_idx_t n = duk_get_top(J);
	if (n > 2) {
		GLint uniform = duk_get_number(J, 0);
		GLfloat x = duk_get_number(J, 1);
		GLfloat y = duk_get_number(J, 2);
		glUniform2f( uniform, x, y );
	}
	return 0;
}
bindjs (sgl_uniform_3f) {
	duk_idx_t n = duk_get_top(J);
	if (n > 3) {
		GLint uniform = duk_get_number(J, 0);
		GLfloat r = duk_get_number(J, 1);
		GLfloat g = duk_get_number(J, 2);
		GLfloat b = duk_get_number(J, 3);
		glUniform3f( uniform, r, g, b );
	}
	return 0;
}
bindjs (sgl_uniform_3fv) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1 && duk_is_buffer_data(J, 1)) {
		GLfloat *arr = duk_get_buffer_data(J, 1, NULL);
		if (arr) {
			glUniform3fv( duk_get_number(J, 0), 1, arr);
		}
	}
	return 0;
}
bindjs (sgl_uniform_4fv) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1 && duk_is_buffer_data(J, 1)) {
		GLfloat *arr = duk_get_buffer_data(J, 1, NULL);
		if (arr) {
			glUniform4fv( duk_get_number(J, 0), 1, arr);
		}
	}
	return 0;
}
bindjs (sgl_uniform_4f) {
	duk_idx_t n = duk_get_top(J);
	if (n > 4) {
		GLint uniform = duk_get_number(J, 0);
		GLfloat r = duk_get_number(J, 1);
		GLfloat g = duk_get_number(J, 2);
		GLfloat b = duk_get_number(J, 3);
		GLfloat a = duk_get_number(J, 4);
		glUniform4f( uniform, r, g, b, a );
	}
	return 0;
}
bindjs (sgl_uniform_matrix_3fv) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1 && duk_is_buffer_data(J, 1)) {
		GLfloat *mat = duk_get_buffer_data(J, 1, NULL);
		if (mat) {
			glUniformMatrix3fv( duk_get_number(J, 0), 1, GL_FALSE, mat);
		}
	}
	return 0;
}
bindjs (sgl_uniform_matrix_4fv) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1 && duk_is_buffer_data(J, 1)) {
		GLfloat *mat = duk_get_buffer_data(J, 1, NULL);
		if (mat) {
			glUniformMatrix4fv( duk_get_number(J, 0), 1, GL_FALSE, mat);
		}
	}
	return 0;
}
bindjs (sgl_create_program) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		const char * vpath = duk_to_string(J, 0);
		const char * fpath = duk_to_string(J, 1);
		
		GLuint prog = glCreateProgram();
		
		GLint compile_ok = GL_FALSE, link_ok = GL_FALSE, validate_ok = GL_FALSE;
		GLuint vs, fs;

		if (n > 2 && duk_get_number(J, 2)) { // init using strings
			if ((vs = create_shader_text("vertex shader", vpath, GL_VERTEX_SHADER  )) == 0) return 0;
			if ((fs = create_shader_text("fragment shader", fpath, GL_FRAGMENT_SHADER)) == 0) return 0;
		} else {
			if ((vs = create_shader(vpath, GL_VERTEX_SHADER  )) == 0) return 0;
			if ((fs = create_shader(fpath, GL_FRAGMENT_SHADER)) == 0) return 0;
		}

		glAttachShader	( prog, vs );
		glAttachShader	( prog, fs );

		glLinkProgram	( prog );
		glGetProgramiv	( prog, GL_LINK_STATUS, &link_ok );

		if (!link_ok) { printf("error in glLinkProgram\n"); print_log( prog ); return 0; }

		glValidateProgram( prog );
		glGetProgramiv( prog, GL_VALIDATE_STATUS, &validate_ok );
		if (!validate_ok) { fprintf(stderr, "glValidateProgram:"); print_log( prog ); return 0; }

		duk_push_number(J, prog);
		return 1;
	}
	return 0;
}
bindjs (sgl_use_program) {
	duk_idx_t n = duk_get_top(J);
	if (n) {
		GLuint prog = duk_get_number(J, 0);
		glUseProgram(prog);
	}
	return 0;
}
bindjs (sgl_delete_program) {
	duk_idx_t n = duk_get_top(J);
	if (n) glDeleteProgram(duk_get_number(J, 0));
	return 0;
}
bindjs (sgl_current_framebuffer) {
	duk_idx_t n = duk_get_top(J);
	GLuint uid;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &uid);
	duk_push_number(J, uid);
	return 1;
}
bindjs (sgl_create_framebuffer) {
	duk_idx_t n = duk_get_top(J);
	GLuint uid;
	glGenFramebuffers(1, &uid);
	duk_push_number(J, uid);
	return 1;
}
bindjs (sgl_delete_framebuffer) {
	duk_idx_t n = duk_get_top(J);
	if (n && duk_is_buffer_data(J, 0)) {
		duk_size_t size;
		GLuint *arr = duk_get_buffer_data(J, 0, &size);
		if (arr) {
			glDeleteFramebuffers(size/sizeof(GLuint), arr);
		}
	}
	return 0;
}
bindjs (sgl_attach_framebuffer) {
	duk_idx_t n = duk_get_top(J);
	if (n > 2) {
		GLenum target = duk_get_number(J, 0);
		GLenum attachment = duk_get_number(J, 1);
		GLuint uid = duk_get_number(J, 2);
		if (target == GL_TEXTURE_2D_MULTISAMPLE || target == GL_TEXTURE_2D) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, uid, 0);
		}
		if (target == GL_RENDERBUFFER) {
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, target, uid);
		}
	}
	return 0;
}
bindjs (sgl_check_framebuffer) {
	GLenum s = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (s == GL_FRAMEBUFFER_COMPLETE ) duk_push_number(J, 1);
	enum_push(GL_FRAMEBUFFER_UNDEFINED);
	enum_push(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
	enum_push(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
	enum_push(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
	enum_push(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
	enum_push(GL_FRAMEBUFFER_UNSUPPORTED);
	enum_push(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
	enum_push(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);
	return 1;
}
bindjs (sgl_blit_framebuffer) {
	duk_idx_t n = duk_get_top(J);
	if (n > 3) {
		GLuint fbr = duk_get_number(J, 0); // read
		GLuint fbw = duk_get_number(J, 1); // write
		GLsizei w = duk_get_number(J, 2);
		GLsizei h = duk_get_number(J, 3);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbr);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbw);
		glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	return 0;
}
bindjs (sgl_polygon_mode) {
	duk_idx_t n = duk_get_top(J);
	if (n > 1) {
		GLenum a = duk_get_number(J, 0);
		GLenum b = duk_get_number(J, 1);
		glPolygonMode(a, b);
	}
	return 0;
}
bindjs (sgl_get_error) {
	GLenum errCode;
	const GLubyte *errString = "";
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
	}
	duk_push_string(J, errString);
	return 1;
}

void sgl_bind_js(duk_context *J) {
	duk_push_object(J); {
		// TODO transition to bindjspropx(J, ...)
		#define FOREACH(_) \
		_(GL_STENCIL_BUFFER_BIT) _(GL_STENCIL_TEST) _(GL_LINE_STRIP) _(GL_POINTS) \
		_(GL_DEPTH_BUFFER_BIT) _(GL_COLOR_BUFFER_BIT) _(GL_LINES) _(GL_LINE_LOOP) \
		_(GL_TRIANGLE_FAN) _(GL_TRIANGLE_STRIP) _(GL_LINE_STRIP_ADJACENCY) _(GL_LINES_ADJACENCY) \
		_(GL_TRIANGLES) _(GL_TRIANGLE_STRIP_ADJACENCY) _(GL_TRIANGLES_ADJACENCY) _(GL_PATCHES) \
		_(GL_KEEP) _(GL_ALWAYS) _(GL_NEVER) _(GL_REPLACE) _(GL_EQUAL) _(GL_UNSIGNED_INT) \
		_(GL_FLOAT) _(GL_UNSIGNED_SHORT) _(GL_DEPTH_COMPONENT) _(GL_RGB32F) \
		_(GL_CULL_FACE) _(GL_DEPTH_TEST) _(GL_BLEND) _(GL_SCISSOR_TEST) \
		_(GL_R32F) _(GL_RG32UI) _(GL_RG16UI) _(GL_R32UI) _(GL_RGB16UI) \
		_(GL_RED) _(GL_RED_INTEGER) _(GL_RG) _(GL_RGB) _(GL_RGBA) \
		_(GL_TEXTURE_2D) _(GL_TEXTURE_2D_MULTISAMPLE) _(GL_UNSIGNED_BYTE) \
		_(GL_COLOR_ATTACHMENT0) _(GL_DEPTH24_STENCIL8) _(GL_DEPTH_STENCIL_ATTACHMENT) \
		_(GL_RENDERBUFFER) _(GL_ARRAY_BUFFER) _(GL_ELEMENT_ARRAY_BUFFER) _(GL_FRAMEBUFFER) \
		_(GL_VERTEX_ARRAY) _(GL_TEXTURE0) _(GL_DEPTH_STENCIL) _(GL_NONE) \
		_(GL_FILL) _(GL_LINE) _(GL_POINT) _(GL_RENDERER) _(GL_VERSION) \
		_(GL_CW) _(GL_CCW) _(GL_FRONT_AND_BACK) _(GL_BACK) _(GL_FRONT)
		FOREACH(bindjsprop)
		#undef FOREACH
		
//		#define FOREACH(_) \
//		#define GENERATE_STRING(STRING) #STRING,
//		#undef FOREACH

		push_c_method_x(J, "get_string", sgl_get_string);

		push_c_method_x(J, "polygon_mode", sgl_polygon_mode);

		push_c_method_x(J, "enable_array_buffer", sgl_enable_array_buffer);
		push_c_method_x(J, "disable_array_buffer", sgl_disable_array_buffer);
		push_c_method_x(J, "draw_arrays", sgl_draw_arrays);
		push_c_method_x(J, "draw_range", sgl_draw_range);
		push_c_method_x(J, "draw_elements", sgl_draw_elements);
		push_c_method_x(J, "draw_buffer", sgl_draw_buffer);
		push_c_method_x(J, "read_buffer", sgl_read_buffer);
		push_c_method_x(J, "get_error", sgl_get_error);

		push_c_method_x(J, "bind", sgl_bind);
		push_c_method_x(J, "update_ssbo", sgl_update_ssbo);
		push_c_method_x(J, "ssbo", sgl_ssbo);
		push_c_method_x(J, "bind_ssbo_base", sgl_bind_ssbo_base);
		push_c_method_x(J, "vertex_array", sgl_vertex_array);
		push_c_method_x(J, "delete_vertex_arrays", sgl_delete_vertex_arrays);
		push_c_method_x(J, "array_buffer", sgl_array_buffer);
		push_c_method_x(J, "array_sub_buffer", sgl_array_sub_buffer);
		push_c_method_x(J, "attrib_pointer", sgl_attrib_pointer);
		push_c_method_x(J, "element_buffer", sgl_element_buffer);
		push_c_method_x(J, "delete_buffers", sgl_delete_buffers);
		push_c_method_x(J, "active_texture", sgl_active_texture);
		push_c_method_x(J, "texture", sgl_texture);
		push_c_method_x(J, "delete_textures", sgl_delete_textures);
		push_c_method_x(J, "generate_texture_mipmap", sgl_generate_texture_mipmap);
		push_c_method_x(J, "renderbuffer", sgl_renderbuffer);
		push_c_method_x(J, "framebuffer", sgl_create_framebuffer);
		push_c_method_x(J, "current_framebuffer", sgl_current_framebuffer);
		push_c_method_x(J, "attach_framebuffer", sgl_attach_framebuffer);
		push_c_method_x(J, "check_framebuffer", sgl_check_framebuffer);
		push_c_method_x(J, "delete_framebuffer", sgl_delete_framebuffer);
		push_c_method_x(J, "blit_framebuffer", sgl_blit_framebuffer);

		push_c_method_x(J, "create_program", sgl_create_program);
		push_c_method_x(J, "use_program", sgl_use_program);
		push_c_method_x(J, "delete_program", sgl_delete_program);

		push_c_method_x(J, "clear_stencil", sgl_clear_stencil);
		push_c_method_x(J, "stencil_op", sgl_stencil_op);
		push_c_method_x(J, "stencil_func", sgl_stencil_func);
		push_c_method_x(J, "stencil_index", sgl_stencil_index);

		push_c_method_x(J, "viewport", sgl_viewport);
		push_c_method_x(J, "depth", sgl_depth);
		push_c_method_x(J, "pixel", sgl_pixel);
		push_c_method_x(J, "pixels", sgl_pixels);

		push_c_method_x(J, "uniform_1i", sgl_uniform_1i);
		push_c_method_x(J, "uniform_1ui", sgl_uniform_1ui);

		push_c_method_x(J, "uniform_1f", sgl_uniform_1f);
		push_c_method_x(J, "uniform_2f", sgl_uniform_2f);
		push_c_method_x(J, "uniform_3f", sgl_uniform_3f);
		push_c_method_x(J, "uniform_3fv", sgl_uniform_3fv);
		push_c_method_x(J, "uniform_4f", sgl_uniform_4f);
		push_c_method_x(J, "uniform_4fv", sgl_uniform_4fv);
		push_c_method_x(J, "uniform_matrix_3fv", sgl_uniform_matrix_3fv);
		push_c_method_x(J, "uniform_matrix_4fv", sgl_uniform_matrix_4fv);

		push_c_method_x(J, "get_uniform", sgl_get_uniform);
		push_c_method_x(J, "get_attribute", sgl_get_attribute);

		push_c_method_x(J, "clear", sgl_clear);
		push_c_method_x(J, "clear_color", sgl_clear_color);

		push_c_method_x(J, "cull_face", sgl_cull_face);
		push_c_method_x(J, "front_face", sgl_front_face);

		push_c_method_x(J, "enable", sgl_enable);
		push_c_method_x(J, "disable", sgl_disable);
	}
	duk_put_global_string(J, "sgl");
}
char sgl_init() {
	sgl_ready = 1;
	return 1;
}
void sgl_destroy() {
	sgl_ready = 0;
}

