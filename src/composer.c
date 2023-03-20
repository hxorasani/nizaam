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

kaatib ktb;
eqonah eqnh;
composer trkb;

int use_canvas = 1;

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
#define ARRAY_LENGTH(a) (sizeof (a) / sizeof (a)[0])
int CLAMP (int v, int v1, int v2) {
  return v >= v1 ? (v <= v2 ? v : v2) : v1;
}
cairo_surface_t * _cairo_image_surface_create_compatible (cairo_surface_t *surface) {
	return cairo_image_surface_create (cairo_image_surface_get_format (surface),
					   cairo_image_surface_get_width (surface),
					   cairo_image_surface_get_height (surface));
}
u_char * _cairo_image_surface_flush_and_get_data (cairo_surface_t *surface) {
	if (surface != NULL) NULL;

	cairo_surface_flush (surface);
	return cairo_image_surface_get_data (surface);
}

u_char CAIRO_RED, CAIRO_GREEN, CAIRO_BLUE, CAIRO_ALPHA;

/* Performs a simple 2D Gaussian blur of radius @radius on surface @surface. */
void blur_image_surface2 (cairo_surface_t *surface, int radius) {
    cairo_surface_t *tmp;
    int width, height;
    int src_stride, dst_stride;
    int x, y, z, w;
    uint8_t *src, *dst;
    uint32_t *s, *d, a, p;
    int i, j, k;
    uint8_t kernel[17];
    const int size = ARRAY_LENGTH (kernel);
    const int half = size / 2;

    if (cairo_surface_status (surface))
    return;

    width = cairo_image_surface_get_width (surface);
    height = cairo_image_surface_get_height (surface);

    switch (cairo_image_surface_get_format (surface)) {
    case CAIRO_FORMAT_A1:
    default:
    /* Don't even think about it! */
    return;

    case CAIRO_FORMAT_A8:
    /* Handle a8 surfaces by effectively unrolling the loops by a
     * factor of 4 - this is safe since we know that stride has to be a
     * multiple of uint32_t. */
    width /= 4;
    break;

    case CAIRO_FORMAT_RGB24:
    case CAIRO_FORMAT_ARGB32:
    break;
    }

    tmp = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    if (cairo_surface_status (tmp))
    return;

    src = cairo_image_surface_get_data (surface);
    src_stride = cairo_image_surface_get_stride (surface);

    dst = cairo_image_surface_get_data (tmp);
    dst_stride = cairo_image_surface_get_stride (tmp);

    a = 0;
    for (i = 0; i < size; i++) {
    double f = i - half;
    a += kernel[i] = exp (- f * f / 30.0) * 80;
    }

    /* Horizontally blur from surface -> tmp */
    for (i = 0; i < height; i++) {
    s = (uint32_t *) (src + i * src_stride);
    d = (uint32_t *) (dst + i * dst_stride);
    for (j = 0; j < width; j++) {
        if (radius < j && j < width - radius) {
        d[j] = s[j];
        continue;
        }

        x = y = z = w = 0;
        for (k = 0; k < size; k++) {
        if (j - half + k < 0 || j - half + k >= width)
            continue;

        p = s[j - half + k];

        x += ((p >> 24) & 0xff) * kernel[k];
        y += ((p >> 16) & 0xff) * kernel[k];
        z += ((p >>  8) & 0xff) * kernel[k];
        w += ((p >>  0) & 0xff) * kernel[k];
        }
        d[j] = (x / a << 24) | (y / a << 16) | (z / a << 8) | w / a;
    }
    }

    /* Then vertically blur from tmp -> surface */
    for (i = 0; i < height; i++) {
    s = (uint32_t *) (dst + i * dst_stride);
    d = (uint32_t *) (src + i * src_stride);
    for (j = 0; j < width; j++) {
        if (radius <= i && i < height - radius) {
        d[j] = s[j];
        continue;
        }

        x = y = z = w = 0;
        for (k = 0; k < size; k++) {
        if (i - half + k < 0 || i - half + k >= height)
            continue;

        s = (uint32_t *) (dst + (i - half + k) * dst_stride);
        p = s[j];

        x += ((p >> 24) & 0xff) * kernel[k];
        y += ((p >> 16) & 0xff) * kernel[k];
        z += ((p >>  8) & 0xff) * kernel[k];
        w += ((p >>  0) & 0xff) * kernel[k];
        }
        d[j] = (x / a << 24) | (y / a << 16) | (z / a << 8) | w / a;
    }
    }

    cairo_surface_destroy (tmp);
    cairo_surface_mark_dirty (surface);
}
void box_blur (cairo_surface_t *source, cairo_surface_t *destination, int radius, u_char *div_kernel_size) {
	int     width, height, src_rowstride, dest_rowstride;
	u_char *p_src, *p_dest, *c1, *c2;
	int     x, y, i, i1, i2, width_minus_1, height_minus_1, radius_plus_1;
	int     r, g, b, a;
	u_char *p_dest_row, *p_dest_col;

	width = cairo_image_surface_get_width (source);
	height = cairo_image_surface_get_height (source);
	radius_plus_1 = radius + 1;

	/* horizontal blur */

	p_src = _cairo_image_surface_flush_and_get_data (source);
	p_dest = _cairo_image_surface_flush_and_get_data (destination);
	src_rowstride = cairo_image_surface_get_stride (source);
	dest_rowstride = cairo_image_surface_get_stride (destination);
	width_minus_1 = width - 1;
	for (y = 0; y < height; y++) {

		/* calculate the initial sums of the kernel */

		r = g = b = a = 0;

		for (i = -radius; i <= radius; i++) {
			c1 = p_src + (CLAMP (i, 0, width_minus_1) * 4);
			r += c1[CAIRO_RED];
			g += c1[CAIRO_GREEN];
			b += c1[CAIRO_BLUE];
			/*if (n_channels == 4)
				a += c1[CAIRO_ALPHA];*/
		}

		p_dest_row = p_dest;
		for (x = 0; x < width; x++) {
			/* set as the mean of the kernel */

			p_dest_row[CAIRO_RED] = div_kernel_size[r];
			p_dest_row[CAIRO_GREEN] = div_kernel_size[g];
			p_dest_row[CAIRO_BLUE] = div_kernel_size[b];
			p_dest_row[CAIRO_ALPHA] = 0xff;
			/*if (n_channels == 4)
				p_dest_row[CAIRO_ALPHA] = div_kernel_size[a];*/
			p_dest_row += 4;

			/* the pixel to add to the kernel */

			i1 = x + radius_plus_1;
			if (i1 > width_minus_1)
				i1 = width_minus_1;
			c1 = p_src + (i1 * 4);

			/* the pixel to remove from the kernel */

			i2 = x - radius;
			if (i2 < 0)
				i2 = 0;
			c2 = p_src + (i2 * 4);

			/* calculate the new sums of the kernel */

			r += c1[CAIRO_RED] - c2[CAIRO_RED];
			g += c1[CAIRO_GREEN] - c2[CAIRO_GREEN];
			b += c1[CAIRO_BLUE] - c2[CAIRO_BLUE];
			/*if (n_channels == 4)
				a += c1[CAIRO_ALPHA] - c2[CAIRO_ALPHA];*/
		}

		p_src += src_rowstride;
		p_dest += dest_rowstride;
	}
	cairo_surface_mark_dirty (destination);

	/* vertical blur */

	p_src = _cairo_image_surface_flush_and_get_data (destination);
	p_dest = _cairo_image_surface_flush_and_get_data (source);
	src_rowstride = cairo_image_surface_get_stride (destination);
	dest_rowstride = cairo_image_surface_get_stride (source);
	height_minus_1 = height - 1;
	for (x = 0; x < width; x++) {
		/* calculate the initial sums of the kernel */

		r = g = b = a = 0;

		for (i = -radius; i <= radius; i++) {
			c1 = p_src + (CLAMP (i, 0, height_minus_1) * src_rowstride);
			r += c1[CAIRO_RED];
			g += c1[CAIRO_GREEN];
			b += c1[CAIRO_BLUE];
			/*if (n_channels == 4)
				a += c1[CAIRO_ALPHA];*/
		}

		p_dest_col = p_dest;
		for (y = 0; y < height; y++) {
			/* set as the mean of the kernel */

			p_dest_col[CAIRO_RED] = div_kernel_size[r];
			p_dest_col[CAIRO_GREEN] = div_kernel_size[g];
			p_dest_col[CAIRO_BLUE] = div_kernel_size[b];
			p_dest_col[CAIRO_ALPHA] = 0xff;
			/*if (n_channels == 4)
				p_dest_row[CAIRO_ALPHA] = div_kernel_size[a];*/
			p_dest_col += dest_rowstride;

			/* the pixel to add to the kernel */

			i1 = y + radius_plus_1;
			if (i1 > height_minus_1)
				i1 = height_minus_1;
			c1 = p_src + (i1 * src_rowstride);

			/* the pixel to remove from the kernel */

			i2 = y - radius;
			if (i2 < 0)
				i2 = 0;
			c2 = p_src + (i2 * src_rowstride);

			/* calculate the new sums of the kernel */

			r += c1[CAIRO_RED] - c2[CAIRO_RED];
			g += c1[CAIRO_GREEN] - c2[CAIRO_GREEN];
			b += c1[CAIRO_BLUE] - c2[CAIRO_BLUE];
			/*if (n_channels == 4)
				a += c1[CAIRO_ALPHA] - c2[CAIRO_ALPHA];*/
		}

		p_src += 4;
		p_dest += 4;
	}
	cairo_surface_mark_dirty (source);
}
void _cairo_image_surface_box_blur (cairo_surface_t *source, int radius) {
	long             kernel_size;
	u_char          *div_kernel_size;
	int              i;
	cairo_surface_t *tmp;

	kernel_size = 2 * radius + 1;

	/* optimization to avoid divisions: div_kernel_size[x] == x / kernel_size */
	div_kernel_size = calloc(256 * kernel_size, sizeof(u_char));
	if (!div_kernel_size) { perror("calloc div_kernel_size"); exit(EXIT_FAILURE); }

	for (i = 0; i < 256 * kernel_size; i++)
		div_kernel_size[i] = (u_char) (i / kernel_size);

	tmp = _cairo_image_surface_create_compatible (source);
	box_blur (source, tmp, radius, div_kernel_size);

	cairo_surface_destroy (tmp);
	free(div_kernel_size);
}
void blur_image(double r) {
	if (!syq) return;

	_cairo_image_surface_box_blur(current_surface, r);
}
void flush_image() {
	if (!syq) return;

	cairo_surface_flush(current_surface);
}
void font_bounds(ihaatahf *ih) {
	if (!ih || !syq) return;

	cairo_font_extents_t fe;
	cairo_font_extents (syq, &fe);

	ih->x = fe.max_x_advance;
	ih->y = fe.max_y_advance;
	ih->s = fe.ascent;
	ih->w = fe.descent;
	ih->h = fe.height;
}
void harf_hajm(ihaatahf *ih, int c) {
	if (!ih || !syq) return;

	cairo_text_extents_t te;
	char str[4]; // not doing this conversion bugs cairo :/
	if (uni2chars(str, fushatarmeez.tounicode[c], 4)) {
		cairo_text_extents (syq, str, &te);

		ih->x = te.x_advance;
		ih->s = te.x_bearing;
		ih->y = te.y_bearing;
		ih->w = te.width;
		ih->h = te.height;
	}
}
void huroof_hajm(ihaatahf *ih, int *c) {
	if (!ih || !syq) return;

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
		cairo_text_extents (syq, str, &te);
//		printf("%s %f\n", str, te.height);

		ih->x = te.x_advance;
		ih->y = te.y_bearing;
		ih->w = te.width;
		ih->h = te.height;
	}

	free(str);
}
void text_bounds(ihaatahf *ih, char *c) {
	if (!ih || !syq) return;

	cairo_text_extents_t te;
	cairo_text_extents (syq, c, &te);

	ih->x = te.x_advance;
	ih->y = te.y_bearing;
	ih->w = te.width;
	ih->h = te.height;
}
void matn_harf(int c) {
	if (!syq) return;

	char str[4];
	if (uni2chars(str, fushatarmeez.tounicode[c], 4))
		cairo_show_text(syq, str);
}
void matn_huroof(int *c) {
	if (!syq) return;

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
		cairo_show_text(syq, str);
	
	free(str);
}
void xat_wijhah(const char *c) { // add BOLD, ITALIC
	if (!c || !syq) return;

	cairo_select_font_face(syq, c, 0, 0);
}
void font_size(float qadd) {
	if (!syq) return;
	cairo_set_font_size(syq, qadd);
}
void mustateel(ihaatahf ih) {
	if (!syq) return;
	
	cairo_rectangle(syq, ih.x, ih.y, ih.w, ih.h);
}
void circle(float x, float y, float r, float sa, float se) {
	if (!syq) return;
	
	cairo_arc(syq, x, y, r, toradians(sa), toradians(se));
}
void move_to(float x, float y) {
	if (!syq) return;
	
	cairo_move_to(syq, x, y);
}
void satar(float x, float y) {
	if (!syq) return;
	
	cairo_line_to(syq, x, y);
}
void set_dash(double *dashes, int num_dashes, double offset) {
	if (!syq) return;

	cairo_set_dash (syq, dashes, num_dashes, offset);
}
void stroke(char preserve) {
	if (!syq) return;
	
	if (preserve) cairo_stroke_preserve(syq);
	else cairo_stroke(syq);
}
void fill(char preserve) {
	if (!syq) return;
	
	if (preserve) cairo_fill_preserve(syq);
	else cairo_fill(syq);
}
void clip() { // clip
	if (syq) cairo_clip (syq);
}
void unclip() { // clip
	if (syq) cairo_reset_clip (syq);
}
void scale(float x, float y) {
	if (syq) cairo_scale (syq, x, y);
}
void rotate(float x) {
	if (syq) cairo_rotate (syq, toradians(x));
}
void translate(float x, float y) {
	if (syq) cairo_translate (syq, x, y);
}
void user_to_device(double *x, double *y) {
	if (syq) cairo_user_to_device (syq, x, y);
}
void device_to_user(double *x, double *y) {
	if (syq) cairo_device_to_user (syq, x, y);
}
void renew_path() {
	if (syq) cairo_new_path (syq);
}
void paint() {
	if (syq) cairo_paint (syq);
}
void lwn_rgb(float r, float g, float b) {
	if (!syq) return;
	
	cairo_set_source_rgb(syq, r, g, b);
}
void lwn_rgba(float r, float g, float b, float a) {
	if (!syq) return;
	
	cairo_set_source_rgba(syq, r, g, b, a);
}
void line_width(float l) {
	if (!syq) return;
	
	cairo_set_line_width(syq, l);
}
void show_text(char *str) {
	if (!syq) return;
	
	cairo_show_text(syq, str);
}
void save() {
	if (!syq) return;
	
	cairo_save(syq);
}
void restore() {
	if (!syq) return;
	
	cairo_restore(syq);
}
void set_pixel(u_int x, u_int y, u_char a, u_char r, u_char g, u_char b) {
	if (current_matrix) {
		u_int argb = 0;
		components_to_argb(a, r, g, b, &argb);
		if (a == 255) msfoof_set(current_matrix, x, y, argb);
		else if (a > 0) msfoof_set_blend(current_matrix, x, y, argb, (float) a/255.0);
	}
}
//}

