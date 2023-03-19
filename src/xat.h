#ifndef _xat_h
#define _xat_h

#include "malaf.h"
#include "utf8.h"
#include "tarmeez.h"
#include "eqonah.h"

#include <stdlib.h>		// exit
#include <string.h>		// strerror
#include <errno.h>		// errno
//#include <sys/types.h>	// ??
#include <sys/stat.h>	// file stats
#include <stdio.h>		// perror printf
#include <unistd.h>		// close sleep
#include <fcntl.h>		// open O_RDWR

//#include <ft2build.h>
//#include FT_FREETYPE_H
//#include FT_GLYPH_H
//FT_Face		face;

typedef struct ta3alluq {
	char none		; // not joinable
	char end		; // connects to next
	char start		; // connects to prev
	char both		; // connects to both
	char endonly	; // connects to next only
	char startonly	; // connects to prev only
	char harakah	; // harakah
} ta3alluq;

typedef struct xat {
	int w;
	int h;
	int length; // length is always 256 * 4
	struct eqonah *eqonaat; 
} xat;

int xat_3aks(unsigned char c); // should invert?

int xat_arabic(int id);
int xat_latin(int id);

// [isolated, initial, medial, final, harakah]
int xat_initial(struct xat *x, int id);
int xat_medial(struct xat *x, int id);
int xat_final(struct xat *x, int id);
int xat_harakah(int id);

void xat_ta3alluq(xat *x, int id, ta3alluq *t);
int xat_3alaqa(xat *x, int xc, int c, int nc);

int xat_decode(struct xat *x, char *matn, int length);
int xat_encode(struct xat x, matn *m);
int xat_init(struct xat *x, char *path);

void errexit(u_char *s);
//void rendercharcode(msfoof *r, ihaatahf *g, int charcode, int color);

#endif