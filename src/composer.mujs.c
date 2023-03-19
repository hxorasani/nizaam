/* composer
 * 
 * z-indexed scene-graph API for nizaam apps
 * 
 * contains eanaasir (elements)
 * 
 * an element is a module that only contains logic and canvas calls
 * its unaware of where it is placed, that info is handled by composer itself
 * this means that any drawing calls it does can be overriden by transforms
 * without the element being aware of it
 * 
 * */

#include "composer.h"

#define XATAA 0
#define QADRTAARAH 40

kaatib ktb;
eqonah eqnh;
composer trkb;
js_State *J;
polygonf poly;

int use_canvas = 1, ayyi = RAEES;

cairo_surface_t *surfaceraees = NULL;
cairo_t *cairaees = NULL;
cairo_surface_t *surfacemshr = NULL;
cairo_t *caimshr = NULL;
cairo_t *syq = NULL; // current cairo context for currently rendering wijhah

canvas qmc = { 0 };

/*//{ xat driver
void matn_hajm(ihaatahf *ih, int c) {
	if (!ih) return;
	ih.x = (m->x.w*m->g.s) - (m->g.s*m->hrfmsfh) ;
	ih.w = (m->x.w*m->g.s) - (m->g.s*m->hrfmsfh) ;
	ih.h = m->x.h*m->g.s;
}
void matn_harf() {
	eqonah_sattar(eqnh, msff, &g, color);

	eqonah eqnh = m->x.eqonaat[ eqonah_index(str[k].harakaat[j], HARAKAH) ];
	eqonah_sattar(eqnh, msff, &g, 0x888888);

	eqnh = m->x.eqonaat[ eqonah_index(c, str[k].type) ];
	if (str[k].wazan == 1) {
		if (!(c == '\n' || c == '\t' || c == ' ')) {
			g.y--;
			eqonah_sattar(eqnh, msff, &g, shadow);
			g.y += 2;
			eqonah_sattar(eqnh, msff, &g, shadow);
			g.y--;

			g.x--;
			eqonah_sattar(eqnh, msff, &g, shadow);
			g.x += 2;
			eqonah_sattar(eqnh, msff, &g, shadow);
			g.x--;
		}
	}
}
//}*/

/*/{ freetype driver
void font_size(float qadd) {
	if (!face) return;
	
	if (FT_Set_Pixel_Sizes(face, 0, qadd))
		errexit("freetype set pixel sizes");
}
void matn_hajm(ihaatahf *ih, int c) {
	if (!ih || !face) return;
	int error = 0;
	unsigned int glyph_index = FT_Get_Char_Index( face, (unsigned long) charcode );

	error = FT_Load_Glyph(face, glyph_index, 0 );
	if (error) errexit("freetype load glyph");
	
	FT_GlyphSlot slot = face->glyph;
	
	ahjaam[charcode].w = slot->advance.x >> 6;

	error = FT_Load_Glyph(face, glyph_index, 0 );
	if (error) errexit("freetype load glyph");
	
//	if (slot->format != FT_GLYPH_FORMAT_BITMAP) {
////		if (smooth)
//		error = FT_Render_Glyph( slot, FT_RENDER_MODE_NORMAL );
////		else
////		error = FT_Render_Glyph( slot, FT_RENDER_MODE_MONO );
//		if (error) errexit("freetype render glyph");
//	}

	ahjaam[charcode].h = slot->metrics.height >> 6;
//	ahjaam[charcode].h = slot->metrics.horiBearingY >> 6;
	ahjaam[charcode].y = face->descender>>6;
	ahjaam[charcode].h = (face->ascender>>6) - (face->descender>>6);
}
void matn_harf() {
	rendercharcode(&msff, &g, fushatarmeez.tounicode[c], color);
	rendercharcode(&msff, &g, fushatarmeez.tounicode[ str[k].harakaat[j] ], 0x888888);
}
//}*/

//{ canvas/cairo bridge/driver/wrapper functions
// these can be wrapped with a tmp var like axav_msfoof
// TODO cairo_mask (cairo_t *cr, cairo_pattern_t *pattern);
void font_bounds(ihaatahf *ih) {
	if (!ih || !cairaees) return;

	cairo_font_extents_t fe;
	cairo_font_extents (cairaees, &fe);

	ih->x = fe.max_x_advance;
	ih->y = fe.max_y_advance;
	ih->s = fe.ascent;
	ih->w = fe.descent;
	ih->h = fe.height;
}
void harf_hajm(ihaatahf *ih, int c) {
	if (!ih || !cairaees) return;

	cairo_text_extents_t te;
	char str[4]; // not doing this conversion bugs cairo :/
	if (uni2chars(str, fushatarmeez.tounicode[c], 4)) {
		cairo_text_extents (cairaees, str, &te);

		ih->x = te.x_advance;
		ih->s = te.x_bearing;
		ih->y = te.y_bearing;
		ih->w = te.width;
		ih->h = te.height;
	}
}
void huroof_hajm(ihaatahf *ih, int *c) {
	if (!ih || !cairaees) return;

	int len = wcslen(c);
	for (int i = 0; i < len; ++i) {
		if (c[i] < 256)
		c[i] = fushatarmeez.tounicode[ c[i] ];
	}

	char *str = calloc(len*4, sizeof(char));
	if (str == NULL) {
		perror("huroof_hajm calloc"); exit(EXIT_FAILURE);
	}

	cairo_text_extents_t te;
	if (unis2chars(str, c, len)) {
		cairo_text_extents (cairaees, str, &te);
//		printf("%s %f\n", str, te.height);

		ih->x = te.x_advance;
		ih->y = te.y_bearing;
		ih->w = te.width;
		ih->h = te.height;
	}

	free(str);
}
void matn_harf(int c) {
	if (!cairaees) return;

	char str[4];
	if (uni2chars(str, fushatarmeez.tounicode[c], 4))
		cairo_show_text(cairaees, str);
}
void matn_huroof(int *c) {
	if (!cairaees) return;

	int len = wcslen(c);
	for (int i = 0; i < len; ++i) {
		if (c[i] < 256)
		c[i] = fushatarmeez.tounicode[ c[i] ];
	}

	char *str = calloc(len*4, sizeof(char));
	if (str == NULL) {
		perror("matn_huroof calloc"); exit(EXIT_FAILURE);
	}

	if (unis2chars(str, c, len))
		cairo_show_text(cairaees, str);
	
	free(str);
}
void xat_wijhah(const char *c) { // add BOLD, ITALIC
	if (!c || !cairaees) return;

	cairo_select_font_face(cairaees, c, 0, 0);
}
void font_size(float qadd) {
	if (!cairaees) return;
	
	cairo_set_font_size(cairaees, qadd);
}
void mustateel(ihaatahf ih) {
	if (!cairaees) return;
	
	cairo_rectangle(cairaees, ih.x, ih.y, ih.w, ih.h);
}
void move_to(float x, float y) {
	if (!cairaees) return;
	
	cairo_move_to(cairaees, x, y);
}
void satar(float x, float y) {
	if (!cairaees) return;
	
	cairo_line_to(cairaees, x, y);
}
void stroke(char preserve) {
	if (!cairaees) return;
	
	if (preserve) cairo_stroke_preserve(cairaees);
	else cairo_stroke(cairaees);
}
void fill(char preserve) {
	if (!cairaees) return;
	
	if (preserve) cairo_fill_preserve(cairaees);
	else cairo_fill(cairaees);
}
void iqta3() { // clip
	if (syq) cairo_clip (syq);
}
void renew_path() {
	if (syq) cairo_new_path (syq);
}
void lwn_rgb(float r, float g, float b) {
	if (!cairaees) return;
	
	cairo_set_source_rgb(cairaees, r, g, b);
}
void lwn_rgba(float r, float g, float b, float a) {
	if (!cairaees) return;
	
	cairo_set_source_rgba(cairaees, r, g, b, a);
}
//}

msfoof *axav_msfoof(int ayyi) {
	if (ayyi == MUSHEER) return &WJHH.mshr.mtrx;
	else if (ayyi == RAEES) return &WJHH.raees.mtrx;
	return NULL;
}
void cairo2msfoof(msfoof *m, cairo_surface_t *s, cairo_t *c) {
	if (!use_canvas) return;

	cairo_surface_flush(s);
    int stride = cairo_image_surface_get_stride (s);
	int width  = cairo_image_surface_get_width  (s);
    int height = cairo_image_surface_get_height (s);
    u_char *dst = cairo_image_surface_get_data  (s);
	u_char *pixel = 0;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			pixel = dst + y*stride + x*4;
//			msfoof_set_blend(m, x, y, argb32torgb( pixel ), 1.0f-( argb32_get_a(pixel) /255.0f));
			msfoof_set(m, x, y, argb32torgb( pixel ));
		}
	}
}

