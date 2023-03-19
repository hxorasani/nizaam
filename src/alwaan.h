//#include "alwaan.8bit.h"
#ifndef _alwaan_h
#define _alwaan_h

#include <math.h>
#include <stdlib.h>
#define swap_(a, b) do{ __typeof__(a) tmp;  tmp = a; a = b; b = tmp; }while(0)
#define IS_BIG_ENDIAN (!*(unsigned char *)&(u_int16_t){1})

extern int EIGHTBIT[256];

typedef struct lwn {
	u_char a;
	u_char r;
	u_char g;
	u_char b;
} lwn;
typedef struct lwnf {
	float a;
	float r;
	float g;
	float b;
} lwnf;
typedef struct lwnh {
	float h; // hue
	float s; // sat
	float v; // val or lum
} lwnh;
float haddqanaatf255(float q);
float haddqanaatf(float q);
int haddqanaat(int q);
unsigned char hl2char (float h, float l);
unsigned char rgb2char (int rgb);
int char2rgb (unsigned char a);
void argb2lwn (int argb, lwn *l);
void rgb2lwn (int rgb, lwn *l);
void rgb2lwnf (int rgb, lwnf *l);
void rgb2lwnh (int rgb, lwnh *l);
int lwnl2rgb (lwnh *l); // h s lum
int lwnh2rgb (lwnh *l); // h s val
int lwn2argb (lwn *l);
int lwnf2rgb (lwnf *l);
u_int lwn2rgb (lwn *l);

void argb_to_components(u_int argb, u_char *a, u_char *r, u_char *g, u_char *b);
void components_to_argb(u_char a, u_char r, u_char g, u_char b, u_int *argb);

/* CAIRO compat helpers
 * these also deal with endianess issues
 * */
int argb32torgb (u_char *c);
int argb32_get_a(u_char *c);

int argb2rgb(int c);
int alwaan_a (int color, float pct); // pct of channel alif
int alwaan_r (int color, float pct);
int alwaan_g (int color, float pct);
int alwaan_b (int color, float pct);
int alwaan_mod (int color, float r, float g, float b);
int alwaan_add (int color0, int color1);
int alwaan_avg (int color0, int color1);
int alwaan_pct (int color, float pct);
int alwaan_avg_pct(int color0, int color1, float pct);
int alwaan_add_pct(int color0, int color1, float pct);

#endif