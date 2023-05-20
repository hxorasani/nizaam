#ifndef _ttf_h
#define _ttf_h

#include "duktape.h"
#include "common.h"
#include "ttf2mesh.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>

void ttf_start();
void ttf_destroy();
void ttf_bind_js(duk_context *);

#endif