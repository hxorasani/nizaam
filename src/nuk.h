#ifndef _nuk_h
#define _nuk_h

#include "duktape.h"
#include "common.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>

void nuk_start(SDL_Window *);
void nuk_destroy();
void nuk_input_start();
void nuk_input_event(SDL_Event *);
void nuk_input_end();
void nuk_render();
void nuk_bind_js(duk_context *);

#endif