int bahac_tareeqah(js_State *J, const char *ism) { // find js function
	js_getglobal(J, ism); // if not found, it pushed undefined on stack
	
	if (js_isundefined(J, -1)) { // -1 means the last thing pushed on stack
		return -2;
	}
	
	return 0;
}
int qaal_tareeqah(js_State *J, int idx, const char *ism) { // call js function
	// call function and check for exceptions
	if (js_pcall(J, idx)) {
		printf("%s xataa %s fi %s%s\n", REDB, RST, ism, RST);
		// TODO show stack trace
		js_pop(J, 1);
		return -1;
	}

	// retrieve return value
	int result = js_tonumber(J, -1);
	js_pop(J, 1);

	return result;
}
void adaaf_tareeqah(js_State *J, const char *name, js_CFunction fun, int n) {
	const char *realname = strchr(name, '.');
	realname = realname ? realname + 1 : name;
	js_newcfunction(J, fun, name, n);
	js_defproperty(J, -2, realname, JS_READONLY | JS_DONTCONF);
}
void adaaf_wasf(js_State *J, const char *name) {
	js_defproperty(J, -2, name, JS_READONLY | JS_DONTCONF);
}
void adaaf_sift(js_State *J, const char *name) {
	js_defproperty(J, -2, name, JS_DONTCONF);
}
int composer_b_icaarah(mafateeh wm) {
	if (J == NULL) return 0;
	if (bahac_tareeqah(J, "b_icaarah")) return -2;

	// push arguments to function
	js_pushnull(J); // the 'this' object to use

	js_newobject(J);
	{
		if (wm.ctrl)
			js_pushnumber(J, wm.ctrl), adaaf_wasf(J, "ctrl");
		if (wm.shift)
			js_pushnumber(J, wm.shift), adaaf_wasf(J, "shift");
		if (wm.alt)
			js_pushnumber(J, wm.alt), adaaf_wasf(J, "alt");
		if (wm.meta)
			js_pushnumber(J, wm.meta), adaaf_wasf(J, "meta");
		if (wm.state)
			js_pushnumber(J, wm.state), adaaf_wasf(J, "haal");
		
		js_pushnumber(J, wm.x), adaaf_wasf(J, "x");
		js_pushnumber(J, wm.y), adaaf_wasf(J, "y");
		js_pushnumber(J, wm.w), adaaf_wasf(J, "w");
		js_pushnumber(J, wm.key), adaaf_wasf(J, "n"); // nuqaat / points / fingers

		js_pushstring(J, wm.ism); adaaf_wasf(J, "ism");
		js_pushstring(J, wm.huroof); adaaf_wasf(J, "harf");
	}
	
	return qaal_tareeqah(J, 1, "b_icaarah");
}
int composer_b_lowhah(mafateeh wm) {
	if (J == NULL) return 0;
	if (bahac_tareeqah(J, "b_lowhah")) return -2;

	// push arguments to function
	js_pushnull(J); // the 'this' object to use

	js_newobject(J);
	{
		if (wm.ctrl)
			js_pushnumber(J, wm.ctrl), adaaf_wasf(J, "ctrl");
		if (wm.shift)
			js_pushnumber(J, wm.shift), adaaf_wasf(J, "shift");
		if (wm.alt)
			js_pushnumber(J, wm.alt), adaaf_wasf(J, "alt");
		if (wm.meta)
			js_pushnumber(J, wm.meta), adaaf_wasf(J, "meta");
		if (wm.state)
			js_pushnumber(J, wm.state), adaaf_wasf(J, "haal");

		js_pushnumber(J, wm.key); adaaf_wasf(J, "key");

		js_pushstring(J, wm.ism); adaaf_wasf(J, "ism");

		int len = strlen(wm.huroof);
		int *src = calloc(len, sizeof(int));
		char *dest = calloc(len*4, sizeof(char));
		for (int i = 0; i < len; ++i) {
			src[i] = fushatarmeez.tounicode[wm.huroof[i]];
		}

		if (src == NULL || dest == NULL) {
			perror("composer_b_lowhah calloc"); exit(EXIT_FAILURE);
		}
		
		if (unis2chars(dest, src, len*4) > -1) {
			js_pushstring(J, dest); adaaf_wasf(J, "harf");
		}

		free(src);
		free(dest);
	}
	
	return qaal_tareeqah(J, 1, "b_lowhah");
}
int composer_on_paint_window() {
	if (J == NULL) return -2;
	if (bahac_tareeqah(J, "on_paint_window")) return -2;

	// push arguments to function
	js_pushnull(J); // the 'this' object to use
	
	return qaal_tareeqah(J, 0, "on_paint_window");
}
int composer_on_paint_indicator() {
	if (J == NULL) return -2;
	if (bahac_tareeqah(J, "on_paint_indicator")) return -2;

	// push arguments to function
	js_pushnull(J); // the 'this' object to use
	
	return qaal_tareeqah(J, 0, "on_paint_indicator");
}
int composer_b_hajm(int arg1) {
	if (J == NULL) return -2;
	if (bahac_tareeqah(J, "b_hajm")) return -2;

	// push arguments to function
	js_pushnull(J); // the 'this' object to use
	js_pushnumber(J, arg1);
	
	return qaal_tareeqah(J, 1, "b_hajm");
}

void composer_polygon_adaaf(js_State *J) {
	float x = js_tonumber(J, 1),
		  y = js_tonumber(J, 2);

	if (poly.length < 256) poly.length++;
	poly.nuqaat[poly.length].x = x;
	poly.nuqaat[poly.length].y = y;

	js_pushnumber(J, poly.length);
}
void composer_polygon_havaf(js_State *J) {
	if (poly.length > 0) poly.length--;

	js_pushnumber(J, poly.length);
}
void composer_polygon_ja3al(js_State *J) {
	int i = js_tonumber(J, 1);
	float x = js_tonumber(J, 2),
		  y = js_tonumber(J, 3);

	if (i < poly.length) {
		poly.nuqaat[i].x = x;
		poly.nuqaat[i].y = y;
	}

	js_pushundefined(J);
}
void composer_polygon_axav(js_State *J) {
	int i = js_tonumber(J, 1);

	if (i < poly.length) {
		js_newobject(J);
		{
			js_pushnumber(J, poly.nuqaat[i].x), adaaf_sift(J, "x");
			js_pushnumber(J, poly.nuqaat[i].y), adaaf_sift(J, "y");
		}
	} else {
		js_pushundefined(J);
	}
}
void composer_polygon_sattar(js_State *J) {
	int ayyi = js_tonumber(J, 1), // which
		o = js_tonumber(J, 2),
		l = js_tonumber(J, 3);

	msfoof *s;
		 if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	else if (ayyi == RAEES  ) s = &WJHH.raees.mtrx;

	if (s) msfoof_mudalla3(s, &poly, o, l);

	js_pushundefined(J);
}

//{ hisaab
void composer_hisaab_drj(js_State *J) {
	float z = js_tonumber(J, 1);

	js_pushnumber(J, todegrees(z));
}
void composer_hisaab_rad(js_State *J) {
	float z = js_tonumber(J, 1);

	js_pushnumber(J, toradians(z));
}
void composer_hisaab_niz(js_State *J) {
	float x0 = js_tonumber(J, 1),
		  y0 = js_tonumber(J, 2),
		  x1 = js_tonumber(J, 3),
		  y1 = js_tonumber(J, 4);

	js_pushnumber(J, coordtoangle(x0, y0, x1, y1));
}
void composer_hisaab_zin(js_State *J) {
	float x0 = js_tonumber(J, 1),
		  y0 = js_tonumber(J, 2),
		  r = js_tonumber(J, 3),
		  z = js_tonumber(J, 4);

	nuqtahf n;
	angletocoord(x0, y0, r, z, &n);
	
	js_newobject(J);
	{
		js_pushnumber(J, n.x), adaaf_sift(J, "x");
		js_pushnumber(J, n.y), adaaf_sift(J, "y");
	}
}
//}

