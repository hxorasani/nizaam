#ifndef _kaatib_h
#define _kaatib_h

#include "matn.h"
#include "utf8.h"
#include "eqonah.h"
#include "canvas.h"
#include "xat.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

xat xat2022; // default global xat

typedef struct harf {
	u_char id;
	u_char harakaat[4]; // null terminated
	u_char muntaxab;
	u_char wazan;
	char type;
	char mutadarrar;
	char ltr;
	ihaatahf ihth; // maximum bounds x, y, w, h
	ihaatahf aqil; // minimum bounds
} harf;
typedef struct lafz {
	long length;
	char mutadarrar;
	harf *barzax;
} lafz;
typedef struct kaatib {
	int mst3d;
	float hrfmsfh; // letter spacing harf masaafah
	char ltr; // current direction
	long cursor;
	long showcursor;
	long length;
	ihaatahf g; // where to place in mtrx
	ihaatah muntahaa; // maximum bounds for scrolling
	ihaatah gw; // same but cached extra space
	ihaatah darar; // damage region, gets reset on draw
	nuqtahf n; // scrolling offsets, ahdaaf for mutaharrikah, .c toggle anime
	long muntaxab; // selection offsets, x to y
	palette p;
	xat x;
	msfoof mtrx;
	matn mtn;
	canvas *qmc;
	char taht; // background, underpaint
	char mu3aaq; // disabled
	int ma3looq; // allocated
	harf *barzax; // if NULL, don't use huroof then, render directly
} kaatib;
void kaatib_msfoof( kaatib *m, msfoof *msff ); // set masfoof
void kaatib_ilaa_matn( kaatib *m ); // barzax -> internal matn
char kaatib_pageup( kaatib *m );
char kaatib_pagedown( kaatib *m );
char kaatib_home(kaatib *m);
char kaatib_end(kaatib *m);
char kaatib_mutadarrar(kaatib *m);
char kaatib_up( kaatib *m );
char kaatib_down( kaatib *m );
char kaatib_right( kaatib *m );
char kaatib_left( kaatib *m );
char kaatib_muntaxab( kaatib *m );
char kaatib_muntaxab_jaanibi(kaatib *m, char jaanib);
void kaatib_muntaxab_tamaam(kaatib *m);
char kaatib_muntaxab_kul( kaatib *m );
char kaatib_ghayr_muntaxab_kul( kaatib *m );
char kaatib_wazan( kaatib *m );
void kaatib_destroy( kaatib *m );
void kaatib_hadhaf_kul( kaatib *m ); // delete all
void kaatib_hadhaf( kaatib *m, char qaadim ); // delete char
void kaatib_baddal( kaatib *m, u_char *c ); // replace with str
void kaatib_tasteer( kaatib *m, msfoof *msff, u_char *str );
void kaatib_adaaf( kaatib *m, u_char c ); // add/insert char
void kaatib_huroof( kaatib *m );
void kaatib_itlaq_hudood( kaatib *m );
int kaatib_harf_zaahir( kaatib *m, int i );
int kaatib_zaahir( kaatib *m, harf *h );
harf * kaatib_cu3aa3( kaatib *m, float x, float y );
void kaatib_muashshar( kaatib *m );
void kaatib_idhan_ikraah( kaatib *m, msfoof msff );
void kaatib_idhan( kaatib *m, msfoof msff, char ikraah );
void kaatib_draw( kaatib *m );
void kaatib_qadd( kaatib *m, float qadd );
void kaatib_xataa( kaatib *m );
void kaatib_init( kaatib *m );

#endif