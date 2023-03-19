#ifndef _eqonah_h
#define _eqonah_h

#include "msfoof.h"
#include "malaf.h"
#include "utf8.h"
#include "matn.h"
#include "helper.h"

#include <stdlib.h>		// exit
#include <string.h>		// strerror
#include <errno.h>		// errno
//#include <sys/types.h>	// ??
#include <sys/stat.h>	// file stats
#include <stdio.h>		// perror printf
#include <wchar.h>		// wcstol
#include <unistd.h>		// close sleep
#include <fcntl.h>		// open O_RDWR

#define LINE	1
#define BONE	2
#define RECT	3
#define CIRCLE	4

#define ISOLATED	0
#define INITIAL		1
#define MEDIAL		2
#define FINAL		3
#define HARAKAH		4

typedef struct shakl { // shape
	long bg; // background color
	long fg; // foreground color
	int tabqah; // layer
	int type;
	int x;
	int y;
	float w;
	float h;
	int as; // angle start
	int ae; // angle end
	int r; // radius
	int f; // filled?
	int s; // stroke width
	int length; // total nuqaat
	nuqtahf *nuqaat;
} shakl;
typedef struct eqonah { // icon
	int id; // unique id (m: 0-255) within a set/namespace like a xat
	int type; // [isolated, initial, medial, final, harakah]
	int w;
	int h;
	int length; // total ashkaal
	ihaatah hudood; // max bounds
	shakl *ashkaal; // shapes
} eqonah;
int eqonah_index(int id, int type);
int eqonah_draw(eqonah e, msfoof m, ihaatah *g, int color);
int eqonah_sattar(eqonah e, msfoof m, ihaatahf *g, int color);
int eqonah_decode(eqonah *e, matn m);
int eqonah_encode(eqonah e, matn *m);
void eqonah_destroy(eqonah *e);
void eqonah_nazaf(eqonah *e);
int eqonah_init(eqonah *e, char *path);

#endif