//{ msfoof
void composer_msfoof_qadd(js_State *J) {
	float l = js_tonumber(J, 2);

	msfoof *s;
	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	else if (ayyi == RAEES) s = &WJHH.raees.mtrx;
	
	if (s && !isnan(l)) {
		if (l < .25) l = .25;
		else if (l > 10) l = 10;

		if (ayyi == MUSHEER) s->strokew = l;
		else if (ayyi == RAEES) s->strokew = l;
	}

	js_pushnumber(J, s->strokew);
}
void composer_msfoof_layyin(js_State *J) {
	char l = js_tonumber(J, 2); // na3am

	msfoof *s;
	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	else if (ayyi == RAEES) s = &WJHH.raees.mtrx;

	if (s != NULL) {
		if (l == 1 || l == 0) s->smooth = l;
		js_pushnumber(J, s->smooth);
	} else
		js_pushundefined(J);
}
void composer_msfoof_nazzaf(js_State *J) {
	int l = js_tonumber(J, 2); // lwn color

	if (ayyi == MUSHEER) msfoof_clear(&WJHH.mshr.mtrx, l);
	if (ayyi == RAEES) msfoof_clear(&WJHH.raees.mtrx, l);

	js_pushundefined(J);
}
void composer_msfoof_nazzaf_alif(js_State *J) {
	int l = js_tonumber(J, 2); // lwn color

	if (ayyi == MUSHEER) msfoof_clear_alif(&WJHH.mshr.mtrx, l);
	if (ayyi == RAEES) msfoof_clear_alif(&WJHH.raees.mtrx, l);

	js_pushundefined(J);
}
void composer_msfoof_ja3al_alif(js_State *J) {
	int x = js_tonumber(J, 2),
		y = js_tonumber(J, 3),
		l = js_tonumber(J, 4); // color

	msfoof *s;
	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) s = &WJHH.raees.mtrx;

	msfoof_set_alif(s, x, y, l);

	js_pushundefined(J);
}
void composer_msfoof_axav_alif(js_State *J) {
	int x = js_tonumber(J, 2),
		y = js_tonumber(J, 3);

	msfoof *s;
	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) s = &WJHH.raees.mtrx;

	js_pushnumber(J, msfoof_get_alif(s, x, y));
}
void composer_msfoof_axav_owst_alif(js_State *J) {
	js_pushnumber(J, 0);
}
void composer_matrix_print_alpha(js_State *J) {
	int l = js_tonumber(J, 2); // color

	msfoof *s;

	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) s = &WJHH.raees.mtrx;

	matrix_print_alpha(s, l);

	js_pushundefined(J);
}
void composer_msfoof_ikbar(js_State *J) {
	int ayyi  = js_tonumber(J, 1), // which
		hadaf = js_tonumber(J, 2), // target
		x     = js_tonumber(J, 3), // x
		y     = js_tonumber(J, 4); // y

	msfoof *s, *t;

	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) s = &WJHH.raees.mtrx;
	if (hadaf == MUSHEER) t = &WJHH.mshr.mtrx;
	if (hadaf == RAEES) t = &WJHH.raees.mtrx;

	ihaatah i = {0, 0, s->w, s->h};
	nuqtahf n = {0, 0, 0, x, y};
	msfoof_scale(s, t, &i, &n);

	js_pushundefined(J);
}
void composer_msfoof_ikbar_alif(js_State *J) {
	int ayyi = js_tonumber(J, 1), // which
		hadaf = js_tonumber(J, 2), // target
		l = js_tonumber(J, 3); // color

	msfoof *s, *t;

	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) s = &WJHH.raees.mtrx;
	if (hadaf == MUSHEER) t = &WJHH.mshr.mtrx;
	if (hadaf == RAEES) t = &WJHH.raees.mtrx;

	msfoof_ikbar_alif(s, t, l);

	js_pushundefined(J);
}
void composer_msfoof_satar(js_State *J) { // line
	int	l = js_tonumber(J, 6); // lwn color
	float	x0 = js_tonumber(J, 2),
			y0 = js_tonumber(J, 3),
			x1 = js_tonumber(J, 4),
			y1 = js_tonumber(J, 5);

	if (ayyi == MUSHEER) msfoof_satar(&WJHH.mshr.mtrx, x0, y0, x1, y1, l);
	if (ayyi == RAEES) msfoof_satar(&WJHH.raees.mtrx, x0, y0, x1, y1, l);

	js_pushundefined(J);
}
void composer_msfoof_satar_alif(js_State *J) { // line
	int	l = js_tonumber(J, 6), // lwn color
		tb3 = js_tonumber(J, 7);
	float	x0 = js_tonumber(J, 2),
			y0 = js_tonumber(J, 3),
			x1 = js_tonumber(J, 4),
			y1 = js_tonumber(J, 5);

	msfoof *m;
	if (ayyi == MUSHEER) m = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) m = &WJHH.raees.mtrx;

	if (m) msfoof_satar_alif(m, x0, y0, x1, y1, l, tb3, NULL);

	js_pushundefined(J);
}
void composer_msfoof_line_alif(js_State *J) { // line
	int	l = js_tonumber(J, 6), // lwn color
		tb3 = js_tonumber(J, 7);
	float	x0 = js_tonumber(J, 2),
			y0 = js_tonumber(J, 3),
			x1 = js_tonumber(J, 4),
			y1 = js_tonumber(J, 5);

	msfoof *m;
	if (ayyi == MUSHEER) m = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) m = &WJHH.raees.mtrx;

	if (m) msfoof_line_alif(m, x0, y0, x1, y1, l);

	js_pushundefined(J);
}
void composer_matrix_circle(js_State *J) { // circle
	int	fg = js_tonumber(J, 7),
		l = js_tonumber(J, 8); // lwn color
	float	x0 = js_tonumber(J, 2),
			y0 = js_tonumber(J, 3),
			r = js_tonumber(J, 4),
			sa = js_tonumber(J, 5),
			ea = js_tonumber(J, 6);

	if (ayyi == MUSHEER) matrix_circle(&WJHH.mshr .mtrx, x0, y0, r, sa, ea, fg, l);
	if (ayyi == RAEES  ) matrix_circle(&WJHH.raees.mtrx, x0, y0, r, sa, ea, fg, l);

	js_pushundefined(J);
}
void composer_matrix_circle_alif(js_State *J) { // circle aa
	int	fg = js_tonumber(J, 7),
		l = js_tonumber(J, 8); // lwn color
	char taba3 = js_tonumber(J, 9);
	float	x0 = js_tonumber(J, 2),
			y0 = js_tonumber(J, 3),
			r = js_tonumber(J, 4),
			sa = js_tonumber(J, 5),
			ea = js_tonumber(J, 6);

	if (ayyi == MUSHEER) matrix_circle_alif(&WJHH.mshr .mtrx, x0, y0, r, sa, ea, fg, l, taba3);
	if (ayyi == RAEES  ) matrix_circle_alif(&WJHH.raees.mtrx, x0, y0, r, sa, ea, fg, l, taba3);

	js_pushundefined(J);
}
void composer_msfoof_mustateel(js_State *J) { // mustateel
	int	fg = js_tonumber(J, 6),
		l = js_tonumber(J, 7); // lwn color
	float	x0 = js_tonumber(J, 2),
			y0 = js_tonumber(J, 3),
			w = js_tonumber(J, 4),
			h = js_tonumber(J, 5);

	if (ayyi == MUSHEER) msfoof_rect(&WJHH.mshr .mtrx, x0, y0, w, h, fg, l);
	if (ayyi == RAEES  ) msfoof_rect(&WJHH.raees.mtrx, x0, y0, w, h, fg, l);

	js_pushundefined(J);
}
void composer_msfoof_mustateel_alif(js_State *J) { // mustateel aa
	int	fg = js_tonumber(J, 6),
		l = js_tonumber(J, 7); // lwn color
	char taba3 = js_tonumber(J, 8);
	float	x0 = js_tonumber(J, 2),
			y0 = js_tonumber(J, 3),
			w = js_tonumber(J, 4),
			h = js_tonumber(J, 5);

	if (ayyi == MUSHEER) msfoof_mustateel(&WJHH.mshr .mtrx, x0, y0, w, h, fg, l, taba3);
	if (ayyi == RAEES  ) msfoof_mustateel(&WJHH.raees.mtrx, x0, y0, w, h, fg, l, taba3);

	js_pushundefined(J);
}
void composer_msfoof_ijtama3(js_State *J) { // composite, combine
	int		min		= js_tonumber(J, 1), // from which
			ilaa	= js_tonumber(J, 2); // to which
	float	x		= js_tonumber(J, 3),
			y		= js_tonumber(J, 4),
			w 		= js_tonumber(J, 5),
			h 		= js_tonumber(J, 6),
			x1		= js_tonumber(J, 7),
			y1		= js_tonumber(J, 8);

	ihaatah	i1 = {x, y, w, h},
			i2 = {x1, y1};
	
	msfoof	*m1 = &WJHH.mshr.mtrx,
			*m2 = &WJHH.mshr.mtrx;

	if (min == RAEES) m1 = &WJHH.raees.mtrx;
	if (ilaa == RAEES) m2 = &WJHH.raees.mtrx;

	msfoof_composite_clipped(m1, m2, &i1, &i2);
	msfoof_composite_clipped(m1, m2, &i1, &i2);

	js_pushundefined(J);
}
void composer_msfoof_maqtoo3(js_State *J) { // rectangular clip
	int	ayyi = js_tonumber(J, 1); // lwn color
	float	x0 = js_tonumber(J, 2),
			y0 = js_tonumber(J, 3),
			w = js_tonumber(J, 4),
			h = js_tonumber(J, 5);

	msfoof *s = NULL;
	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) s = &WJHH.raees.mtrx;

	if (s) s->maqtoo3 = (ihaatah) {x0, y0, w, h};

	js_pushundefined(J);
}
//}

