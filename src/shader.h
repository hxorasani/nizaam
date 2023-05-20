#ifndef _shader_h
#define _shader_h
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

extern char *file_read(const char *filename);
extern void print_log(GLuint object);
extern GLuint create_shader(const char *filename, GLenum type);
extern GLuint create_shader_text(const char *name, const char *text, GLenum type);
GLint shader_bind_attribute(const char *name, GLuint program, int *load_ok);
GLint shader_bind_uniform(const char *name, GLuint program, int *load_ok);

#endif
