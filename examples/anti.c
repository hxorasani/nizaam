/* anti: 
 * TODO:
 * 	give amr a hint that we want musheer and raees before connection
 * 	then amr should handle all the actual logic and errors
 * */

#include "anti.h"

#define XATAA 0

int musheerdrawn = 0, started = 0, mashghool = 0;
struct timespec waqt; kaatib ktb;
cairo_surface_t *surface;
cairo_t *cr;

void set_sourcegradient() {
	cairo_pattern_t *radpat, *linpat;

	/* Drawing code goes here */
	radpat = cairo_pattern_create_radial (0.25, 0.25, 0.1,  0.5, 0.5, 0.5);
	cairo_pattern_add_color_stop_rgb (radpat, 0,  1.0, 0.8, 0.8);
	cairo_pattern_add_color_stop_rgb (radpat, 1,  0.9, 0.0, 0.0);

	int i, j;
	for (i=1; i<10; i++)
		for (j=1; j<10; j++)
			cairo_rectangle (cr, i/10.0 - 0.04, j/10.0 - 0.04, 0.08, 0.08);
	cairo_set_source (cr, radpat);
	cairo_fill (cr);

	linpat = cairo_pattern_create_linear (0.25, 0.35, 0.75, 0.65);
	cairo_pattern_add_color_stop_rgba (linpat, 0.00,  1, 1, 1, 0);
	cairo_pattern_add_color_stop_rgba (linpat, 0.25,  0, 1, 0, 0.5);
	cairo_pattern_add_color_stop_rgba (linpat, 0.50,  1, 1, 1, 0);
	cairo_pattern_add_color_stop_rgba (linpat, 0.75,  0, 0, 1, 0.5);
	cairo_pattern_add_color_stop_rgba (linpat, 1.00,  1, 1, 1, 0);

	cairo_rectangle (cr, 0.0, 0.0, 1, 1);
	cairo_set_source (cr, linpat);
	cairo_fill (cr);

	cairo_set_source (cr, linpat);
	cairo_pattern_destroy (linpat);

	cairo_mask (cr, radpat);
	cairo_pattern_destroy (radpat);
}
void mask() {
	cairo_pattern_t *linpat, *radpat;

	/* Drawing code goes here */
	linpat = cairo_pattern_create_linear (0, 0, 1, 1);
	cairo_pattern_add_color_stop_rgb (linpat, 0, 0, 0.3, 0.8);
	cairo_pattern_add_color_stop_rgb (linpat, 1, 0, 0.8, 0.3);

	radpat = cairo_pattern_create_radial (0.5, 0.5, 0.25, 0.5, 0.5, 0.75);
	cairo_pattern_add_color_stop_rgba (radpat, 0, 0, 0, 0, 1);
	cairo_pattern_add_color_stop_rgba (radpat, 0.5, 0, 0, 0, 0);

	cairo_set_source (cr, linpat);
	cairo_pattern_destroy (linpat);

	cairo_mask (cr, radpat);
	cairo_pattern_destroy (radpat);
}
void drawraees() {
	msfoof *mshr = &WJHH.mshr.mtrx;
	msfoof *r = &WJHH.raees.mtrx;
//	msfoof_clear(r, 0x333333);
	msfoof_clear(r, 0);
//	msfoof_rect(r, 0, 0, 240, 240, -1, 0);
	int w = WJHH.raees.mtrx.w, h = WJHH.raees.mtrx.h, white = 0xffffff;
	float mw = WJHH.mshr.mtrx.w, mh = WJHH.mshr.mtrx.h;

	cairo_surface_flush(surface);

    int stride = cairo_image_surface_get_stride (surface);
	int width = cairo_image_surface_get_width (surface);
    int height = cairo_image_surface_get_height (surface);
    u_char *dst = cairo_image_surface_get_data (surface);
	u_char *pixel = 0;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			pixel = dst + y*stride + x*4;
			msfoof_set_blend(r, x+10, y+10, argb32torgb( pixel ), 1.0f-( argb32_get_a(pixel) /255.0f));
//			msfoof_set(r, x+10, y+10, argb32torgb( pixel ));
		}
	}

	kaatib_baddal(&ktb, "hi");
	kaatib_idhan(&ktb, *r, 0);

	started = 1;
}
void drawmusheer() {
	if (!musheerdrawn) {
		msfoof *m = &WJHH.mshr.mtrx;
		float w = WJHH.mshr.mtrx.w, h = WJHH.mshr.mtrx.h;

	}
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		if (w->ism == MUDEER && w->miftaah == LOWHAH && !WJHH.raees.mutadarrar && !WJHH.mshr.mutadarrar) {
			char yes = 0; float delta = .25;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			if (m.state) {
				if (m.key == KEY_S) yes = 1;
				if (m.key == KEY_Q) exit(0);
			}
			if (yes) {
				WJHH.mshr.mutadarrar = 1;
				WJHH.raees.mutadarrar = 1;
				mashghool = 1;
			}
		}
		if (w->ism == MUSHEER) {
			if (w->miftaah == MST3D) {
				WJHH.mshr.mtrx.smooth = 1;
			}
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				if (WJHH.mshr.mutadarrar) {
					WJHH.mshr.mutadarrar = 0;
					drawmusheer();
					amr_irsal_ilaa(MUDEERUID, MUSHEER, DARAR, 1);
				}
			}
		}
		if (w->ism == RAEES) {
			if (w->miftaah == MST3D) {
				WJHH.raees.mtrx.smooth = 1;
				WJHH.raees.mutadarrar = 1;
				if (w->qadr == 1 && ktb.mst3d == 0) {
//					ktb.g.y = 480;
					ktb.g.w = WJHH.raees.mtrx.w;
					ktb.g.h = WJHH.raees.mtrx.h;
					ktb.mtrx = WJHH.raees.mtrx;
					kaatib_xataa(&ktb);
					ktb.mst3d = 1;
				} else {
					ktb.mst3d = 0;
				}
			}
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
				if (WJHH.raees.mutadarrar) {
					WJHH.raees.mutadarrar = 0;
					drawraees();
					amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
				}
			}
		}
	}

	return 0;
}
int main(int argc, char **argv) {
	int w = 120, h = 120;
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
//	surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, w, h);
	cr = cairo_create (surface);

	cairo_pattern_t *linpat, *radpat;

    cr = cairo_create (surface);
	/* Examples are in 1.0 x 1.0 coordinate space */
	cairo_scale (cr, w, h);

	set_sourcegradient();
	mask();

//	cairo_set_source_rgb (cr, 0, 0, 255);
//	cairo_rectangle (cr, .25, .25, .5, .5);
//	cairo_fill (cr);
//
//
//	cairo_text_extents_t te;
//	cairo_set_source_rgb (cr, 255.0, 0.0, 0.0);
//	cairo_select_font_face (cr, "Georgia",
//		CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//	cairo_set_font_size (cr, 1.2);
//	cairo_text_extents (cr, "a", &te);
//	cairo_move_to (cr, 0.5 - te.width / 2 - te.x_bearing,
//		0.5 - te.height / 2 - te.y_bearing);
//	cairo_show_text (cr, "a");
//
//
////	cairo_set_source_rgb (cr, 0.0, 255.0, 255.0);
//	cairo_paint_with_alpha (cr, 0);
//	cairo_paint (cr);

	tarmeez_core();

	kaatib_init(&ktb);
	ktb.hrfmsfh = 3;
	ktb.g.s = 1.5;
	ktb.cursor = -2;
	ktb.taht = 0;


	amr_towheed("anti");
	amr_wijhah(1);


	amr_axath(&handler);
	

    return EXIT_SUCCESS;
}
