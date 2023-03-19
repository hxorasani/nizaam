/* .xat .tarmeez file encode decode files
 * structs for use in other mods
 * 
 * TODO add global defaultxat xatcore();
 * */

#include "xat.h"		// header file

#define XATAA 0
#define ISOLATED	0
#define INITIAL		1
#define MEDIAL		2
#define FINAL		3
#define HARAKAH		4

int ftheight = 24, smooth = 1;
//char freetype_loaded = 0;
//FT_Library	library;

void errexit(u_char *s) {
	perror(s); exit(EXIT_FAILURE);
}
/*void rendercharcode(msfoof *r, ihaatahf *g, int charcode, int color) {
	if (!charcode) return;

	int error = 0, offx = 0, offy = 0, mw = 0, mh = 0;
	if (g) { offx = g->x; offy = g->y; mw = g->w; mh = g->h; }

	// TODO efficiency
//	unsigned int previous = 0;
//	if (prevchar)
//		previous = FT_Get_Char_Index( face, (unsigned long) prevchar );
		
	unsigned int glyph_index = FT_Get_Char_Index( face, (unsigned long) charcode );
//	printf("glyph_index %d\n", glyph_index);

	error = FT_Load_Glyph(face, glyph_index, 0 );
	if (error) errexit("freetype load glyph");

	FT_GlyphSlot slot = face->glyph;
	
	if (slot->format != FT_GLYPH_FORMAT_BITMAP) {
		if (smooth)
		error = FT_Render_Glyph( slot, FT_RENDER_MODE_NORMAL );
		else
		error = FT_Render_Glyph( slot, FT_RENDER_MODE_MONO );
		if (error) errexit("freetype render glyph");
	}

	FT_Bitmap bmp = slot->bitmap;
	FT_Pos bearingY = slot->metrics.horiBearingY >> 6;
	int ox = slot->bitmap_left;
//	int oy = slot->bitmap_top - bearingY;
	int oy = slot->bitmap_top - bearingY - (g->h-bearingY);

	ihaatah g2 = { 0, 0, bmp.width, bmp.rows };
	void hh( msfoof *m, int x, int y, int c ) {
		if ((mw && x >= mw) || (mh && y >= mh)) return;
		if (smooth) {
			u_char value = bmp.buffer[bmp.width * y + x];
			float br = 1.0f-(float) value/255;
			msfoof_set_blend(m, offx+ox+x, offy+y-oy, color, br );
		} else {
			u_char *row = &bmp.buffer[bmp.pitch * y];
			u_char value = row[(x >> 3)];
			char exists = (value & (128 >> (x & 7))) == 0;
			if (!exists) {
				msfoof_set(m, offx+ox+x, offy+y-oy, color );
			}
		}
	}
	msfoof_each(r, &hh, &g2);
}*/
int xat_arabic(int id) {
	return (id >= 160 && id <= 199);
}
int xat_latin(int id) {
	return (id >= 65 && id <= 90) || (id >= 97 && id <= 122);
}
int xat_3aks(unsigned char c) {
	switch (c) {
		case '<':
		case '>':
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '?':
		case '\\':
		case '/':
		case ',':
		case '`':
			return 1;
	}
	return 0;
}
int xat_harakah(int id) {
	return (id >= 230 && id <= 250);
}
int xat_initial(xat *x, int id) {
	if ( x->eqonaat[ eqonah_index(id, INITIAL) ].id > -1 ) return 1;
	return 0;
}
int xat_medial(xat *x, int id) {
	if ( x->eqonaat[ eqonah_index(id, MEDIAL) ].id > -1 ) return 1;
	return 0;
}
int xat_final(xat *x, int id) {
	if ( x->eqonaat[ eqonah_index(id, FINAL) ].id > -1 ) return 1;
	return 0;
}
void xat_ta3alluq(xat *x, int id, ta3alluq *t) {
	int ini = xat_initial(x, id),
		med = xat_medial(x, id)	,
		fin = xat_final(x, id)	;
	
	*t = (ta3alluq) { 0 };
	
	if (ini == 0 && med == 0 && fin == 0)
		t->none = 1;
	if (ini)
		t->end = 1;
	if (ini && med == 0 && fin == 0)
		t->endonly = 1;
	if (fin)
		t->start = 1;
	if (fin && med == 0 && ini == 0)
		t->startonly = 1;
	if (med) {
		t->start = 1;
		t->end = 1;
		t->both = 1;
	}
	if (xat_harakah(id))
		t->harakah = 1;
}
int xat_3alaqa(xat *x, int xc, int c, int nc) {
	/*
	 * if curr is connectable to prev only
	 * 		is prev connectable
	 * 			return curr final form
	 * if curr is connectable to next only
	 * 		is next connectable
	 * 			return curr initial form
	 * if curr is connectable to both
	 * 		are both prev & next connectable?
	 * 			return curr medial form
	 * 		only prev connectable
	 * 			return curr final form
	 * 		only next connectable
	 * 			return curr initial form
	 * */

	u_char form = 0;
	ta3alluq prev, next, curr;
	
	xat_ta3alluq( x, xc, &prev );
	xat_ta3alluq( x,  c, &curr );
	xat_ta3alluq( x, nc, &next );
	
	if (curr.harakah)
		form = HARAKAH;
	else
	if (curr.startonly && prev.end)
		form = FINAL;
	else
	if (curr.endonly && next.start)
		form = INITIAL;
	else {
		if (curr.start && prev.end) form = FINAL;
		if (curr.end && next.start) form = INITIAL;
		if (curr.both && prev.end && next.start) form = MEDIAL;
	}
	
	if ( x->eqonaat[ eqonah_index(c, form) ].id > -1 ) return form;
	else return ISOLATED;
}
int xat_decode(xat *x, char *matn, int length) {
}
int xat_encode(xat x, matn *m) {
	XATAA && printf("xat_encode %p %ld\n", m->barzax, m->length);
	matn_init(m);
	matn_adaaf(m, helper_stringify( "%d|%d", x.w, x.h ), 1);
	for (int i = 0; i < x.length; ++i) {
		if (x.eqonaat[i].id > -1) {
			eqonah_encode(x.eqonaat[i], m);
		}
	}
}
/*void xat_freetype() {
	if (freetype_loaded) return;
	
	int error = FT_Init_FreeType( &library );
	if (error) errexit("freetype init");

	error = FT_New_Face( library,
					"/home/nano/.fonts/segoeui.ttf",
//					"/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
					0,
					&face );

	if ( error == FT_Err_Unknown_File_Format )
		errexit("freetype unknown file format");
	else if ( error )
		errexit("freetype broken file");
	
	error = FT_Set_Pixel_Sizes(face, 0, ftheight);
	if (error) errexit("freetype set pixel sizes");

	freetype_loaded = 1;
}*/
int xat_init(xat *x, char *path) {
//	xat_freetype();

	XATAA && printf("xat_init %s\n", path);
	matn m;
	matn_init(&m);
	malaf mlf;
	malaf_init(&mlf, path);
	utf8 u;
	u.length = mlf.length; // same length as matn
	utf8_init(&u, NULL); // get memory allocated to u.length
	tarmeez_decode(&fushatarmeez, &mlf, &u);
	utf8_clean(&u); // remove junk
	utf8_ilaa_matn(&u, &m.barzax, &m.length); // convert to matn
	
	int c, col = 0, badeel = -1, line = 0;
	int eqonaatcount = 256*5, eqnhno = 0, id = -1, type;
	int strlimit = 8;
	unsigned char str[strlimit];
	int coli = 0; // key column index
	int stri = 0; // key string index
	int key = 0; // key string as int
	int eqnhfirstbreak = 0;
	int eqnhinside = 0; // =|<---- here
	int eqnhidindexed = 0; // =<number>\n
	int eqnhlength = 0; // length of eqonah malaf
	x->length = eqonaatcount;
	x->eqonaat = (eqonah *) calloc( eqonaatcount, sizeof(eqonah) );
	eqonah *eqnh;
	for (int i = 0; i < eqonaatcount; ++i) { // set ids to -1 first
		eqonah_nazaf(&x->eqonaat[i]);
	}

	long eqnhstart = 0;
	for (long i = 0; i < m.length+1; ++i) {
		if (i == m.length) c = 10; // to allow indexing the last column
		else c = m.barzax[i];
//		printf("%lc", c);

//		if (eqnhno > 3) return 1;
		
//		if (1) {} else
		if (c == ',' || c == '|' || c == 10) { // came across a pipe or new line
			// either way
			// null terminate string
			if (line > 0 && col == 0 && stri > 0) {
				str[stri-1] = '\0';
			} else {
				str[stri] = '\0';
			}
			// wcstol( const wchar_t *nptr, wchar_t **endptr, int base )
			key = strtol(str, (char **) &str[stri], 10);
			if (line == 0) {
				// exiting 1st and 2nd column
				if (col == 0) x->w = key;
				if (col == 1) x->h = key;
			} else {
				if (eqnhinside) {
					if (eqnhno) {
						if (col == 0) id = key;
						if (col == 1) type = key;
						eqnhidindexed = 1;
					}
				} else {
//					printf("%lc", c);
				}
			}
			matn_nazaf_str(str, strlimit); // reset string
			stri = 0; // reset key string index
			coli = 0; // reset key column index

			if (c == 10) { // only when it's a new line
//				XATAA && printf("\n[%d] ", line);
				if (eqnhinside) {
					if (!eqnhfirstbreak) {
						eqnhfirstbreak = 1;
						eqnhstart = i+1; // remember the starting index of this eqonah
					}
					eqnhinside = 0;
				}
				col = 0; // back to the first column
				key = -1; // key not read yet
				badeel = -1; // badeel not read yet
				line++;
			} else
			if (c == '|' || c == ',') { // only when it's a new pipe or comma
				col++; // going to the next column
			}
		} else {
//			if (eqnhinside && col) XATAA >= 2 && printf("(%d)%lc", col, c);
			if (c == '=') { // start of another eqonah
				eqnhfirstbreak = 0;
				eqnhinside = 1;
				// index previous eqonah = eqnhno-1
				if (eqnhlength && id > -1 && eqnhidindexed) {
					matn t;
					matn_init(&t);
					eqnh = &x->eqonaat[ eqonah_index(id, type) ];
					eqnh->w = x->w;
					eqnh->h = x->h;
					eqnh->id = id;
					eqnh->type = type;
					matn_iqtabas(&m, &t, eqnhstart, i);
					eqonah_decode(eqnh, t);
//					XATAA && printf("%d -> %d,%d", eqonah_index(id, type), eqnh->id, eqnh->type);
//					XATAA >= 2 && printf(" [%ld - %ld]", eqnhstart, i);
//					XATAA && printf(" (%d)\n", eqnh->length);
					matn_destroy(&t);
				}
				eqnhno++;
				eqnhidindexed = 0;
				eqnhlength = 0;
				id = -1;
				type = 0;
			}
			if (stri < strlimit) {
				if (line > 0 && col == 0 && stri > 0) {
					str[stri-1] = c;
				} else {
					str[stri] = c;
				}
				if (!eqnhinside) {
//					printf("%lc", c);
				}
				stri++;
			}
			coli++;
			eqnhlength++;
		}
	}
	
	utf8_destroy(&u);
	matn_destroy(&m);
	malaf_destroy(&mlf);
}