float composer_ease_by_name(int i, float t, float b, float c, float d) {
	float (*func)(float, float, float, float) = NULL;
	if (i ==  0) func = EaseLinearNone	;
	if (i ==  1) func = EaseLinearIn	;
	if (i ==  2) func = EaseLinearOut	;
	if (i ==  3) func = EaseLinearInOut	;
	if (i ==  4) func = EaseSineIn		;
	if (i ==  5) func = EaseSineOut		;
	if (i ==  6) func = EaseSineInOut	;
	if (i ==  7) func = EaseCircIn		;
	if (i ==  8) func = EaseCircOut		;
	if (i ==  9) func = EaseCircInOut	;
	if (i == 10) func = EaseCubicIn		;
	if (i == 11) func = EaseCubicOut	;
	if (i == 12) func = EaseCubicInOut	;
	if (i == 13) func = EaseQuadIn		;
	if (i == 14) func = EaseQuadOut		;
	if (i == 15) func = EaseQuadInOut	;
	if (i == 16) func = EaseExpoIn		;
	if (i == 17) func = EaseExpoOut		;
	if (i == 18) func = EaseExpoInOut	;
	if (i == 19) func = EaseBackIn		;
	if (i == 20) func = EaseBackOut		;
	if (i == 21) func = EaseBackInOut	;
	if (i == 22) func = EaseBounceIn	;
	if (i == 23) func = EaseBounceOut	;
	if (i == 24) func = EaseBounceInOut	;
	if (i == 25) func = EaseElasticIn	;
	if (i == 26) func = EaseElasticOut	;
	if (i == 27) func = EaseElasticInOut;
	
	if (func) return func(t, b, c, d);
	return 0;
}

