#include "shader.h"

// Store all the file's contents in memory, useful to pass shaders
// source code to OpenGL.  Using SDL_RWops for Android asset support.
char *file_read(const char *filename) {
	SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
	if (rw == NULL) return NULL;
	
	Sint64 res_size = SDL_RWsize(rw);
	char* res = (char*)malloc(res_size + 1);

	Sint64 nb_read_total = 0, nb_read = 1;
	char* buf = res;
	while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}
	SDL_RWclose(rw);
	if (nb_read_total != res_size) {
		free(res);
		return NULL;
	}
	
	res[nb_read_total] = '\0';
	return res;
}
// Display compilation errors from the OpenGL shader compiler
void print_log(GLuint object) {
	GLint log_length = 0;
	if (glIsShader(object)) {
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else if (glIsProgram(object)) {
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else {
		printf("printlog: Not a shader or a program\n");
		return;
	}

	char* log = (char*)malloc(log_length);
	
	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);
	
	printf("%s\n", log);
	free(log);
}
// Compile the shader from file 'filename', with error handling
GLuint create_shader_text(const char *name, const char *text, GLenum type) {
	GLuint res = glCreateShader(type);
	const GLchar* sources[] = {
#ifdef GL_ES_VERSION_2_0
		"#version 310\n"  // OpenGL ES 3.1 // was 100 aka 2.0
#else
		"#version 450\n"  // OpenGL 3.3 // was 120 aka 2.1
#endif
	,
	text };
	glShaderSource(res, 2, sources, NULL);
	
	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		printf("%s\n", name);
		print_log(res);
		glDeleteShader(res);
		return 0;
	}
	
	return res;
}
GLuint create_shader(const char *filename, GLenum type) {
	const GLchar* source = file_read(filename);
	if (source == NULL) {
		printf("Error opening %s %s\n", filename, SDL_GetError());
		return 0;
	}
	GLuint uid = create_shader_text(filename, source, type);
	free((void*)source);
	return uid;
}
GLint shader_bind_attribute(const char *name, GLuint program, int *load_ok) {
	GLint attribute = glGetAttribLocation(program, name);
	if (attribute == -1) {
		printf("could not bind attribute %s\n", name);
		*load_ok = 0;
		return -1;
	}

	return attribute;
}
GLint shader_bind_uniform(const char *name, GLuint program, int *load_ok) {
	GLint uniform = glGetUniformLocation(program, name);
	if (uniform == -1) {
		printf("could not bind uniform %s\n", name);
		*load_ok = 0;
		return -1;
	}

	return uniform;
}



