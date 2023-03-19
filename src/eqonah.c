/* encode decode .eqonah malafaat and amtaan
 * structs for use in other mods
 * */
#include "eqonah.h"		// header file

#define XATAA 0

void eqonah_parsecoordstr(shakl *s, u_char *barzax, int end, int len) {
//	XATAA && printf("eqonah_parsecoordstr len %d\n", len);
	int coordno = 0, coordi = 0, coordstr[len], c, coord,
		nqtcount = 0, nqth = 0;
	// count total nuqaat first
	for (int j = end-len; j < end+1; ++j) {
		if (j == end) c = ' '; // to allow indexing the last nuqtah
		else c = barzax[j];
		if (c == ' ' || j == end-len) { // next nuqtah on space
			nqtcount++;
		}
	}
	
	if (nqtcount == 0) return;

	s->length = nqtcount;
	s->nuqaat = (nuqtahf *) calloc(nqtcount, sizeof(nuqtahf));
	if (s->nuqaat == NULL) {
		perror("eqonah_parsecoordstr calloc");
	}

	for (int j = end-len; j < end+1; ++j) {
		if (j == end) c = ','; // to allow indexing the last nuqtah
		else c = barzax[j];

		if (c == ' ' || c == ',') { // reset coordi
			coordstr[coordi] = '\0';
			coord = wcstol(coordstr, (wchar_t **) &coordstr[len], 10);
			if (nqth > -1) {
				if (coordno == 0)				  s->nuqaat[nqth].x  = coord;
				if (coordno == 1)				  s->nuqaat[nqth].y  = coord;
				if (coordno == 2)				  s->nuqaat[nqth].cx = coord;
				if (coordno == 3)				  s->nuqaat[nqth].cy = coord;
				if (coordno == 2 || coordno == 3) s->nuqaat[nqth].c = 1;
			}

			coordi = 0;
			utf8_emptystr(coordstr, len);

			if (c == ',') { // next coord on comma
				coordno++;
			} else
			if (c == ' ' || j == end-len) { // next nuqtah on space
				coordno = 0;
				nqth++;
				s->nuqaat[nqth].c = 0;
			}
		} else {
			coordstr[coordi] = c;
			coordi++;
		}
	}
}
int eqonah_index(int id, int type) {
	return id * 5 + type;
}
void eqonah_hudood(eqonah *e) { // calc the max unscaled bounds of an eqonah
	
}
int eqonah_draw(eqonah e, msfoof m, ihaatah *g, int color) {
	int ox = g->x, oy = g->y, fill = color, strk = color;
	float scl = g->s;
	int ew = e.w, eh = e.h, invert = g->i;
	int ew2 = e.w/2;
//	if (e.id) printf("%d,%d\n", e.id, e.type);
//	if (e.id) printf("g{%d,%d,%f,%d}\n", g->x, g->y, g->s, g->i);
//	msfoof_rect(&m, ox, oy, ew*scl, eh*scl, 0x555555, -1);
	for (int i = 0; i < e.length; ++i) {
		shakl s = e.ashkaal[i];

		if (s.f) { fill = color; } else { fill = -1; }
//		if (s.s) { strk = color; } else { strk = -1; }
//		printf("%x\n", strk);

		if (s.type == LINE) {
			if (invert) s.x = ew2 - s.x;

//			printf("line %d\n", s.length);
			for (int j = 1; j < s.length; ++j) {
				nuqtahf np = s.nuqaat[j-1],
						n = s.nuqaat[j];
				
				if (invert) {
					np.x = ew2 - np.x;
					n.x = ew2 - n.x;
				}

				int x0 = ox+(s.x*scl)+(np.x*scl), y0 = oy+(s.y*scl)+(np.y*scl),
					x2 = ox+(s.x*scl)+(n.x*scl) , y2 = oy+(s.y*scl)+(n.y*scl) ;
				
				if (n.c) {
					if (invert) n.cx = ew2 - n.cx;

					int x1 = ox+(s.x*scl)+(n.cx*scl), y1 = oy+(s.y*scl)+(n.cy*scl);
					
					msfoof_curve(&m, x0, y0, x1, y1, x2, y2, strk);
				} else {
					msfoof_line(&m, x0, y0, x2, y2, strk);
				}
			}
		}
		if (s.type == CIRCLE) {
//			printf("circle xy(%f x %d) r%d \n", ox+(s.x*scl), s.y, s.r);
			matrix_circle(&m, ox+(s.x*scl), oy+(s.y*scl), s.r*scl, 0, 360, strk, fill);
		}
		if (s.type == RECT) {
			if (invert) s.x = ew - s.w - s.x;

			int w = s.w*scl, h = s.h*scl;

//			printf("rect xy(%f x %d) wh(%d x %d) filled(%d) stroke(%d)\n", ox+(s.x*scl), s.y, w, h, s.f, s.s);
			msfoof_rect(&m, ox+(s.x*scl), oy+(s.y*scl), w, h, strk, fill);
		}
	}
}
int eqonah_sattar(eqonah e, msfoof m, ihaatahf *g, int color) {
	ihaatah g2 = { g->x, g->y, g->w, g->h, g->s, g->i };
//	eqonah_draw(e, m, &g2, color);
	float ox = g->x, oy = g->y, scl = g->s;
	int fill = color, strk = color;
	int ew = e.w, eh = e.h, invert = g->i;
	float ew2 = e.w/2.0;
//	if (e.id) printf("eqonah id %d type %d\n", e.id, e.type);
//	if (e.id) printf("g{%d,%d,%f,%d}\n", g->x, g->y, g->s, g->i);
//	msfoof_rect(&m, ox, oy, ew*scl, eh*scl, 0x555555, -1);
	if (e.id == 221) {
//		msfoof_rect(&m, ox, oy, g2.w, g2.h, color, color);
		msfoof_rect(&m, ox, oy, g2.w, eh*scl, color, -1);
		msfoof_rect(&m, ox+1, oy+1, g2.w-2, eh*scl-2, color, -1);
		return 0;
	}
	for (int i = 0; i < e.length; ++i) {
		shakl s = e.ashkaal[i];

		if (s.f) { fill = color; } else { fill = -1; }
//		if (s.s) { strk = color; } else { strk = -1; }
//		printf("%x\n", strk);

		if (s.type == LINE) {
			if (invert) s.x = ew2 - s.x;

//			printf("line %d\n", s.length);
			/* TODO
			 * the -1 to ignore the last command is weird
			 * is that the stroke or fill command or what is it?
			 * it was messing with xutoot where the where connecting the last
			 * nuqtah to the first one, like a closed polygon :/
			 * 
			 * fix this buggy temp conditional fix :(
			 * */
			int buggytotal = s.length-1;
			if (e.id == -1) buggytotal = s.length; // hint that not in xatmode

			for (int j = 1; j < buggytotal; ++j) {
				nuqtahf np = s.nuqaat[j-1],
						n = s.nuqaat[j];
				
				if (invert) {
					np.x = ew2 - np.x;
					n.x = ew2 - n.x;
				}

				float x0 = ox+(s.x*scl)+(np.x*scl), y0 = oy+(s.y*scl)+(np.y*scl),
					x2 = ox+(s.x*scl)+(n.x*scl) , y2 = oy+(s.y*scl)+(n.y*scl) ;
				
				if (n.c) {
					if (invert) n.cx = ew2 - n.cx;

					float x1 = ox+(s.x*scl)+(n.cx*scl), y1 = oy+(s.y*scl)+(n.cy*scl);
					
					if (m.smooth) msfoof_3atf(&m, x0, y0, x1, y1, x2, y2, strk);
					else msfoof_curve(&m, x0, y0, x1, y1, x2, y2, strk);
				} else {
					if (m.smooth) msfoof_satar(&m, x0, y0, x2, y2, strk);
					else msfoof_line(&m, x0, y0, x2, y2, strk);
				}
			}
		}
		if (s.type == CIRCLE) {
//			printf("circle xy(%f x %d) r%d \n", ox+(s.x*scl), s.y, s.r);
			matrix_circle(&m, ox+(s.x*scl), oy+(s.y*scl), s.r*scl, 0, 360, strk, fill);
		}
		if (s.type == RECT) {
			if (invert) s.x = ew - s.w - s.x;

			float w = s.w*scl, h = s.h*scl;

//			printf("rect xy(%f x %d) wh(%d x %d) filled(%d) stroke(%d)\n", ox+(s.x*scl), s.y, w, h, s.f, s.s);
			msfoof_rect(&m, ox+(s.x*scl), oy+(s.y*scl), w, h, strk, fill);
		}
	}
}
int eqonah_decode(eqonah *e, matn m) {
	XATAA && printf("eqonah_decode %d,%d len(%ld) ", e->id, e->type, m.length);
//	XATAA && printf("%s\n", m.barzax);

	unsigned char c, fc = 0;
	int col = 0, line = 0;
	int ashkaalcount = 0, shaklno = -1;
	int strlimit = 4;
	unsigned char str[strlimit];
	int coli = 0; // key column index
	int stri = 0; // key string index
	int xatmode = 1; // if eqonah does have w & h set, assume we're in xat mode
	if (e->w == 0 && e->h == 0) xatmode = 0;
	int key = 0; // key string as int

	// when fc is \0 or '0', the entire line gets ignored
	for (long i = 0; i < m.length; ++i) { // count ashkaal first
		c = m.barzax[i];
		// whenever come across \n & fc is not indexed
		if (c == '\n' || i == 0) {
			if (i == 0) fc = c;
			else if (i < m.length-1 && fc == 0) fc = m.barzax[i+1];

			if (fc <= '9' && fc > '0') {
				ashkaalcount++;
			}

			fc = 0; // reset first character
		}
	}
	fc = 0;

	e->length = ashkaalcount;
	e->ashkaal = (shakl *) calloc(ashkaalcount, sizeof(shakl));
	XATAA && printf(" <%d>", ashkaalcount);

	shakl *shkl;
	int ignore = 1;
	for (int i = 0; i < m.length; ++i) {
		if (i == m.length) c = 10; // to allow indexing the last column
		else c = m.barzax[i];

		if (c == '\n' || i == 0) {
			if (i == 0) fc = c;
			else if (i < m.length && fc == 0) fc = m.barzax[i+1];

			if (fc <= '9' && fc > '0') {
				ignore = 0;
				shaklno++;
				XATAA >= 3 && printf("(fc %c [%d]) ", fc, shaklno);
			} else {
				ignore = 1;
			}
			fc = 0; // reset first character
		}
		
		if (ignore) {}
		else if (c == '|' || c == 10) { // came across a pipe or new line
			// either way
			str[stri] = '\0'; // null terminate string
			// wcstol( const wchar_t *nptr, wchar_t **endptr, int base )
			key = strtol(str, (char **) &str[stri], 10);
			if (line == 0 && !xatmode) {
				// exiting 1st and 2nd column
				if (col == 0) e->w = key;
				if (col == 1) e->h = key;
			} else {
				shkl = &e->ashkaal[ shaklno ];
				if (col == 0) shkl->type	= key;
				if (col == 1) shkl->x		= key;
				if (col == 2) shkl->y		= key;
				if (shkl->type == LINE) {
					if (col == 3) {
						eqonah_parsecoordstr(shkl, m.barzax, i, coli);
					}
				}
				if (shkl->type == CIRCLE) {
					if (col == 3) shkl->r = key;
					if (col == 4) shkl->as = key;
					if (col == 5) shkl->ae = key;
					if (col == 6) shkl->w = key||1;
					if (col == 7) shkl->f = key||0;
//					if (col == 8) shkl->fg = key;
				}
				if (shkl->type == RECT) {
					if (col == 3) shkl->w = key;
					if (col == 4) shkl->h = key;
					if (col == 5) shkl->f = key; // filled?
					if (col == 6) shkl->s = key; // stroke width
				}
			}
			matn_nazaf_str(str, strlimit); // reset string
			stri = 0; // reset key string index
			coli = 0; // reset key column index

			if (c == 10) { // only when it's a new line
//				XATAA && printf("\n[%d] ", line);
				col = 0; // back to the first column
				key = -1; // key not read yet
				line++;
			} else
			if (c == '|') { // only when it's a new pipe
				col++; // going to the next column
			}
		} else {
			if (stri < strlimit) {
//				XATAA >= 3 && printf("(%d)", col);
				str[stri] = c;
				stri++;
			}
			coli++;
		}
	}
	
	XATAA > 1 && !xatmode && printf("eqonah w: %d h: %d\n", e->w, e->h);
	if (XATAA > 1)
	for (int i = 0; i < e->length; ++i) {
		struct shakl s = e->ashkaal[i];
		if (s.type == LINE) printf("line");
		if (s.type == CIRCLE) printf("circle");
		if (s.type == RECT) printf("rect");

		printf(" xy(%d x %d)", s.x, s.y);

		if (s.type == LINE) {
			printf(" nuqaat %d", s.length);
			if (XATAA >= 2)
			for (int j = 0; j < s.length; ++j) {
				printf("\n");
				printf("nuqtah %f %f", s.nuqaat[j].x, s.nuqaat[j].y);
				if (s.nuqaat[j].c)
					printf(" c(%f %f)", s.nuqaat[j].cx, s.nuqaat[j].cy);
			}
		}
		if (s.type == RECT) {
			printf(" wh(%f x %f) filled(%d) stroke(%d)", s.w, s.h, s.f, s.s);
		}
		printf("\n");
	}

	XATAA && printf("\n");
}
int eqonah_encode(eqonah e, matn *m) {
	if (e.id > -1) {
		XATAA && printf("eqonah_encode %d ashkaal(%d)\n", e.id, e.length);
		matn_adaaf(m, helper_stringify( "\n=%d,%d", e.id, e.type ), 1);
	}
	for (int i = 0; i < e.length; ++i) {
		shakl s = e.ashkaal[i];
		matn_adaaf(m, helper_stringify( "\n%d|%d|%d", s.type, s.x, s.y ), 1);

		if (s.type == LINE) {
			matn_adaaf(m, "|", 0);
			for (int j = 0; j < s.length; ++j) {
				if (j) matn_adaaf(m, " ", 0);
				nuqtahf n = s.nuqaat[j];
				matn_adaaf(m, helper_stringify( "%.0f,%.0f", n.x, n.y ), 1 );
				if (n.c)
				matn_adaaf(m, helper_stringify( ",%.0f,%.0f", n.cx, n.cy ), 1 );
			}
			matn_adaaf(m,
				helper_stringify( "|%.1f|%d|%ld", s.w, s.f, s.fg ), 1
			);
		}
		if (s.type == CIRCLE) {
			matn_adaaf(m,
				helper_stringify( "|%d|%d|%d|%.1f|%d|%ld",
				s.r, s.as, s.ae, s.w, s.f, s.fg ), 1
			);
		}
		if (s.type == RECT) {
			matn_adaaf(m,
				helper_stringify( "|%.1f|%.1f|%d|%d|%ld",
				s.w, s.h, s.f, s.s, s.fg ), 1
			);
		}
	}
}
void eqonah_destroy(eqonah *e) {
	if (e) {
		if (e->ashkaal) {
			// go over ashkaal, each shakl has nuqaat calloc possibly, free em
			free(e->ashkaal);
			e->ashkaal = NULL;
		}
	}
}
void eqonah_nazaf(eqonah *e) {
	if (e) {
		e->id = -1; e->type = 0; e->w = 0; e->h = 0;
		ihaatah_nazaf(&e->hudood);
		e->ashkaal = NULL;
	}
}
int eqonah_init(eqonah *e, char *path) {
	malaf mlf;
	malaf_init(&mlf, path);
	matn m = { 0 };
	malaf_ilaa_matn(&mlf, &m.barzax, &m.length);
	eqonah_decode(e, m);
	matn_destroy(&m);
	malaf_destroy(&mlf);
}
