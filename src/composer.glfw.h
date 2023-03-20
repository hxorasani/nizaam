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
#include "mujs.h"
#include <cairo.h>

typedef struct {
	u_char *ism;
	u_char *xitaab;
	char (*b_icaarah)(mafateeh); // on_pointer
	char (*b_lowhah)(mafateeh); // on_keyboard
	char (*on_paint_window)(msfoof *); // on_draw RAEES
	char (*on_paint_indicator)(msfoof *); // on_draw MUSHEER
	void (*on_bound_window)(ihaatah); // on_resize RAEES
	void (*on_bound_indicator)(ihaatah); // on_resize MUSHEER
} composer;

extern kaatib ktb;

enum {
	KAATIB, SOORAH, SAWT, VIDEO
};

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

int composer_b_dihaan(int arg1);
int composer_b_lowhah(mafateeh wm);
void composer_load_app();
void composer_tamaam();
void composer_init(composer t);

#endif