//{ canvas
void composer_canvas_move_to(js_State *J) {
	float	x0 = js_tonumber(J, 1),
			y0 = js_tonumber(J, 2);

	if (syq) {
		cairo_move_to(syq, x0, y0);
	}

	js_pushundefined(J);
}
void composer_canvas_eataf(js_State *J) { // arc
	float	x  = js_tonumber(J, 1),
			y  = js_tonumber(J, 2),
			r  = js_tonumber(J, 3),
			sa = js_tonumber(J, 4),
			ea = js_tonumber(J, 5);

	if (syq) {
		cairo_arc (syq, x, y, r, toradians(sa), toradians(ea));
	}

	js_pushundefined(J);
}
void composer_canvas_rect(js_State *J) { // rect
	float	x = js_tonumber(J, 1),
			y = js_tonumber(J, 2),
			w = js_tonumber(J, 3),
			h = js_tonumber(J, 4);

	if (syq) {
		cairo_rectangle(syq, x, y, w, h);
	}

	js_pushundefined(J);
}
void composer_canvas_line(js_State *J) { // line
	float	x0 = js_tonumber(J, 1),
			y0 = js_tonumber(J, 2);

	if (syq) {
		cairo_line_to(syq, x0, y0);
	}

	js_pushundefined(J);
}
void composer_canvas_stroke(js_State *J) { // stroke / strike
	int	preserve = js_tonumber(J, 1); // preserve

	if (syq) {
		if (preserve)
		cairo_stroke_preserve(syq);
		else
		cairo_stroke(syq);
	}

	js_pushundefined(J);
}
void composer_canvas_fill(js_State *J) { // fill / spill
	int	preserve = js_tonumber(J, 1); // preserve

	if (syq) {
		if (preserve)
		cairo_fill_preserve(syq);
		else
		cairo_fill(syq);
	}

	js_pushundefined(J);
}
void composer_canvas_lwn(js_State *J) {
	float r = js_tonumber(J, 1);
	float g = js_tonumber(J, 2);
	float b = js_tonumber(J, 3);
	float a = js_tonumber(J, 4);

	if (syq && !isnan(r) && !isnan(g) && !isnan(b)) {
		r = haddqanaatf(r);
		g = haddqanaatf(g);
		b = haddqanaatf(b);

		if (!isnan(a)) {
			a = haddqanaatf(a);
			cairo_set_source_rgba (syq, r, g, b, a);
		} else
			cairo_set_source_rgb (syq, r, g, b);
	}

	js_pushundefined(J);
}
void composer_canvas_idhan(js_State *J) { // paint
	if (syq) {
		cairo_paint (syq);
	}

	js_pushundefined(J);
}
void composer_canvas_renew_path(js_State *J) { // renew path
	if (syq) {
		cairo_new_path (syq);
	}

	js_pushundefined(J);
}
void composer_canvas_line_width(js_State *J) {
	float l = js_tonumber(J, 1);

	if (syq && !isnan(l)) {
		if (l < 0) l = 0;
		else if (l > 100) l = 100;

		cairo_set_line_width (syq, l);
	}

	js_pushnumber(J, l);
}
void composer_canvas_xat(js_State *J) { // font face
	const char *str = js_tostring(J, 1);

	if (syq && str) {
		cairo_select_font_face (syq, str, 0, 0);
	}

	js_pushundefined(J);
}
void composer_canvas_font_size(js_State *J) { // font size
	float l = js_tonumber(J, 1);

	if (syq && !isnan(l)) {
		cairo_set_font_size (syq, l);
	}

	js_pushundefined(J);
}
void composer_canvas_font_bounds(js_State *J) { // font extents
	if (syq) {
		ihaatahf ih = { 0 };
		font_bounds (&ih);

		js_newobject(J);
		{
			js_pushnumber(J, ih.x), adaaf_sift(J, "x");
			js_pushnumber(J, ih.y), adaaf_sift(J, "y");
			js_pushnumber(J, ih.s), adaaf_sift(J, "a");
			js_pushnumber(J, ih.w), adaaf_sift(J, "d");
			js_pushnumber(J, ih.h), adaaf_sift(J, "h");
		}
	} else js_pushundefined(J);
}
void composer_canvas_matn(js_State *J) { // text (returns extents!)
	float uni = js_tonumber(J, 1);
	int dry = js_tonumber(J, 2); // only return extents

	if (syq) {
		if (isnan(uni)) {
			const char *str = js_tostring(J, 1);
			cairo_show_text(syq, str);
		} else {
			uni = fushatarmeez.tofusha[(int)uni];
			
			ihaatahf ih;
			harf_hajm(&ih, uni);
			
			if (dry != 1) matn_harf(uni);
			
			js_newobject(J);
			{
				js_pushnumber(J, ih.x), adaaf_sift(J, "a");
				js_pushnumber(J, ih.s), adaaf_sift(J, "x");
				js_pushnumber(J, ih.y), adaaf_sift(J, "y");
				js_pushnumber(J, ih.w), adaaf_sift(J, "w");
				js_pushnumber(J, ih.h), adaaf_sift(J, "h");
			}
		}
	} else js_pushundefined(J);
}
void composer_canvas_haal(js_State *J) { // canvas on/off
	int l = js_tonumber(J, 1);

	if (syq) {
		use_canvas = l;
	}

	js_pushundefined(J);
}
void composer_canvas_restore(js_State *J) { // restore
	if (syq) {
		cairo_restore (syq);
	}

	js_pushundefined(J);
}
void composer_canvas_iqta3(js_State *J) { // clip
	iqta3();

	js_pushundefined(J);
}
void composer_canvas_save(js_State *J) { // save
	if (syq) {
		cairo_save (syq);
	}

	js_pushundefined(J);
}
void composer_canvas_scale(js_State *J) { // scale
	float x = js_tonumber(J, 1);
	float y = js_tonumber(J, 2);
	if (syq) {
		cairo_scale (syq, x, y);
	}

	js_pushundefined(J);
}
void composer_canvas_rotate(js_State *J) { // rotate
	float z = js_tonumber(J, 1);
	if (syq) {
		cairo_rotate (syq, z);
	}

	js_pushundefined(J);
}
void composer_canvas_tarjam(js_State *J) { // translate
	float x = js_tonumber(J, 1),
		  y = js_tonumber(J, 2);
	if (syq) {
		cairo_translate (syq, x, y);
	}

	js_pushundefined(J);
}
//}

void composer_alwaan_hsl(js_State *J) { // hsl to rgb
	float	h = js_tonumber(J, 1),
			s = js_tonumber(J, 2),
			v = js_tonumber(J, 3);

	lwnh l = {h, s, v};

	js_pushnumber(J, lwnl2rgb(&l));
}

