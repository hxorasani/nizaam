#ifndef _composer_h
#define _composer_h

#include "matn.h"
#include "utf8.h"
#include "helper.h"
#include "amr.h"
#include "tarmeez.h"

#ifndef COMPOSER_SDL
	#define COMPOSER_SDL
#endif

#ifdef COMPOSER_SDL
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_events.h>
#endif
#ifdef COMPOSER_GLFW
	#include <GLFW/glfw3.h>
#endif

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <errno.h>

#include "canvas.h"
#include "reasings.h"

typedef struct {
	u_char *ism;
	u_char *xitaab;
	char (*on_pointer)(mafateeh);
	char (*on_touch)(mafateeh);
	char (*on_pinch)(mafateeh);
	char (*on_rotate)(mafateeh);
	char (*b_lowhah)(mafateeh); // on_keyboard
	char (*on_paint_window)(); // on_paint RAEES
	char (*on_paint_indicator)(); // on_paint MUSHEER
	void (*on_bound_window)(ihaatahf); // on_resize RAEES
	void (*on_bound_indicator)(ihaatahf); // on_resize MUSHEER
	void (*after_canvas_raees)(msfoof *); // after canvas is drawn on raees
	void (*on_tick)();
	void (*on_drop)(char *);
	char (*on_focus)(char);
	void (*on_reload)(); // for drawing calls
//	void (*on_frame)(); // on new frame
	canvas *qmc;

	// override amr* functions
	void (*title)(const u_char *);
	void (*set_window)(u_char *);
	void (*event_provider)( int (*onwaaqi3ah)() );
} composer;

extern canvas *current_canvas;
extern msfoof *current_matrix;

double composer_ease_by_name(int index, double t, double b, double c, double d);
void composer_attach(composer *t);
int composer_b_dihaan(int arg1);
int composer_b_lowhah(mafateeh wm);
char composer_cursor(char);
void composer_load_app(char i);
void composer_clear();
void composer_destroy();
void composer_init(composer t);

msfoof * get_current_matrix();

#endif