void composer_canvas(char yes) {
	use_canvas = yes;
}
void composer_canvas_context_window() {
	if (surfaceraees) cairo_surface_destroy(surfaceraees);
	if (cairaees) cairo_destroy(cairaees);

	surfaceraees = cairo_image_surface_create_for_data (
		(u_char *) WJHH.raees.mtrx.barzax,
		CAIRO_FORMAT_ARGB32,
		WJHH.raees.mtrx.w, WJHH.raees.mtrx.h, WJHH.raees.mtrx.stride
	);
	if (cairo_surface_status (surfaceraees) != CAIRO_STATUS_SUCCESS) {
		printf ("couldn't create cairo surface\n"); exit(EXIT_FAILURE);
	}
	
	current_context = cairaees = cairo_create (surfaceraees);
	if (cairo_status (current_context) != CAIRO_STATUS_SUCCESS) {
		printf ("couldn't create cairo context\n"); exit(EXIT_FAILURE);
	}

	current_context = syq = cairaees;
	current_matrix = &WJHH.raees.mtrx;
	current_surface = surfaceraees;
}
void composer_canvas_context_indicator() {
	if (surfacemshr) cairo_surface_destroy(surfacemshr);
	if (caimshr) cairo_destroy(caimshr);

	surfacemshr = cairo_image_surface_create_for_data (
		(u_char *) WJHH.mshr.mtrx.barzax,
		CAIRO_FORMAT_ARGB32,
		WJHH.mshr.mtrx.w,
		WJHH.mshr.mtrx.h,
		WJHH.mshr.mtrx.stride
	);
	caimshr = cairo_create (surfacemshr);
}