//{ eqonah
void composer_eqonah_iftah(js_State *J) { // open
	const char *str = js_tostring(J, 1);

	eqonah_init(&eqnh, (char *) str);

	js_pushundefined(J);
}
void composer_eqonah_hajm(js_State *J) { // bounds
	int x = js_tonumber(J, 1),
		y = js_tonumber(J, 2),
		w = js_tonumber(J, 3),
		h = js_tonumber(J, 4);

	eqnh.hudood.x = x;
	eqnh.hudood.y = y;
	eqnh.hudood.w = w;
	eqnh.hudood.h = h;

	js_pushundefined(J);
}
void composer_eqonah_qadd(js_State *J) { // size/scale
	float qadd = js_tonumber(J, 1);

	eqnh.hudood.s = qadd;

	js_pushundefined(J);
}
void composer_eqonah_sattar(js_State *J) { // draw
	int ayyi = js_tonumber(J, 1),
		l = js_tonumber(J, 2);

	msfoof *s = NULL;
	if (ayyi == MUSHEER) s = &WJHH.mshr.mtrx;
	if (ayyi == RAEES) s = &WJHH.raees.mtrx;
	int x = eqnh.hudood.x;
	int y = eqnh.hudood.y;
	// TEST the pipe comparison can be buggy
	int w = eqnh.hudood.w || s->w;
	int h = eqnh.hudood.h || s->h;
	float scale = eqnh.hudood.s;

	ihaatahf ihth = {x, y, w, h, scale, 0};

	if (s != NULL) eqonah_sattar(eqnh, *s, &ihth, l);

	js_pushundefined(J);
}
//}

//{ kaatib
void composer_kaatib_tool(js_State *J) { // total huroof in kaatib
	js_pushnumber(J, ktb.length);
}
void composer_kaatib_harf(js_State *J) { // get harf details
	int i = js_tonumber(J, 1);
	if (i < ktb.length && ktb.barzax) {
		harf *h = &ktb.barzax[i];
		js_newobject(J);
		{
			js_pushnumber(J, h->id), adaaf_sift(J, "id");
			js_pushnumber(J, h->ihth.x), adaaf_sift(J, "x");
			js_pushnumber(J, h->ihth.y), adaaf_sift(J, "y");
			js_pushnumber(J, h->ihth.w), adaaf_sift(J, "w");
			js_pushnumber(J, h->ihth.h), adaaf_sift(J, "h");
			js_pushnumber(J, h->aqil.x), adaaf_sift(J, "ax");
			js_pushnumber(J, h->aqil.y), adaaf_sift(J, "ay");
			js_pushnumber(J, h->aqil.w), adaaf_sift(J, "aw");
			js_pushnumber(J, h->aqil.h), adaaf_sift(J, "ah");
			js_pushnumber(J, h->ltr), adaaf_sift(J, "ltr");
			js_pushnumber(J, h->muntaxab), adaaf_sift(J, "muntaxab");
			js_pushnumber(J, kaatib_zaahir(&ktb, h)), adaaf_sift(J, "zaahir");
		}
	} else js_pushundefined(J);
}
void composer_kaatib_axav(js_State *J) { // get a new kaatib
	

	js_pushundefined(J);
}
void composer_kaatib_iftah(js_State *J) { // open
	const char *str = js_tostring(J, 1);

	malaf mlf;
	malaf_init(&mlf, (char *) str);
	matn_nazaf(&ktb.mtn);
	malaf_ilaa_matn(&mlf, &ktb.mtn.barzax, &ktb.mtn.length);
	kaatib_init(&ktb); // will do matn -> huroof
	malaf_destroy(&mlf);

	js_pushundefined(J);
}
void composer_kaatib_save(js_State *J) { // save
	const char *str = js_tostring(J, 1);

	matn_nazaf(&ktb.mtn);
	kaatib_ilaa_matn(&ktb);
	malaf mlf;
	mlf.length = ktb.mtn.length;
	mlf.barzax = (u_char *) ktb.mtn.barzax;
	malaf_hfiz(&mlf, (char *) str);

	js_pushundefined(J);
}
void composer_kaatib_ihsab(js_State *J) { // calc bounds of all huroof
	kaatib_huroof(&ktb);
	js_pushundefined(J);
}
void composer_kaatib_hajm(js_State *J) { // get/set bounds
	float	x = js_tonumber(J, 1),
			y = js_tonumber(J, 2),
			w = js_tonumber(J, 3),
			h = js_tonumber(J, 4);

	if (isnan(x)) {
		js_newobject(J);
		{
			js_pushnumber(J, ktb.g.x), adaaf_sift(J, "x");
			js_pushnumber(J, ktb.g.y), adaaf_sift(J, "y");
			js_pushnumber(J, ktb.g.w), adaaf_sift(J, "w");
			js_pushnumber(J, ktb.g.h), adaaf_sift(J, "h");
		}
	} else {
		ktb.g.x = x; ktb.g.y = y;
		ktb.g.w = w; ktb.g.h = h;
		
		kaatib_xataa(&ktb);

		js_pushundefined(J);
	}
}
void composer_kaatib_mu3aaq(js_State *J) { // disabled
	ktb.mu3aaq = js_tonumber(J, 1);

	js_pushundefined(J);
}
void composer_kaatib_taht(js_State *J) { // ???
	ktb.taht = js_tonumber(J, 1);

	js_pushundefined(J);
}
void composer_kaatib_qadd(js_State *J) { // xat size
	float q = js_tonumber(J, 1);
	if (isnan(q)) {
		js_pushnumber(J, ktb.g.s);
	} else {
		kaatib_qadd( &ktb, q );
		js_pushundefined(J);
	}
}
void composer_kaatib_makaan(js_State *J) { // scrolling offsets
	ktb.n.x = js_tonumber(J, 1);
	ktb.n.y = js_tonumber(J, 2);

	js_pushundefined(J);
}
void composer_kaatib_idhan(js_State *J) { // draw
	int ikraah = js_tonumber(J, 2);
	
	char ikraah2 = 0;

	if (ikraah == 0) ikraah2 = 0;
	if (ikraah == 1) ikraah2 = 1;

	if (ayyi == MUSHEER) kaatib_idhan(&ktb, WJHH.mshr .mtrx, ikraah2);
	if (ayyi == RAEES  ) kaatib_idhan(&ktb, WJHH.raees.mtrx, ikraah2);

	js_pushundefined(J);
}
void composer_kaatib_tasteer(js_State *J) { // draw
	const char *str = js_tostring(J, 2);

	if (ayyi == MUSHEER) kaatib_tasteer(&ktb, &WJHH.mshr .mtrx, (u_char *) str);
	if (ayyi == RAEES  ) kaatib_tasteer(&ktb, &WJHH.raees.mtrx, (u_char *) str);

	js_pushundefined(J);
}
void composer_kaatib_muntahaa(js_State *J) { // scroll offsets & bounds
	js_newobject(J);
	{
		js_pushnumber(J, ktb.n.x), adaaf_sift(J, "x");
		js_pushnumber(J, ktb.n.y), adaaf_sift(J, "y");
		js_pushnumber(J, ktb.muntahaa.w), adaaf_sift(J, "w");
		js_pushnumber(J, ktb.muntahaa.h), adaaf_sift(J, "h");
	}
}
void composer_kaatib_muashshar(js_State *J) { // cursor
	int i = js_tonumber(J, 1);
	
	if (i >= -1 && i < ktb.length) ktb.cursor = i;
	
	js_pushnumber(J, ktb.cursor);
}
void composer_kaatib_muashshar_zaahir(js_State *J) { // cursor
	int i = js_tonumber(J, 1);
	
	if (i == 0 || i == 1) ktb.showcursor = i;
	
	js_pushnumber(J, ktb.showcursor);
}
void composer_kaatib_mutaharrikah(js_State *J) { // animate scrolling
	int i = js_tonumber(J, 1);
	
	if (i == 0 || i == 1) ktb.n.c = i;
	
	js_pushnumber(J, ktb.n.c);
}
void composer_kaatib_cu3aa3(js_State *J) { // raycast
	int		x = js_tonumber(J, 1),
			y = js_tonumber(J, 2);

	harf *h = kaatib_cu3aa3(&ktb, x, y);

	if (h) {
		js_newobject(J);
		{
			js_pushnumber(J, h->id), adaaf_sift(J, "id");
			js_pushnumber(J, h->ihth.x), adaaf_sift(J, "x");
			js_pushnumber(J, h->ihth.y), adaaf_sift(J, "y");
			js_pushnumber(J, h->ihth.w), adaaf_sift(J, "w");
			js_pushnumber(J, h->ihth.h), adaaf_sift(J, "h");
			js_pushnumber(J, h->ltr), adaaf_sift(J, "ltr");
			js_pushnumber(J, h->muntaxab), adaaf_sift(J, "muntaxab");
			js_pushnumber(J, kaatib_zaahir(&ktb, h)), adaaf_sift(J, "zaahir");
		}
	} else js_pushundefined(J);
}
//}

