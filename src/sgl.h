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

void sgl_bind_js(duk_context *);

char sgl_init();
void sgl_destroy();

#endif