void composer_load_app(char i) { // get app
	if (trkb.on_reload) trkb.on_reload();
	/*if (J == NULL) return;
	eqonah_nazaf(&eqnh); // sets .id to -1
	eqnh.hudood.s = 1;

	// TODO havaf all kaatibeen
	js_dofile(J, "apps/eventcodes.js");
	js_dofile(J, trkb.xitaab);*/
	// redo events
	if (i && trkb.on_bound_window  ) {
		composer_canvas_context_window();
		trkb.on_bound_window  ((ihaatahf) {0, 0, WJHH.raees.mtrx.w, WJHH.raees.mtrx.h});
	}
	if (i && trkb.on_bound_indicator) {
		composer_canvas_context_indicator();
		trkb.on_bound_indicator((ihaatahf) {0, 0, WJHH.mshr .mtrx.w, WJHH.mshr .mtrx.h});
	}
	// TODO BUG fix, after js reload, doesn't damage
	WJHH.raees.mutadarrar = 1;
	WJHH.mshr.mutadarrar = 1;
}
void composer_tamaam() { // end, finish, destroy
//    if (J) js_freestate(J);
}
void composer_kaatib_paint(char force) {
	kaatib_idhan(&ktb, *current_matrix, force);
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
int composer_handler(waaqi3ah *w, mafateeh *mif) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);
		if (w->ism == MUDEER && w->miftaah == RAKKAZ) {
			if (trkb.on_focus) trkb.on_focus(w->qadr);
		}
		if (w->ism == MUDEER && w->miftaah == ISHAARAH) {
			char yes = 0;
			mafateeh m = { 0 };
			if (mif) m = *mif; else str2mafateeh(w->qadrstr, &m);
			
//			printf("[%d] %d %.1f %.1f\n", m.state, m.key, m.x, m.y);

			if (!yes && trkb.on_pointer) {
				yes = trkb.on_pointer(m);
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
			if (mif) m = *mif; else str2mafateeh(w->qadrstr, &m);

//			printf("[%d] c%d s%d a%d m%d %d %s\n", m.state, m.ctrl, m.shift, m.alt, m.meta, m.key, m.ism);

			if (!m.state) {
				if (m.ctrl) {
					if (m.key == KEY_R) composer_load_app(1), yes = 1;
					if (m.key == KEY_Q) exit(0), yes = 1;
				}
			}
			if (!yes && trkb.b_lowhah) {
				yes = trkb.b_lowhah(m);
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
				int width = WJHH.mshr.mtrx.w, height = WJHH.mshr.mtrx.h;
				syq = caimshr;

				composer_canvas_context_indicator();

				/*cairo_set_source_rgb( caimshr, 0, 0, 0 );
				cairo_paint (caimshr);

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
				if (axis > width) pct = width / axis;
				else pct = axis / width;

				cairo_scale (caimshr, pct, pct);
				cairo_set_source_surface (caimshr, image, 0, 0);
				cairo_paint (caimshr);
				cairo_surface_flush(surfacemshr);
				
				cairo_surface_destroy(image);*/

//				WJHH.mshr.mtrx.smooth = 1;
				if (w->qadr == 1)
					if (trkb.on_bound_indicator) trkb.on_bound_indicator( (ihaatahf) {0, 0, width, height} );
			}
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				syq = caimshr;
				current_matrix = &WJHH.mshr.mtrx;
				current_surface = surfacemshr;
				if (WJHH.mshr.mutadarrar) {
					char darar = 0;
					// if has C func override, call that first
					if (trkb.on_paint_indicator) darar = trkb.on_paint_indicator();

					if (darar) {
						cairo_surface_flush(surfacemshr);
						
						if (MUDEERUID > -1) // IMPORTANT when not on nizaam-waajihah backend
						amr_irsal_ilaa(MUDEERUID, MUSHEER, DARAR, 1);
					}
					
					if (kaatib_mutadarrar(&ktb)) darar = 2;

					// if animating, return 2 until anim is complete
					// this keeps on_paint_* being called until you'r done
					if (darar != 2) WJHH.mshr.mutadarrar = 0;
				}
			}
		}
		if (w->ism == RAEES) {
			if (w->miftaah == MST3D) {
				int width = WJHH.raees.mtrx.w, height = WJHH.raees.mtrx.h;
				
				composer_canvas_context_window();

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
				
				if (w->qadr == 1)
					if (trkb.on_bound_window) trkb.on_bound_window( (ihaatahf) {0, 0, width, height} );
			}
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
				current_context = syq = cairaees;
				current_matrix = &WJHH.raees.mtrx;
				current_surface = surfaceraees;
				if (WJHH.raees.mutadarrar) {
					char darar = 0;
					// if has C func override, call that first
					if (trkb.on_paint_window) darar = trkb.on_paint_window();

					if (darar) {
						cairo_surface_flush(surfaceraees);

						if (trkb.after_canvas_raees)
							trkb.after_canvas_raees(&WJHH.raees.mtrx);

						if (MUDEERUID > -1) // IMPORTANT when not on nizaam-waajihah backend
						amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
					}
					
					if (kaatib_mutadarrar(&ktb)) darar = 2;

					// if animating, return 2 until anim is complete
					// this keeps on_paint_* being called until you'r done
					if (darar != 2) WJHH.raees.mutadarrar = 0;
				}
			}
		}
	}

	return 0;
}
void composer_init(composer t) { // 
	// TODO this comes from helpers, prepend helpers_ to this func to help rmmbr
//	taba3_waqt("badaa");

	if (!IS_BIG_ENDIAN) { // BGRA
		CAIRO_RED   = 2;
		CAIRO_GREEN = 1;
		CAIRO_BLUE  = 0;
		CAIRO_ALPHA = 3;
	} else if (IS_BIG_ENDIAN) { // ARGB
		CAIRO_RED   = 1;
		CAIRO_GREEN = 2;
		CAIRO_BLUE  = 3;
		CAIRO_ALPHA = 0;
	} else { // RABG
		CAIRO_RED   = 0;
		CAIRO_GREEN = 3;
		CAIRO_BLUE  = 2;
		CAIRO_ALPHA = 1;
	}


	trkb.ism					= t.ism					;
	trkb.xitaab					= t.xitaab				;
	trkb.b_lowhah				= t.b_lowhah			;
	trkb.on_pointer				= t.on_pointer			;
	trkb.on_reload				= t.on_reload			;
	trkb.on_paint_indicator		= t.on_paint_indicator	;
	trkb.on_paint_window		= t.on_paint_window		;
	trkb.on_bound_indicator		= t.on_bound_indicator	;
	trkb.on_bound_window		= t.on_bound_window		;
	trkb.after_canvas_raees		= t.after_canvas_raees	;
	trkb.on_focus				= t.on_focus			;
	trkb.set_window				= t.set_window			;
	trkb.event_provider			= t.event_provider		;

	tarmeez_core();
	kaatib_init(&ktb); // will do matn -> huroof

	// setup cavnas wrapper
	qmc.flush			= &flush_image		;
	qmc.blur			= &blur_image		;
	qmc.font_bounds		= &font_bounds		;
	qmc.harf_hajm		= &harf_hajm		;
	qmc.huroof_hajm		= &huroof_hajm		;
	qmc.text_bounds		= &text_bounds		;
	qmc.matn_harf		= &matn_harf		;
	qmc.matn_huroof		= &matn_huroof		;
	qmc.xat_wijhah		= &xat_wijhah		;
	qmc.font_size		= &font_size		;
	qmc.mustateel		= &mustateel		;
	qmc.circle			= &circle			;
	qmc.lwn_rgb			= &lwn_rgb			;
	qmc.lwn_rgba		= &lwn_rgba			;
	qmc.move_to			= &move_to			;
	qmc.satar			= &satar			;
	qmc.fill			= &fill				;
	qmc.stroke			= &stroke			;
	qmc.renew_path		= &renew_path		;
	qmc.paint			= &paint			;
	qmc.line_width		= &line_width		;
	qmc.show_text		= &show_text		;
	qmc.save			= &save				;
	qmc.restore			= &restore			;
	qmc.clip			= &clip				;
	qmc.unclip			= &unclip			;
	qmc.scale			= &scale			;
	qmc.rotate			= &rotate			;
	qmc.translate		= &translate		;
	qmc.user_to_device	= &user_to_device	;
	qmc.device_to_user	= &device_to_user	;
	qmc.set_dash		= &set_dash			;
	qmc.set_pixel		= &set_pixel		;
	
	current_canvas = &qmc;
	
	ktb.qmc = &qmc; // attach canvas wrapper/driver/bridge

	if (trkb.set_window) {
		trkb.set_window(trkb.ism);
	} else {
		amr_towheed(trkb.ism);
		amr_wijhah(1);
	}
	
	composer_load_app(0);

	if (trkb.event_provider) {
		trkb.event_provider( &composer_handler );
	} else {
		amr_axath(&composer_handler);
	}
	
	composer_tamaam();
}

