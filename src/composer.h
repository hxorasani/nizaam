#ifndef _composer_h
#define _composer_h

#include "matn.h"
#include "utf8.h"
#include "eqonah.h"
#include "xat.h"
#include "helper.h"
#include "amr.h"
#include "kaatib.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <errno.h>
#include <cairo.h>

//#include "cairo-blur.h"
#include "reasings.h"

typedef struct {
	u_char *ism;
	u_char *xitaab;
	char (*on_pointer)(mafateeh);
	char (*b_lowhah)(mafateeh); // on_keyboard
	char (*on_paint_window)(); // on_paint RAEES
	char (*on_paint_indicator)(); // on_paint MUSHEER
	void (*on_bound_window)(ihaatahf); // on_resize RAEES
	void (*on_bound_indicator)(ihaatahf); // on_resize MUSHEER
	void (*after_canvas_raees)(msfoof *); // after canvas is drawn on raees
	void (*on_focus)(char);
	void (*on_reload)(); // for drawing calls
//	void (*on_frame)(); // on new frame
	canvas *qmc;

	// override amr* functions
	void (*set_window)(u_char *);
	void (*event_provider)( int (*onwaaqi3ah)() );
} composer;

kaatib ktb;
canvas *current_canvas;
cairo_t *current_context;
cairo_surface_t *current_surface;
msfoof *current_matrix;
void composer_kaatib_paint(char);

enum { KAATIB, SOORAH, SAWT, VIDEO };

typedef struct {
	float x, y, w, h;
	int eadad, z; // unique number, layer
	int naw3; // type like KAATIB, SOORAH
	void (*ibtadaa)(); // initialize
	void (*inhalak)(); // destroy
	union {
		kaatib *ktb;
//		soorah *srh;
//		sawt   *swt;
//		video  *vid;
	};
//	*niqaab; // mask
} element;

typedef struct {
	int length; // length
//	u_char *xitaab;
} view;

// canvas direct calls
/*void font_bounds(ihaatahf *ih);
void harf_hajm(ihaatahf *ih, int c);
void huroof_hajm(ihaatahf *ih, int *c);
void matn_harf(int c);
void matn_huroof(int *c);
void xat_wijhah(const char *c);
void font_size(float qadd);
void mustateel(ihaatahf ih);
void move_to(float x, float y);
void satar(float x, float y);
void stroke(char preserve);
void fill(char preserve);
void iqta3();
void renew_path();
void lwn_rgb(float r, float g, float b);
void lwn_rgba(float r, float g, float b, float a);*/

float composer_ease_by_name(int index, float t, float b, float c, float d);
void composer_attach(composer *t);
void composer_canvas(char arg1);
int composer_b_dihaan(int arg1);
int composer_b_lowhah(mafateeh wm);
void composer_load_app(char i);
void composer_tamaam();
void composer_init(composer t);

msfoof * get_current_matrix();

#endif