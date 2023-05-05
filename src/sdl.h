#ifndef _composer_sdl_bridge_h
#define _composer_sdl_bridge_h

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdbool.h>
#include "nuk.h"
#include "sgl.h"
#include "composer.h"

void sdl_set_frame_rate(int fps);
int sdl_get_frame_rate();
void sdl_composer_bridge(composer *cmpsr);
void sdl_composer_bridge_destroy();

char sdl_destroy_gl();
char sdl_init_gl();

#endif