#ifndef _sgl_h
#define _sgl_h

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "helper.h"
#include "msfoof.h"
#include "shader.h"
#include "duktape.h"
#include "composer.h"
#include "common.h"

void * sgl_pixels_to_pointer(float x, float y, float w, float h, GLenum format,
					GLenum type, int *out_len, int *out_comps, int *out_bpc);

void sgl_bind_js(duk_context *);

char sgl_init();
void sgl_destroy();

#endif