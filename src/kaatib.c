/* kaatib
 * wijhah juzw that provides an editable textbox, can be readonly
 * standard C char input, bidirectional, navigation, export, selection, 
 * 
 * TODO
 * add up/down selection
 * make up/down accurate
 * copy/paste
 * zooming should clear all inside the bounds
 * */

#include "kaatib.h"

#define XATAA 0
#define CLEAR_PRINT 1
#define PRINT_ONLY 2
#define MUSTATEEL 30
#define SATAR 40
#define TAHT 50
int selstyle = TAHT;
int xatinit = 0;
int mono = 0;
ihaatahf ahjaam[256] = { 0 };
ihaatahf xathajm = { 0 };

void renderselection(kaatib *m, ihaatahf ih) {
	if (m->qmc) {
		if (m->qmc->lwn_rgb		) m->qmc->lwn_rgb(.4, .3, .2);
		if (m->qmc->mustateel	) m->qmc->mustateel(ih);
		if (m->qmc->fill		) m->qmc->fill(0);
		if (m->qmc->renew_path		) m->qmc->renew_path();
	}
}
void rendercharat(kaatib *m, ihaatahf ih, int c) {
	if (m->qmc) {
		XATAA > 1 && printf("rendercharat %lc\n", c);
		if (m->qmc->lwn_rgb)
			m->qmc->lwn_rgb(1, 1, 1);
		if (m->qmc->move_to)
			m->qmc->move_to( ih.x, ih.y + (xathajm.h - xathajm.w) );
		if (m->qmc->matn_harf)
			m->qmc->matn_harf( c );
	}
}
void rendercursor(kaatib *m, ihaatahf ih, int c) {
	if (m->qmc) {
		if (m->qmc->lwn_rgb		) m->qmc->lwn_rgb(.2, .3, .7);
		if (m->qmc->mustateel	) {
			if (ih.x < 0) ih.x += 3;
			if (ih.x+ih.w > m->g.w) ih.x -= 3;
			m->qmc->mustateel(ih);
		}
		if (m->qmc->fill		) m->qmc->fill(0);
		if (m->qmc->renew_path		) m->qmc->renew_path();
	}
}
void kaatib_harf_hajm( kaatib *m, int c) {
	if (m->qmc) {
		if (m->qmc->harf_hajm) {
			ihaatahf ih = { 0 };
			m->qmc->harf_hajm(&ih, c);
			
//			if (ih.x)
//			printf("%d %f %f \n", c, ih.x, ih.h);

			ahjaam[c].x = ih.x;
			ahjaam[c].y = ih.y;
			ahjaam[c].w = ih.w;
			ahjaam[c].h = ih.h;
		}
	}
}
char fillerfunc(msfoof *m, int x, int y, int color) {
	float pct = 1.0 - ( (float) x / (float) m->w);
	msfoof_set(m, x, y,
		alwaan_avg(
			color,
			alwaan_avg(
				alwaan_pct(0x000000, pct),
				alwaan_pct(0x000000, 1.0-pct)
			)
		)
	);
	return 1;
}
void kaatib_msfoof( kaatib *m, msfoof *msff ) {
	m->mtrx = *msff;
	m->mtrx.clearfn = &fillerfunc;
}
void kaatib_harf(harf *h) { // initialize a harf
	/* EXPLAIN
	 * valgrind was saying this is not init'd so i used memset
	 * that fixed it
	 * */
	memset( h, 0, sizeof(harf) );
	h->id = 0;
	h->mutadarrar = 0;
	h->muntaxab = 0;
	h->wazan = 0;
	memset( h->harakaat, 0, 4 );
	ihaatahf_nazaf(&h->ihth);
	ihaatahf_nazaf(&h->aqil);
}
void kaatib_ilaa_matn(kaatib *m) {
	XATAA && printf("kaatib_ilaa_matn\n");
	harf *str = m->barzax;
	if (m->barzax && m->length && m->mtn.barzax)
	for (int k = 0; str[k].id != '\0' && k < m->length; ++k) {
		u_char *p = helper_stringify("%c%s", str[k].id, str[k].harakaat);
		if (p) { matn_adaaf(&m->mtn, p, 1); }
	}
}
void kaatib_destroy(kaatib *m) {
	if (m) {
		if (m->barzax) {
			XATAA && printf("kaatib_destroy try free %p\n", m->barzax);
			free(m->barzax);
			m->barzax = NULL;
		}
		m->ma3looq = 0;
		// .qmc is the dev's responsibility to set/unset/maintain
	}
}
void kaatib_darar_hwl(kaatib *m, int i) { // smart clear harf + surroundings
	if (i > -1 && i < m->length) {
		if (i > 0 && m->barzax[ i-1 ].mutadarrar == 0)
			m->barzax[ i-1 ].mutadarrar = PRINT_ONLY;
		if (m->barzax[ i ].mutadarrar == 0)
			m->barzax[ i ].mutadarrar = CLEAR_PRINT;
		if (i+1 < m->length && m->barzax[ i+1 ].mutadarrar == 0)
			m->barzax[ i+1 ].mutadarrar = PRINT_ONLY;
	}
}
void kaatib_darar(kaatib *m, int i) { // clear ghosted huroof
	if (i > 0 && i < m->length) {
		ihaatahf g = m->barzax[i].ihth;
		if (mono) g.w = xathajm.x;
		g.h = xathajm.h;

		if (m->taht)
		msfoof_clear_area_f(&m->mtrx, 0, &g);
		ihaatah_ws3(&m->darar, g.x, g.y, g.w, g.h);
	} else
	if (i == -1) { // -1 is a special case
		float	harfw = xathajm.x,
				harfh = xathajm.h;

		if (m->hrfmsfh) {
			harfw -= m->g.s * m->hrfmsfh;
//			harfh -= m->g.s * m->hrfmsfh;
		}

		char ltr = 0;
		if (m->length) {
			char c = m->barzax[0].id;
			if ( xat_latin(c) ) ltr = 1; else if ( xat_arabic(c) ) ltr = 0;
		}

		ihaatah g = { 0, 0, harfw, harfh, m->g.s };
		g.y = 0;
		if (ltr) {
			g.x = m->g.x + (-1 * harfw) - m->hrfmsfh;
			if (m->hrfmsfh) g.x += g.s * m->hrfmsfh;
		}
		else {
			g.x = m->g.x + ( m->g.w - (-1 * harfw) - harfw );
			if (m->hrfmsfh) g.x -= g.s * m->hrfmsfh;
		}

		if (m->taht)
		msfoof_clear_area(&m->mtrx, 0, &g);
		ihaatah_ws3(&m->darar, g.x, g.y, g.w, g.h);
		
		kaatib_darar_hwl(m, 0);
	}
}
void kaatib_hadhaf_kul(kaatib *m) { // delete all
	XATAA && printf("kaatib_hadhaf %ld\n", m->length);
	if (m->length && m->cursor > -1) {
		if (m->barzax != NULL) {
			XATAA && printf("kaatib_hadhaf_kul try free %p\n", m->barzax);
			free(m->barzax);
			if (m->barzax == NULL) perror("kaatib_hadhaf_kul free");
			else XATAA && printf("kaatib_hadhaf_kul freed %p\n", m->barzax);
			m->barzax = NULL;
		}
		m->cursor = -1;
		m->length = 0;
		ihaatah_nazaf(&m->muntahaa); // reset scroll bounds
	}
}
void kaatib_hadhaf(kaatib *m, char qaadim) {
	XATAA && printf("kaatib_hadhaf %ld\n", m->length);
	if (m->length && m->cursor > -1) {
		harf *h = &m->barzax[m->cursor];
		int len = strlen(h->harakaat);
		if (len == 0) { // special case, remove harakaat one by one
			if (m->cursor < m->length) {
				kaatib_darar(m, m->cursor);
			}
			// darar cases
			for (int i = m->cursor+1; i < m->length && m->barzax[i].id != '\n'; ++i) {
				kaatib_darar(m, i);
				m->barzax[i].mutadarrar = 1;
			}
			// move huroof by 1 harf at m->cursor
			for (int i = m->cursor+1; i < m->length; ++i) {
//				kaatib_darar(m, i);
				m->barzax[i-1] = m->barzax[i];
				if (h->id == '\n') { // because all lines will be shifted
					m->barzax[i-1].mutadarrar = 1;
				}
			}
			m->length--;
			/*if (m->length) {
				long harfsize = sizeof(m->barzax[m->cursor]);
				m->barzax = (harf *) realloc( m->barzax, m->length*harfsize );
				if (m->barzax == NULL) perror("kaatib_hadhaf realloc");
//				else printf("kaatib_hadhaf realloced %ld\n", m->length);
			} else if (m->barzax != NULL) {
				free(m->barzax);
				if (m->barzax == NULL) perror("kaatib_hadhaf free");
				else XATAA && printf("kaatib_hadhaf freed %p\n", m->barzax);
				m->barzax = NULL;
			}*/
			if (m->cursor > -1 && !qaadim) m->cursor--;

			ihaatah_nazaf(&m->muntahaa); // reset scroll bounds

		} else {
			h->harakaat[len-1] = 0;
			h->mutadarrar = 1;
		}
		if (m->cursor < -1) m->cursor = -1;
		if (m->cursor > m->length-1) m->cursor = m->length-1;
		if (m->barzax != NULL && m->cursor > -1)
			m->barzax[m->cursor].mutadarrar = 1;
	}
}
void kaatib_adaaf( kaatib *m, u_char c ) {
//	printf("kaatib_adaaf %d\n", c);
	if ( !xat_harakah(c) ) {
		m->length++;
//		m->barzax = (harf *) realloc( m->barzax, m->length*sizeof(harf) );
//		if (m->barzax == NULL) perror("kaatib_adaaf recalloc");

		int newsize = m->length*sizeof(harf);
		if (newsize > m->ma3looq) {
//			printf("kaatib size changing %p %d -> %d\n", m->barzax, m->ma3looq, newsize);
			harf *tmp = (harf *) realloc( m->barzax, newsize );
			if (tmp == NULL) perror("kaatib_adaaf realloc");
			else {
				m->barzax = tmp;
			}
//			printf("kaatib size changed %d -> %d\n", m->ma3looq, newsize);
			m->ma3looq = newsize;
		}

//		else printf("kaatib_adaaf realloced %p\n", m->barzax);
		if (m->cursor > -1) { // darar prev harf
			m->barzax[m->cursor].mutadarrar = 1;
		}
		m->cursor++;
		for (int i = m->length-1; i > m->cursor; --i) {
			m->barzax[i] = m->barzax[i-1];
		}

		kaatib_harf(&m->barzax[ m->cursor ]);

		// darar region
		if (c != '\n')
		for (int i = m->cursor; i < m->length && m->barzax[i].id != '\n'; ++i) {
			m->barzax[i].mutadarrar = 1;
		}
		else
		for (int i = m->length-1; i > m->cursor; --i) {
			kaatib_darar(m, i);
			m->barzax[i].mutadarrar = 1;
		}

		m->barzax[ m->cursor ].id = c;
		m->barzax[ m->cursor ].mutadarrar = 1;
		memset( m->barzax[ m->cursor ].harakaat, 0, 4 );
	} else {
		for (int k = 0; k < 3; ++k) {
			if (m->barzax[ m->cursor ].harakaat[k] == 0) {
				m->barzax[ m->cursor ].harakaat[k] = c;
				m->barzax[ m->cursor ].mutadarrar = 1;
				break;
			}
		}
	}
	if (m->cursor < 0) m->cursor = 0;
	if (m->cursor > m->length-1) m->cursor = m->length-1;
}
void kaatib_baddal( kaatib *m, u_char *str ) {
	m->length = 0;
	for (int k = 0; str[k] != '\0' && k < strlen(str); ++k) {
		if ( !xat_harakah(str[k]) ) ++m->length;
	}
//	kaatib_destroy(m);
	int newsize = m->length*sizeof(harf);
	if (newsize > m->ma3looq) {
		harf *tmp = (harf *) realloc( m->barzax, newsize );
		if (tmp == NULL) perror("kaatib_baddal realloc");
		else m->barzax = tmp;
//		printf("kaatib size changed %d -> %d\n", m->ma3looq, newsize);
		m->ma3looq = newsize;
		// TEST temply enabled this, valgrind was saying uninit'd value
		memset(m->barzax, 0, newsize );
	}
	int cursor = -1, harakah = 0;
	for (int k = 0; str[k] != '\0' && k < strlen(str); ++k) {
		if ( !xat_harakah(str[k]) ) {
			cursor++;
			// initial to avoid garbage
			kaatib_harf(&m->barzax[cursor]);

			m->barzax[cursor].id = str[k];
			m->barzax[cursor].mutadarrar = 1;
			m->barzax[cursor].muntaxab = 0;
			m->barzax[cursor].wazan = 0;

			harakah = 0;
		} else {
			if (harakah < 3) {
				m->barzax[cursor].harakaat[harakah] = str[k];
			}
			harakah++;
		}
	}
}
void kaatib_tasteer( kaatib *m, msfoof *msff, u_char *str ) {
	if (str != NULL) {
		kaatib_baddal(m, str);
//		free(str);
	}
}
char kaatib_mutadarrar(kaatib *m) { // is damaged
	if (m->n.c) {
		float oy = m->n.y, oyh = m->n.cy;
		if (oyh != oy) return 1;
	}
	return 0;
}
char kaatib_up(kaatib *m) {
	for (int i = m->cursor-1; i > -2; --i) {
		/* EXPLAIN
		 * -1 is first to avoid an invalid read from barzax
		 * */
		if (i == -1 || m->barzax[i].id == '\n') {
			kaatib_darar_hwl(m, m->cursor);
			m->cursor = i;
			kaatib_darar_hwl(m, m->cursor);
			kaatib_muashshar(m);
			return 1;
		}
	}
	return 0;
}
char kaatib_down(kaatib *m) {
	if (m->cursor == -1 && m->length)
		kaatib_darar(m, -1);

	for (int i = m->cursor+1; i < m->length; ++i) {
		if (m->barzax[i].id == '\n') {
			kaatib_darar_hwl(m, m->cursor);
			m->cursor = i;
			kaatib_darar_hwl(m, m->cursor);
			kaatib_muashshar(m);
			return 1;
		}
	}
	return 0;
}
char kaatib_pageup(kaatib *m) {
	int w = m->muntahaa.w, h = m->muntahaa.h, max, step = 128;
	float mw = m->g.w, mh = m->g.h, oy = m->n.y;
	char yes = 0;

	max = h > 0 ? h-mh : mh;

	if (oy > 0) oy -= step, yes = 1;
	
	if (h < mh) oy = 0;
	else if (oy > max) oy = max;
	else if (oy < 0) oy = 0;

	if (m->n.c && m->n.cy != oy)
		m->n.cy = oy, yes = 1;
	else if (m->n.cy != oy)
		m->n.y = oy, yes = 1;
	
	return yes;
}
char kaatib_pagedown(kaatib *m) {
	int w = m->muntahaa.w, h = m->muntahaa.h, max, step = 128;
	float mw = m->g.x+m->g.w, mh = m->g.h, oy = m->n.y;
	char yes = 0;

	max = (h > 0 ? h-mh : mh);

//	if (oy <= max)
	oy += step;
	
	if (h < mh) oy = 0;
	else if (oy > max) oy = max;
	else if (oy < 0) oy = 0;

	if (m->n.c && m->n.cy != oy)
		m->n.cy = oy, yes = 1;
	else if (m->n.cy != oy)
		m->n.y = oy, yes = 1;
	
	return yes;
}
char kaatib_home(kaatib *m) {
//	if (!m->ltr)
//		return kaatib_end(m);

	if (!m->barzax) return 0;

	harf *h = &m->barzax[m->cursor];
	if (h && h->id == '\n') return 0;
	
	for (int i = m->cursor-1; i > -2; --i) {
		if (m->barzax[i].id == '\n' || i == -1) {
			kaatib_darar_hwl(m, m->cursor);
			m->cursor = i;
			kaatib_darar_hwl(m, m->cursor);
			kaatib_muashshar(m);
			return 1;
		}
	}
	return 0;
}
char kaatib_end(kaatib *m) {
//	if (!m->ltr)
//		return kaatib_home(m);

	if (m->cursor == -1 && m->length)
		kaatib_darar(m, -1);

	for (int i = m->cursor+1; i < m->length; ++i) {
		if (m->barzax[i].id == '\n' || i == m->length-1) {
			kaatib_darar_hwl(m, m->cursor);
			m->cursor = i == m->length-1 ? i : i-1;
			kaatib_darar_hwl(m, m->cursor);
			kaatib_muashshar(m);
			return 1;
		}
	}
	return 0;
}
void kaatib_prev_o(kaatib *m) {
	long i = m->cursor;
	if (i-1 > -1 && i < m->length) {
		m->barzax[ i-1 ].mutadarrar = CLEAR_PRINT;
	}
	if (i+1 > -1 && i < m->length) {
		kaatib_darar_hwl(m, m->cursor);
		m->cursor--;
		kaatib_darar_hwl(m, m->cursor);
		kaatib_muashshar(m);
	}
}
void kaatib_next_o(kaatib *m) {
	long i = m->cursor;
	if (i+1 < m->length-1) {
		m->barzax[ i+1 ].mutadarrar = CLEAR_PRINT;
	}
	if (i+1 > -1 && i+1 < m->length) {
		kaatib_darar_hwl(m, m->cursor);
		m->cursor++;
		kaatib_darar_hwl(m, m->cursor);
		kaatib_muashshar(m);
	}
}
char kaatib_right(kaatib *m) {
	if (m->ltr) { if (m->cursor < m->length-1) { kaatib_next_o(m); return 1; }
	} else if (m->cursor > -1) { kaatib_prev_o(m); return 1; }
	return 0;
}
char kaatib_left(kaatib *m) {
	if (m->ltr) { if (m->cursor > -1) { kaatib_prev_o(m); return 1; }
	} else if (m->cursor < m->length-1) { kaatib_next_o(m); return 1; }
	return 0;
}
char kaatib_muntaxab(kaatib *m) { // dumb toggle selection
	int i = m->cursor;
	if (i > -1 && i < m->length) {
		m->barzax[ i ].muntaxab = !m->barzax[ i ].muntaxab;
//		m->barzax[ i ].muntaxab = 1;
		m->barzax[ i ].mutadarrar = 1;
		return 1;
	}
	return 0;
}
char kaatib_muntaxab_jaanibi(kaatib *m, char jaanib) { // -1 left +1 right
	int i = m->cursor;
	if (i > -1 && i < m->length) {
		if (!m->ltr) {
			if (jaanib == -1) jaanib =  1;
			else if (jaanib ==  1) jaanib = -1;
		}
		/* ALGO
		 * no selection -1 -1, started selection
		 * if no start point x, then set it
		 * if start point x is set, then ...
		 * */
		if (m->muntaxab == -1) {
			m->muntaxab = i;
			m->barzax[ i ].muntaxab = 1;
		} else {
			if (jaanib == -1)
				m->barzax[ i ].muntaxab = i <= m->muntaxab ? 1 : 0;
			else
			if (jaanib ==  1)
				m->barzax[ i ].muntaxab = i >= m->muntaxab ? 1 : 0;
		}
		m->barzax[ i ].mutadarrar = 1;
		return 1;
	}
	return 0;
}
void kaatib_muntaxab_tamaam(kaatib *m) {
	m->muntaxab = -1;
}
char kaatib_muntaxab_kul(kaatib *m) {
	char yes = 0;
	for (int i = 0; i < m->length; ++i) {
		if (m->barzax[i].muntaxab == 0) {
			m->barzax[i].muntaxab = 1;
			m->barzax[i].mutadarrar = 1;
			yes = 1;
		}
	}
	m->muntaxab = 0;
	if (yes) return 1;
	return 0;
}
char kaatib_ghayr_muntaxab_kul(kaatib *m) {
	char yes = 0;
	for (int i = 0; i < m->length; ++i) {
		if (m->barzax[i].muntaxab == 1) {
			m->barzax[i].muntaxab = 0;
			m->barzax[i].mutadarrar = 1;
			yes = 1;
		}
	}
	kaatib_muntaxab_tamaam(m);
	if (yes) return 1;
	return 0;
}
char kaatib_wazan( kaatib *m ) {
	char yes = 0, newstate = -1;
	for (int i = 0; i < m->length; ++i) {
		if (m->barzax[i].muntaxab == 1) {
			if (newstate == -1) newstate = !m->barzax[i].wazan;
			m->barzax[i].wazan = newstate;
			m->barzax[i].mutadarrar = 1;
			yes = 1;
		}
	}
	if (yes) return 1;
	return 0;
}
void kaatib_harf_hsb( kaatib *m, int i, char ltr ) { // calc harf ihaatah
	
}
int kaatib_harf_zaahir( kaatib *m, int i ) {
	if (i < m->length && m->barzax) {
		harf *h = &m->barzax[i];
		return isinside(h->ihth.x, h->ihth.y, &m->gw);
	} else return 0;
}
int kaatib_zaahir( kaatib *m, harf *h ) {
	return isinside(h->ihth.x, h->ihth.y, &m->gw);
}
harf * kaatib_cu3aa3( kaatib *m, float x, float y ) {
	harf *h;
	harf *str = m->barzax;
	if (str && m->length)
	for (int k = 0; k < m->length && str[k].id != '\0'; ++k) {
		h = &str[k];
		if (isinsidef(x, y, &h->ihth)) {
			return h;
		}
	}
	return NULL;
}
void kaatib_huroof( kaatib *m ) {
	float harfw = xathajm.x, harfh = xathajm.h,
		  ox = m->n.x,
		  oy = m->n.y, astep = 0.1, muddah = 2.0,
		  oyh = m->n.cy; // scroll offset hadaf

	if (oyh != oy && m->n.c) { // scrolling animation logic
		if (oyh > oy) astep = (oyh-oy) / muddah;
		else if (oyh < oy) astep = (oy-oyh) / muddah;
		else astep = 1;
		if (astep < .25) astep = .25; // to avoid infinite loop

		if (oyh > oy) {
			oy += astep;
			if (oy > oyh) oy = oyh;
		}
		else if (oyh < oy) {
			oy -= astep;
			if (oy < oyh) oy = oyh;
		}
		m->n.y = oy;
	}

	if (m->hrfmsfh) { // letter space
		harfw -= m->g.s * m->hrfmsfh;
	}
	ihaatahf g = { 0, 0, harfw, harfh, m->g.s };

	int col = 0, row = 0,
		cols			= (int) m->g.w / harfw, // for monospace word wrapping
		rows			= (int) m->g.h / harfh,
		start			= 0,
		curlineh		= 0, // height
		lastlineheight	= xathajm.h; // height
	
	char ltr = 1, yes = 0;
	u_char c, xc, nc;
	
	harf *str = m->barzax;
	if (m->barzax && m->length)
	for (int k = 0; k < m->length && str[k].id != '\0'; ++k) {
		yes = 0;
		c = str[k].id;
		xc = nc = -1;
		if (k) xc = str[k-1].id;
		if (k < m->length-1) nc = str[k+1].id;

		if (!mono) {
			kaatib_harf_hajm(m, c);
			g.w = harfw = ahjaam[c].x - m->hrfmsfh;
			g.h = harfh = ahjaam[c].h;
			if (curlineh < ahjaam[c].h) {
				curlineh = ahjaam[c].h;
			}
		}

		if (c == '\n') {
			col = -1; start = -harfw + m->hrfmsfh; row++;
//			lastlineheight += curlineh;
			lastlineheight += xathajm.h;
		} else if (mono) {
			if (col >= cols) { col = 0; start = 0; row++; }
		} else {
			if (start+harfw >= (int) m->g.w) {
				col = 0; start = 0; row++;
//				lastlineheight += curlineh;
				lastlineheight += xathajm.h;
			}
		}

		if ( c == '\n' || c == ' ' || c == '\t' || k == 0 ) {
			u_char tmpc = (c == '\n') ? nc : c;
			if ( xat_latin(tmpc) ) ltr = 1;
			if ( xat_arabic(tmpc) ) ltr = 0;
		}

		if (ltr) {
			if (mono) {
				g.x = m->g.x + (col * harfw) - m->hrfmsfh - ox;
			} else {
				g.x = m->g.x + (start) - m->hrfmsfh - ox;
			}
			if (m->hrfmsfh) g.x += g.s * m->hrfmsfh;
		}
		else {
			if (mono) {
				g.x = ( m->g.w - (col * harfw) - harfw ) - ox + m->g.x;
			} else {
				g.x = ( m->g.w - (start) - harfw ) - ox + m->g.x;
			}
			if (m->hrfmsfh) g.x -= g.s * m->hrfmsfh;
		}
		
		// invert certain characters < > ( ) { } [ ] ? ...
		if ( ltr && ( xat_3aks(c) || xat_arabic(c) ) ) g.i = 1;
		else if (!ltr && xat_latin(c)) g.i = 1;
		else g.i = 0;
		
		if (mono) {
			g.y = m->g.y + (row * harfh) - oy;
		} else {
			g.y = m->g.y + lastlineheight - oy;
		}
		
		
//		printf("%c %f x %f\n", c, g.w, g.h);
		char type = xat_3alaqa(&m->x, xc, c, nc);
		
		// only detect but don't copy new values over yet
		if (str[k].type   != type
		||  str[k].ltr    != ltr
		||  str[k].ihth.x != g.x
		||  str[k].ihth.y != g.y
		||  str[k].ihth.w != g.w
		||  str[k].ihth.h != g.h
		||  str[k].ihth.s != g.s
		||  str[k].ihth.i != g.i ) { yes = 1; }

		if (yes && kaatib_zaahir(m, &str[k])) { // to clear the current location
			kaatib_darar(m, k);
			str[k].mutadarrar = 1;
		}

		// copy new values over now
		if (str[k].type   != type) { str[k].type   = type; }
		if (str[k].ltr    != ltr ) { str[k].ltr    = ltr;  }

		ihaatahf_nasax(&str[k].aqil, &g);

		g.y -= xathajm.s+xathajm.w;
		g.h  = xathajm.s+xathajm.w;

		ihaatahf_nasax(&str[k].ihth, &g);
		
		// extend scroll bounds
		ihaatah_ws3(&m->muntahaa, 0, 0, g.x+g.w+ox-m->g.x, g.y+g.h+oy-m->g.y);

		col++;
		
		if (!mono) start += harfw;
	}

}
void kaatib_itlaq_hudood( kaatib *m ) {
	float mh = m->g.h, oy = m->n.y;
	int h = m->muntahaa.h, max = h > 0 ? h-mh : mh;
	if (h < mh) oy = 0; else if (oy > max) oy = max;
	m->n.y = oy;
	m->n.cy = oy;
}
void kaatib_muashshar( kaatib *m ) { // scroll to cursor
	long i = m->cursor;
	if (i >= -1 && i < m->length) {
		float oy = m->n.y; char yes = 0;
		if (i > -1) {
			ihaatahf g = m->barzax[i].ihth;
			if (g.y+g.h > (m->g.y+m->g.h)) {
				oy = oy + g.y - m->g.h + g.h - m->g.y;
				yes = 1;
			} else
			if (g.y < m->g.y) {
				oy = oy + (g.y) - m->g.y;
				yes = 1;
			}
		} else {
			oy = 0;
			yes = 1;
		}
		if (yes && oy >= 0) {
			if (m->n.c) m->n.cy = oy;
			else m->n.y = oy;
		}
	}
}
void kaatib_idhan_ikraah( kaatib *m, msfoof msff ) {
	kaatib_idhan(m, msff, 1);
}
void kaatib_idhan( kaatib *m, msfoof msff, char ikraah ) {
	if ( m->mst3d != 1 ) {
		printf("kaatib_draw: kaatib not initialized\n"); return;
	} else {
		XATAA && printf("kaatib_idhan %d\n", ikraah);
	}

	ihaatahf2_nasax(&msff.maqtoo3, &m->g);
	
	XATAA > 2 && printf("kaatib_draw %s\n", m->mtn.barzax);
	
	ihaatahf g = { 0, 0, 0, 0, m->g.s };
//	ihaatah_nazaf(&m->darar);

	int shadow = 0xaaaaaa, color = 0xffffff, selcolor = 0x006699, selstart = 0;
	u_char c = 0;

	if (ikraah != 2) // so you can call this indie
		kaatib_huroof( m ); // apply huroof changes, calc locations etc

	if (m->n.cy != m->n.y) {
//	msfoof_rect(&msff, m->g.x, m->g.y, 8, m->g.h, -1, 0); // clear scrollbar
		// TODO report darar so everything can be redraw underneath the scrollb
		// this can only work if we have canvas working a z-index
//		if (m->qmc->darar) m->qmc->darar(m->darar);
	}

	harf *str = m->barzax;
	float smooth = msff.strokew, sw = msff.strokew,
		  harfw = xathajm.x, harfh = xathajm.h;

	/* TODO xat - canvas
	 * it should set the xat_* here for each harf
	 * and each kaatib_idhan can be wrapped in a canvas.[save/restore] :)
	 * */

	msff.smooth = 0;
	msff.strokew = 1;
	if (str != NULL && m->length) {
		for (int k = 0; k < m->length && str[k].id != '\0'; ++k) {
//			if (k+1 == m->length) printf("l1 %d\n", k);

			c = str[k].id;
			g = str[k].ihth;
//			g.w = harfw;
			g.h = harfh;

			if ( (ikraah || str[k].mutadarrar) && kaatib_zaahir(m, &str[k]) ) {
				if (m->taht)
				if (str[k].mutadarrar == CLEAR_PRINT) {
//				if (str[k].mutadarrar == PRINT_ONLY) {
					msfoof_clear_area_f(&msff, 0, &g);
					if (str[k].muntaxab) {
						if (selstyle == TAHT) {
							msfoof_satar(&msff, g.x, g.y+g.h-6, g.x+g.w, g.y+g.h-6, selcolor);
						} else if (selstyle == MUSTATEEL) {
							msfoof_clear_area_f(&msff, selcolor, &g);
						} else {
							if (selstart) {
							} else {
								msfoof_line(&msff, g.x, g.y+3, g.x, g.y+g.h-3, selcolor);
								selstart = 1;
							}
							msfoof_line(&msff, g.x, g.y+3    , g.x+g.w, g.y+3    , selcolor);
							msfoof_line(&msff, g.x, g.y+g.h-3, g.x+g.w, g.y+g.h-3, selcolor);
						}
					} else if (selstart) { // sel ended
						if (selstyle == SATAR) {
							selstart = 0;
							if (k && c == '\n') {
								g.x = str[k-1].ihth.x+g.w-3; g.y = str[k-1].ihth.y;
							}
							msfoof_line(&msff, g.x+5, g.y+3, g.x+5, g.y+g.h-3, selcolor);
						}
					}
					ihaatah_ws3(&m->darar, g.x, g.y, g.w, g.h);
				}
			}
		}
		for (int k = 0; k < m->length && str[k].id != '\0'; ++k) {
			// TODO func look ahead & get line direction
			c = str[k].id;
			g = str[k].ihth;

			if (m->showcursor)
			if (m->cursor == k) {
				m->ltr = str[k].ltr;
				rendercursor(m, g, 221);
			}

			if ( (ikraah || str[k].mutadarrar) && kaatib_zaahir(m, &str[k]) ) {
				if (str[k].muntaxab)
					renderselection(m, g);

				if (c == ' ' || c == '\n') color = 0x333333;
				else if (c == '\t' || c == 221) color = 0x666666;
				else color = 0xffffff;

				rendercharat(m, g, c);

				g.y += str[k].aqil.h-(xathajm.w);
				for (int j = 0; j < 4 && str[k].harakaat[j] != '\0'; ++j) {
					rendercharat(m, g, str[k].harakaat[j]);
				}
				str[k].mutadarrar = 0;
			}
		}
	}

	if (m->showcursor && m->n.y <= harfh)
	if (m->length == 0 || m->cursor == -1) { // show cursor anyway at -1
		ihaatahf_nazaf(&g);
		char ltr = 1;
		g.w = ahjaam['\n'].w;
		g.h = xathajm.h;

		if (m->length) {
			c = str[0].id;
			if ( xat_latin(c) ) ltr = 1;
			else if ( xat_arabic(c) ) ltr = 0;
		}

		g.y = m->g.y - m->n.y;
		if (ltr) {
			g.x = m->g.x - g.w;
		}
		else {
			g.x = ( m->g.w ) + m->g.x;
		}
		rendercursor(m, g, 221);
		ihaatah_ws3(&m->darar, g.x, g.y, g.w, g.h);
	}

	float bar = m->g.h/m->muntahaa.h;
	if (m->n.cy != m->n.y)
	if (bar < 1) {
		float pct = m->n.y/m->muntahaa.h;

		g.x = m->g.x+2; g.y = m->g.y+(m->g.h*pct); g.w = 6; g.h = m->g.h*bar;

		ihaatah_ws3(&m->darar, g.x, g.y, g.w, g.h);
		if (m->qmc) {
			if (m->qmc->mustateel) m->qmc->mustateel(g);
			if (m->qmc->lwn_rgb) m->qmc->lwn_rgb(66, 66, 66);
			if (m->qmc->stroke) m->qmc->stroke(1);
			if (m->qmc->lwn_rgb) m->qmc->lwn_rgb(99, 99, 99);
			if (m->qmc->fill) m->qmc->fill(0);
			if (m->qmc->renew_path) m->qmc->renew_path();
			if (m->qmc->darar) m->qmc->darar(m->darar);
		}
	}

	ihaatah_nazaf(&msff.maqtoo3);
}
void kaatib_draw( kaatib *m ) {
	kaatib_idhan( m, m->mtrx, 0 );
}
void kaatib_xataa( kaatib *m ) {
	// i learned that to modify a struct arg, pass it as pointer
	float harfw, harfh;
//	if (m->hrfmsfh) { harfw -= m->g.s * 2; harfh -= m->g.s * 2; }

	harfw = xathajm.x;
	harfh = xathajm.h;

	m->gw.x = m->g.x-harfw;
	m->gw.y = m->g.y-harfh;
	m->gw.w = m->g.w+harfw;
	m->gw.h = m->g.h+harfh;

	ihaatah_nazaf(&m->muntahaa); // reset scroll bounds

	XATAA > 1 && printf("xat w%f x h%f\n", harfw, harfh);
}
void kaatib_qadd( kaatib *m, float qadd ) {
	XATAA && printf("kaatib_qadd %f\n", qadd);
	m->g.s = qadd;
	
	// TODO this too should be wrapped in canvas.[save/restore]
	if (m->qmc && m->qmc->font_size) {
		m->qmc->font_size(m->g.s);
	}

	if (m->qmc && m->qmc->font_bounds) {
		m->qmc->font_bounds(&xathajm);
	}

	for (int i = 0; i < 256; ++i) {
		kaatib_harf_hajm( m, i );
	}

	kaatib_xataa( m );
}
void kaatib_init( kaatib *m ) {
	XATAA > 1 && printf("kaatib_init\n");

	// does matn->huroof, you destroy the matn outside of this
	kaatib_destroy( m ); // make sure older resources are free
	
	if (!xatinit) {
		xatinit = 1;
		xat2022 = (xat) { 0 };
		xat_init(&xat2022, "./data/2022.xat");
	}
	m->x = xat2022;
	m->mst3d = 1;

	ihaatah_nazaf(&m->muntahaa);
	ihaatah_nazaf(&m->darar);

	m->muntaxab = -1;
	m->n.x = 0; m->n.y = 0;
//	m->n.c = 1;
	m->cursor = -1;
	m->taht = 0;
	m->length = 0;
	m->barzax = NULL;
	
	if (m->mtn.barzax) {
		kaatib_baddal(m, m->mtn.barzax);
	}
}

