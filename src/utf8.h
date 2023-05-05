#ifndef _utf8_h
#define _utf8_h

#include <wctype.h>
#include <locale.h>
#include <wchar.h>

#include <stdlib.h>		// exit
#include <string.h>		// strerror
#include <stdio.h>		// perror printf

typedef struct utf8 {
	long length;
	int * barzax;
} utf8;

int uni2chars(char *dest, int uni, int size);
char *unis2chars(int *uni);
int chars2unis(int *dest, char *uni, int size);
void utf8_nazzaf( utf8 *u );
void utf8_clean( utf8 *u );
void utf8_emptystr(int *str, int len);
long utf8_strlen(int * barzax);
int utf8_cmp(int *a, int *b);
int utf8_cmp_uchar(int *a, unsigned char *b);
// takes array of utf8 chars, returns error
void utf8_destroy( utf8 *u);
int utf8_iqtabas_matn( utf8 *s, unsigned char **barzax, long length, long start, long end);
int utf8_ilaa_matn( utf8 *s, unsigned char **barzax, long *length );
int utf8_iqtabas( utf8 *s, utf8 *t, long start, long end);
int utf8_init( utf8 *u, void *barzax );

#endif