void taba3(js_State *J) {
	if (J == NULL) return;
	const char *str = js_tostring(J, 1);
	taba3_waqt((u_char *) str);
//	printf("%s\n", str);
}
void composer_ahjaam(int qism) {
	if (J == NULL) return;
	if (qism == RAEES) {
		js_newobject(J);
		{
			js_pushnumber(J, WJHH.raees.mtrx.w); js_setproperty(J, -2, "w");
			js_pushnumber(J, WJHH.raees.mtrx.h); js_setproperty(J, -2, "h");
			js_pushnumber(J, WJHH.raees.mtrx.wa); js_setproperty(J, -2, "wa");
			js_pushnumber(J, WJHH.raees.mtrx.ha); js_setproperty(J, -2, "ha");
		}
		js_setglobal(J, "raees");
	}

	if (qism == MUSHEER) {
		js_newobject(J);
		{
			js_pushnumber(J, WJHH.mshr.mtrx.w); js_setproperty(J, -2, "w");
			js_pushnumber(J, WJHH.mshr.mtrx.h); js_setproperty(J, -2, "h");
			js_pushnumber(J, WJHH.mshr.mtrx.wa); js_setproperty(J, -2, "wa");
			js_pushnumber(J, WJHH.mshr.mtrx.ha); js_setproperty(J, -2, "ha");
		}
		js_setglobal(J, "musheer");
	}
	composer_b_hajm(qism);
}
void composer_load_app() { // get app
	if (J == NULL) return;
	poly.length = 0;
	eqonah_nazaf(&eqnh); // sets .id to -1
	eqnh.hudood.s = 1;

	// TODO havaf all kaatibeen
	js_dofile(J, "apps/eventcodes.js");
	js_dofile(J, trkb.xitaab);
	// redo events
	composer_b_hajm(RAEES);
	composer_b_hajm(MUSHEER);
}
void composer_tamaam() { // end, finish, destroy
    if (J) js_freestate(J);
}
char composer_icaarah(mafateeh m) {
	char yes = 0; float step = .125;
	if (mushtamilf(m.x, m.y, &ktb.g) && m.w) {
		if (m.ctrl) { // not reported from x11, TODO make it
			if (m.w == -1 && ktb.g.s > 1) {
				kaatib_qadd(&ktb, ktb.g.s -= step);
				kaatib_huroof(&ktb);
				kaatib_itlaq_hudood(&ktb);
				kaatib_muashshar(&ktb);
				yes = 1;
			}
			if (m.w ==  1 && ktb.g.s < 256) {
				kaatib_qadd(&ktb, ktb.g.s += step);
				kaatib_huroof(&ktb);
				kaatib_itlaq_hudood(&ktb);
				kaatib_muashshar(&ktb);
				yes = 1;
			}
		} else {
			if (m.w == -1) kaatib_up(&ktb), yes = 1;
			if (m.w ==  1) kaatib_down(&ktb), yes = 1;
		}
	}
	return yes;
}
char composer_lowhah(mafateeh m) {
	char yes = 0; float step = .5;
	if (m.ctrl && m.key == KEY_M && m.state) {
		ktb.mu3aaq = !ktb.mu3aaq;
		yes = 1;
	}
	if (!ktb.mst3d || ktb.mu3aaq) return 0;
	if (m.ctrl && m.key == KEY_EQUAL && m.state) {
		if (ktb.g.s < 256) {
			kaatib_qadd(&ktb, ktb.g.s += step);
			kaatib_huroof(&ktb);
			kaatib_itlaq_hudood(&ktb);
			kaatib_muashshar(&ktb);
			yes = 1;
		}
	} else
	if (m.ctrl && m.key == KEY_MINUS && m.state) {
		if (ktb.g.s > 1) {
			kaatib_qadd(&ktb, ktb.g.s -= step);
			kaatib_huroof(&ktb);
			kaatib_itlaq_hudood(&ktb);
			kaatib_muashshar(&ktb);
			yes = 1;
		}
	} else
	if (m.ctrl && m.key == KEY_A && m.state && m.shift) {
		yes = kaatib_ghayr_muntaxab_kul(&ktb);
	} else
	if (m.ctrl && m.key == KEY_A && m.state && !m.shift) {
		yes = kaatib_muntaxab_kul(&ktb);
	} else
	if (m.ctrl && m.key == KEY_B && m.state) {
		yes = kaatib_wazan(&ktb);
	} else
	if (m.shift == 0 && m.state == 0 && m.key == 0) {
		kaatib_muntaxab_tamaam(&ktb);
	} else
	if (m.state == 0) { // catch all unhandled keydowns
	} else
	if (m.key == KEY_LEFT) {
//		if (m.shift) yes = kaatib_muntaxab(&ktb);
		if (m.shift) yes = kaatib_muntaxab_jaanibi(&ktb, -1);
		yes = kaatib_left(&ktb);
	} else
	if (m.key == KEY_RIGHT) {
//		if (m.shift) yes = kaatib_muntaxab(&ktb);
		if (m.shift) yes = kaatib_muntaxab_jaanibi(&ktb,  1);
		yes = kaatib_right(&ktb);
	} else
	if (m.key == KEY_UP) {
		yes = kaatib_up(&ktb);
	} else
	if (m.key == KEY_DOWN) {
		yes = kaatib_down(&ktb);
	} else
	if (m.key == KEY_PAGEUP) {
		yes = kaatib_pageup(&ktb);
	} else
	if (m.key == KEY_PAGEDOWN) {
		yes = kaatib_pagedown(&ktb);
	} else
	if (m.ctrl && m.key == KEY_HOME) {
		// TODO
	} else
	if (m.key == KEY_HOME) {
		yes = kaatib_home(&ktb);
	} else
	if (m.ctrl && m.key == KEY_END) {
		// TODO
	} else
	if (m.key == KEY_END) {
		yes = kaatib_end(&ktb);
	} else
	if (m.key == KEY_ENTER) {
		kaatib_adaaf(&ktb, '\n'); yes = 1;
	} else
	if (m.key == KEY_TAB) {
		kaatib_adaaf(&ktb, '\t'); yes = 1;
	} else
	if (m.key == KEY_SPACE) {
		kaatib_adaaf(&ktb, ' '); yes = 1;
	} else
	if (m.key == KEY_DELETE) {
		kaatib_hadhaf(&ktb, 1); yes = 1;
	} else
	if (m.key == KEY_BACKSPACE) {
		kaatib_hadhaf(&ktb, 0); yes = 1;
	} else if (!yes) {
		int len = strlen(m.huroof);
		if (len) {
			for (int i = 0; i < len; ++i) {
				kaatib_adaaf(&ktb, m.huroof[i]);
			}
			yes = 1;
		}
	}
	return yes;
}
int composer_handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);
		if (w->ism == MUDEER && w->miftaah == ISHAARAH) {
			char yes = 0;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			if (!yes && trkb.b_icaarah) {
				yes = trkb.b_icaarah(m);
			}
			if (!yes) {
				// BUG crashes when too many events at once
				// fixed when commented out :/ :(
				yes = composer_b_icaarah(m) > 0;
			}
			if (!yes) {
				yes = composer_icaarah(m);
			}
			if (yes) {
				if (!WJHH.mshr.mutadarrar )
					 WJHH.mshr.mutadarrar  = 1;
				if (!WJHH.raees.mutadarrar)
					 WJHH.raees.mutadarrar = 1;
			}
		}
		if (w->ism == MUDEER && w->miftaah == LOWHAH) {
			char yes = 0;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			if (m.state) {
				if (m.ctrl) {
					if (m.key == KEY_R) composer_load_app(), yes = 1;
					if (m.key == KEY_Q) exit(0), yes = 1;
				}
				if (m.alt) {
					if (m.key == KEY_S) WJHH.mshr.mtrx.smooth = !WJHH.mshr.mtrx.smooth, yes = 1;
				}
			}
			if (!yes && trkb.b_lowhah) {
				yes = trkb.b_lowhah(m);
			}
			if (!yes) {
				yes = composer_b_lowhah(m);
			}
			if (!yes) {
				yes = composer_lowhah(m);
			}
			if (yes) {
				if (!WJHH.mshr.mutadarrar )
					 WJHH.mshr.mutadarrar  = 1;
				if (!WJHH.raees.mutadarrar)
					 WJHH.raees.mutadarrar = 1;
			}
		}
		if (w->ism == MUSHEER) {
			if (w->miftaah == MST3D) {
				if (surfacemshr) cairo_surface_destroy(surfacemshr);
				if (caimshr) cairo_destroy(caimshr);
				surfacemshr = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WJHH.mshr.mtrx.w, WJHH.mshr.mtrx.h);
				caimshr = cairo_create (surfacemshr);

				int w = WJHH.mshr.mtrx.w, h = WJHH.mshr.mtrx.h;

				cairo_set_source_rgb( caimshr, 0, 0, 0 );
				cairo_paint (caimshr);

				/*
//				cairo_arc (caimshr, w/2, h/2, w/2, 0, 2*M_PI);
//				cairo_clip (caimshr);
//				cairo_new_path (caimshr); // path not consumed by clip()
		
				cairo_surface_t *image;
				image = cairo_image_surface_create_from_png ("/media/nano/projects/nizaam/data/mujs-logo.png");
				int imgw = cairo_image_surface_get_width (image);
				int imgh = cairo_image_surface_get_height (image);
		
				float pct = 1, axis = imgh;
//				if (imgw > imgh) axis = imgw; // fit
				if (imgw < imgh) axis = imgw; // cover
				if (axis > w) pct = w / axis;
				else pct = axis / w;
		
				cairo_scale (caimshr, pct, pct);
				cairo_set_source_surface (caimshr, image, 0, 0);
				cairo_paint (caimshr);
				cairo_surface_flush(surfacemshr);
				
				cairo_surface_destroy(image);*/

				WJHH.mshr.mtrx.smooth = 1;
				if (trkb.on_bound_indicator) trkb.on_bound_indicator( (ihaatah) {0, 0, WJHH.mshr.mtrx.w, WJHH.mshr.mtrx.h} );
				composer_ahjaam(MUSHEER);
			}
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				if (WJHH.mshr.mutadarrar) {
					syq = caimshr;
					ayyi = MUSHEER;
					
					char darar = 0;
					// if has C func override, call that first
					if (trkb.on_paint_indicator) darar = trkb.on_paint_indicator(&WJHH.mshr.mtrx);

					if (!darar) darar = composer_on_paint_indicator();
					if (darar) {
						cairo2msfoof(&WJHH.mshr.mtrx, surfacemshr, caimshr);
						amr_irsal_ilaa(MUDEERUID, MUSHEER, DARAR, 1);
					}
					
					if (kaatib_mutadarrar(&ktb)) darar = 2;

					// if animating, return 2 until anim is complete
					if (darar != 2) WJHH.mshr.mutadarrar = 0;
				}
			}
		}
		if (w->ism == RAEES) {
			if (w->miftaah == MST3D) {
				if (surfaceraees) cairo_surface_destroy(surfaceraees);
				if (cairaees) cairo_destroy(cairaees);
				surfaceraees = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WJHH.raees.mtrx.w, WJHH.raees.mtrx.h);
				cairaees = cairo_create (surfaceraees);

				WJHH.raees.mtrx.smooth = 1;
				WJHH.raees.mutadarrar = 1;
				if (w->qadr == 1) {
					ktb.g.w = WJHH.raees.mtrx.w;
					ktb.g.h = WJHH.raees.mtrx.h;
					ktb.mtrx = WJHH.raees.mtrx;
					kaatib_qadd(&ktb, 16);
					kaatib_xataa(&ktb);
					ktb.mst3d = 1;
				} else {
					ktb.mst3d = 0;
				}
				if (trkb.on_bound_window) trkb.on_bound_window( (ihaatah) {0, 0, WJHH.raees.mtrx.w, WJHH.raees.mtrx.h} );
				composer_ahjaam(RAEES);
			}
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
				if (WJHH.raees.mutadarrar) {
					syq = cairaees;
					ayyi = RAEES;
					
					char darar = 0;
					// if has C func override, call that first
					if (trkb.on_paint_window) darar = trkb.on_paint_window(&WJHH.raees.mtrx);

					if (!darar) darar = composer_on_paint_window();
					if (darar) {
						// idk where to put this
						cairo2msfoof(&WJHH.raees.mtrx, surfaceraees, cairaees);
						amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
					}
					
					if (kaatib_mutadarrar(&ktb)) darar = 2;

					// if animating, return 2 until anim is complete
					if (darar != 2) WJHH.raees.mutadarrar = 0;
				}
			}
		}
	}

	return 0;
}
void composer_badaa_js(composer t) { // 
	J = js_newstate(NULL, NULL, JS_STRICT);

	// allow access to the global environment via the 'global' name
	js_pushglobal(J);
	js_defglobal(J, "global", JS_READONLY | JS_DONTCONF | JS_DONTENUM);

	js_newcfunction(J, taba3, "taba3", 1);
	js_setglobal(J, "taba3");
	
	js_pushnumber(J, MUSHEER);
	js_setglobal(J, "MUSHEER");
	js_pushnumber(J, RAEES);
	js_setglobal(J, "RAEES");

	js_newobject(J);
	{
		js_pushnumber(J, 0x666666); adaaf_wasf(J, "waahid");
		js_pushnumber(J, 0x444444); adaaf_wasf(J, "acnaan");
		js_pushnumber(J, 0x222222); adaaf_wasf(J, "caalic");
		js_pushnumber(J, 0xffffff); adaaf_wasf(J, "qaadim");
		js_pushnumber(J, 0       ); adaaf_wasf(J, "saabiq");
		js_pushnumber(J, 0xffff00); adaaf_wasf(J, "lahjah");
	}
	js_setglobal(J, "mowdoo3");

	js_newobject(J); {
		adaaf_tareeqah(J, "hisaab.deg",	composer_hisaab_drj, 1); // to degrees/darajaat
		adaaf_tareeqah(J, "hisaab.rad",	composer_hisaab_rad, 1); // to radian
		adaaf_tareeqah(J, "hisaab.niz",	composer_hisaab_niz, 4); // nuqtah ilaa zaawiyah
		adaaf_tareeqah(J, "hisaab.zin",	composer_hisaab_zin, 4); // zaawiyah ilaa nuqtah
	}
	js_defglobal(J, "hisaab", JS_READONLY | JS_DONTCONF);

	js_newobject(J); {
		adaaf_tareeqah(J, "msfoof.qadd",			composer_msfoof_qadd,			1);
		adaaf_tareeqah(J, "msfoof.layyin",			composer_msfoof_layyin,			1);
		adaaf_tareeqah(J, "msfoof.nazzaf",			composer_msfoof_nazzaf,			1);
		adaaf_tareeqah(J, "msfoof.nazzaf_alif",		composer_msfoof_nazzaf_alif,		1);
		adaaf_tareeqah(J, "msfoof.ja3al_alif",		composer_msfoof_ja3al_alif,		3);
		adaaf_tareeqah(J, "msfoof.axav_alif",		composer_msfoof_axav_alif,		2);
		adaaf_tareeqah(J, "msfoof.axav_owst_alif",	composer_msfoof_axav_owst_alif,	2);
		adaaf_tareeqah(J, "matrix.print_alpha",		composer_matrix_print_alpha,		1);
		adaaf_tareeqah(J, "msfoof.ikbar",			composer_msfoof_ikbar,			4);
		adaaf_tareeqah(J, "msfoof.ikbar_alif",		composer_msfoof_ikbar_alif,		3);
		adaaf_tareeqah(J, "msfoof.satar",			composer_msfoof_satar,			5);
		adaaf_tareeqah(J, "msfoof.line_alif",		composer_msfoof_line_alif,		6);
		adaaf_tareeqah(J, "msfoof.satar_alif",		composer_msfoof_satar_alif,		6);
		adaaf_tareeqah(J, "msfoof.daairah",			composer_matrix_circle,			7);
		adaaf_tareeqah(J, "msfoof.daairah_alif",	composer_matrix_circle_alif,	8);
		adaaf_tareeqah(J, "msfoof.mustateel",		composer_msfoof_mustateel,		6);
		adaaf_tareeqah(J, "msfoof.mustateel_alif",	composer_msfoof_mustateel_alif,	7);
		adaaf_tareeqah(J, "msfoof.ijtama3",			composer_msfoof_ijtama3,			8);
		adaaf_tareeqah(J, "msfoof.maqtoo3",			composer_msfoof_maqtoo3,			5);
	}
	js_defglobal(J, "msfoof", JS_READONLY | JS_DONTCONF);

	js_newobject(J); {
		adaaf_tareeqah(J, "canvas.move_to",		composer_canvas_move_to,		2); // move
		adaaf_tareeqah(J, "canvas.xat",			composer_canvas_xat,			1); // font face
		adaaf_tareeqah(J, "canvas.font_size",	composer_canvas_font_size,	1); // font size
		adaaf_tareeqah(J, "canvas.font_bounds",	composer_canvas_font_bounds,	0); // font metrics
		adaaf_tareeqah(J, "canvas.matn",		composer_canvas_matn,		2); // text
		adaaf_tareeqah(J, "canvas.rect",	composer_canvas_rect,	4); // rect
		adaaf_tareeqah(J, "canvas.satar",		composer_canvas_line,		2); // line
		adaaf_tareeqah(J, "canvas.eataf",		composer_canvas_eataf,		5); // arc
		adaaf_tareeqah(J, "canvas.stroke",		composer_canvas_stroke,		1); // stroke
		adaaf_tareeqah(J, "canvas.fill",		composer_canvas_fill,		1); // fill
		adaaf_tareeqah(J, "canvas.lwn",			composer_canvas_lwn,			4); // color
		adaaf_tareeqah(J, "canvas.idhan",		composer_canvas_idhan,		0); // paint
		adaaf_tareeqah(J, "canvas.line_width",		composer_canvas_line_width,		1); // width (stroke)
		adaaf_tareeqah(J, "canvas.renew_path",		composer_canvas_renew_path,		0); // renew path
		adaaf_tareeqah(J, "canvas.iqta3",		composer_canvas_iqta3,		0); // clip
		adaaf_tareeqah(J, "canvas.restore",		composer_canvas_restore,		0); // restore
		adaaf_tareeqah(J, "canvas.save",		composer_canvas_save,		0); // save
		adaaf_tareeqah(J, "canvas.haal",		composer_canvas_haal,		1); // canvas on/off
		adaaf_tareeqah(J, "canvas.scale",		composer_canvas_scale,		1); // scale
		adaaf_tareeqah(J, "canvas.rotate",		composer_canvas_rotate,		1); // rotate
	}
	js_defglobal(J, "canvas", JS_READONLY | JS_DONTCONF); // canvas, fabric, cloth

	js_newobject(J); {
		adaaf_tareeqah(J, "alwaan.hsl2rgb", composer_alwaan_hsl, 3);
	}
	js_defglobal(J, "alwaan", JS_READONLY | JS_DONTCONF);

	js_newobject(J); {
		adaaf_tareeqah(J, "eqonah.iftah", composer_eqonah_iftah, 1);
		adaaf_tareeqah(J, "eqonah.hajm", composer_eqonah_hajm, 5); // x, y, w, h
//		adaaf_tareeqah(J, "eqonah.taht", composer_kaatib_taht, 1); // background transparent
		adaaf_tareeqah(J, "eqonah.qadd", composer_eqonah_qadd, 1); // eqonah size
//		adaaf_tareeqah(J, "eqonah.mu3aaq", composer_kaatib_mu3aaq, 1); // disabled
//		adaaf_tareeqah(J, "eqonah.makaan", composer_kaatib_makaan, 2); // ox, oy scroll offsets
		adaaf_tareeqah(J, "eqonah.sattar", composer_eqonah_sattar, 2); // draw [msfoof, color]
//		adaaf_tareeqah(J, "eqonah.muntahaa", composer_kaatib_muntahaa, 0); // scroll w, h
//		adaaf_tareeqah(J, "eqonah.muashshar", composer_kaatib_muashshar, 1); // cursor
//		adaaf_tareeqah(J, "eqonah.muashshar_zaahir", composer_kaatib_muashshar_zaahir, 1); // show cursor
//		adaaf_tareeqah(J, "eqonah.mutaharrikah", composer_kaatib_mutaharrikah, 1); // animate scrolling
	}
	js_defglobal(J, "eqonah", JS_READONLY | JS_DONTCONF);

	js_newobject(J); {
		adaaf_tareeqah(J, "kaatib.harf", composer_kaatib_harf, 1);
		adaaf_tareeqah(J, "kaatib.tool", composer_kaatib_tool, 0);
		adaaf_tareeqah(J, "kaatib.iftah", composer_kaatib_iftah, 1); // open file
		adaaf_tareeqah(J, "kaatib.save", composer_kaatib_save, 1); // save file
		adaaf_tareeqah(J, "kaatib.ihsab", composer_kaatib_ihsab, 0); // calc huroof bounds
//		adaaf_tareeqah(J, "kaatib.up", composer_kaatib_up, 0); // 
//		adaaf_tareeqah(J, "kaatib.down", composer_kaatib_down, 0); // 
		adaaf_tareeqah(J, "kaatib.hajm", composer_kaatib_hajm, 4); // x, y, w, h
		adaaf_tareeqah(J, "kaatib.taht", composer_kaatib_taht, 1); // background transparent
		adaaf_tareeqah(J, "kaatib.qadd", composer_kaatib_qadd, 1); // font size
		adaaf_tareeqah(J, "kaatib.mu3aaq", composer_kaatib_mu3aaq, 1); // disabled
		adaaf_tareeqah(J, "kaatib.makaan", composer_kaatib_makaan, 2); // ox, oy scroll offsets
		adaaf_tareeqah(J, "kaatib.idhan", composer_kaatib_idhan, 1); // paint it
		adaaf_tareeqah(J, "kaatib.tasteer", composer_kaatib_tasteer, 1); // draw
		adaaf_tareeqah(J, "kaatib.muntahaa", composer_kaatib_muntahaa, 0); // scroll w, h
		adaaf_tareeqah(J, "kaatib.muashshar", composer_kaatib_muashshar, 1); // cursor
		adaaf_tareeqah(J, "kaatib.muashshar_zaahir", composer_kaatib_muashshar_zaahir, 1); // show cursor
		adaaf_tareeqah(J, "kaatib.mutaharrikah", composer_kaatib_mutaharrikah, 1); // animate scrolling
		adaaf_tareeqah(J, "kaatib.cu3aa3", composer_kaatib_cu3aa3, 2); // raycast
	}
	js_defglobal(J, "kaatib", JS_READONLY | JS_DONTCONF);

	js_newobject(J); {
		adaaf_tareeqah(J, "polygon.adaaf", composer_polygon_adaaf, 2);
		adaaf_tareeqah(J, "polygon.havaf", composer_polygon_havaf, 0);
		adaaf_tareeqah(J, "polygon.ja3al", composer_polygon_ja3al, 3);
		adaaf_tareeqah(J, "polygon.axav",  composer_polygon_axav, 1);
		adaaf_tareeqah(J, "polygon.sattar",  composer_polygon_sattar, 0);
	}
	js_defglobal(J, "polygon", JS_READONLY | JS_DONTCONF);

	composer_ahjaam(MUSHEER);
	composer_ahjaam(RAEES);
}
void composer_badaa(composer t) { // 
	taba3_waqt("badaa");

	poly.length = 0;
	poly.nuqaat = (nuqtahf *) calloc(256, sizeof(nuqtahf));

	trkb.ism = t.ism;
	trkb.xitaab = t.xitaab;
	trkb.b_lowhah = t.b_lowhah;
	trkb.on_paint_indicator = t.on_paint_indicator;
	trkb.on_paint_window = t.on_paint_window;
	trkb.on_bound_indicator = t.on_bound_indicator;
	trkb.on_bound_window   = t.on_bound_window;

	if (t.xitaab != NULL) composer_badaa_js(t);

	tarmeez_core();
	kaatib_init(&ktb); // will do matn -> huroof

	// setup cavnas wrapper
	qmc.font_bounds	= &font_bounds		;
	qmc.harf_hajm	= &harf_hajm	;
	qmc.huroof_hajm	= &huroof_hajm	;
	qmc.matn_harf	= &matn_harf	;
	qmc.matn_huroof	= &matn_huroof	;
	qmc.xat_wijhah	= &xat_wijhah	;
	qmc.font_size	= &font_size		;
	qmc.mustateel	= &mustateel	;
	qmc.lwn_rgb		= &lwn_rgb		;
	qmc.lwn_rgba	= &lwn_rgba		;
	qmc.move_to		= &move_to		;
	qmc.satar		= &satar		;
	qmc.fill		= &fill		;
	qmc.stroke		= &stroke		;
	qmc.renew_path		= &renew_path		;
	
	ktb.qmc = &qmc; // attach canvas wrapper/driver/bridge

	amr_towheed(trkb.ism);
	amr_wijhah(1);
	
	composer_load_app();

	amr_axath(&composer_handler);
	
	composer_tamaam();
	free(poly.nuqaat);
}

