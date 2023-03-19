#ifndef _matn_h
#define _matn_h

#include "helper.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

typedef struct matn {
	long length;
	unsigned char *barzax;
} matn;
void matn_nazaf_str( unsigned char *p, int len );
void matn_nazaf( matn *m );
void matn_destroy( matn *m );
void matn_adaaf( matn *m, unsigned char *p, int autofree ); // auto frees *p
int matn_iqtabas( matn *s, matn *t, long start, long end );
int matn_iqtabas_uc( matn *s, u_char **t, long start, long end );
void matn_init( matn *m );

#endif