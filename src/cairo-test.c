/* canvas: 
 * msfoof is too low-level, canvas is higher level
 * it should serve as the basis for a GUI toolkit
 * it depends on cairo, and should optionally support hardware acceleration
 * it should allow easy animation
 * image and video editing can be built on top of it
 * but in essence it should remain as simple and lightweight as possible
 * 
 * TODO:
 * 	
 * 	
 * */

#include "cairo-test.h"

#define XATAA 0

int musheerdrawn = 0, mashghool = 0, px = 300, py = 300, pw = 10, drag = 0;
float rotang = 0, zoom = 1;
char goup = 0, godown = 0, goleft = 0, goright = 0;
int lx = 0, ly = 0;
float linewidth = 1;
cairo_surface_t *surface = NULL;
cairo_t *cr = NULL;
cairo_surface_t *surface2 = NULL;
cairo_t *cr2 = NULL;
cairo_t *syq = NULL;
cairo_surface_t *loadedjpg = NULL;


/* GIVE THIS A TRY SOMETIME
 * and btw cairo has rect list struct built in for dynamic clipping!
cairo_public cairo_surface_t *
cairo_image_surface_create_for_data (unsigned char	       *data,
				     cairo_format_t		format,
				     int			width,
				     int			height,
				     int			stride);*/

struct timespec waqt; long since = 0; int frame = 0, fps = 0;
long beat(long howlong) {
	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	long farq = sec2ms(aan.tv_sec-waqt.tv_sec) + nano2ms(aan.tv_nsec-waqt.tv_nsec);
	if (since == 0) since = farq;
	if (farq-since >= howlong) {
		since = 0;
		return farq-since;
	} else return 0;
}


void matn_huroof(int *c) {
	if (!cr) return;

	int len = wcslen(c);
	for (int i = 0; i < len; ++i) {
		c[i] = fushatarmeez.tounicode[ c[i] ];
	}

	char *str = calloc(len*4, sizeof(char));
	if (str == NULL) {
		perror("matn_huroof calloc"); exit(EXIT_FAILURE);
	}

	if (unis2chars(str, c, len))
		cairo_show_text(cr, str);
	
	free(str);
}
void circle(float x, float y, float r, int bg, int fg) {
	cairo_arc (cr, x, y, r, 0, toradians(360));

	if (bg != -1) {
		lwnf b; rgb2lwnf(bg, &b);
		cairo_set_source_rgb (cr, b.r, b.g, b.b);
		cairo_fill_preserve (cr);
	}

	if (fg != -1) {
		lwnf f; rgb2lwnf(fg, &f);
		cairo_set_source_rgb (cr, f.r, f.g, f.b);
		cairo_stroke (cr);
	}
}
void dotext(float x, float y, char *str) {
	cairo_set_source_rgba (cr, 1, 1, 1, 1);
//	cairo_set_source_rgba (cr, 1, 1, .99, 1);
	cairo_set_font_size (cr, 36);

	cairo_text_extents_t te;
	cairo_font_extents_t fe;
	cairo_text_extents (cr, str, &te);
	cairo_font_extents (cr, &fe);
//	cairo_move_to (cr, px + (0.5 - te.width / 2 - te.x_bearing), py + (0.5 - te.height / 2 - te.y_bearing));
	cairo_move_to (cr, x, y+fe.height);
	cairo_show_text (cr, str);
	cairo_new_path (cr);
}
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
void rect(float x, float y, float w, float h, int bg, int fg) {
	cairo_set_line_width (cr, 2);
	cairo_rectangle (cr, x, y, w, h);

	lwnf b; rgb2lwnf(bg, &b);
	cairo_set_source_rgba (cr, b.r, b.g, b.b, b.a);
	cairo_fill_preserve (cr);

	lwnf f; rgb2lwnf(fg, &f);
	cairo_set_source_rgba (cr, f.r, f.g, f.b, f.a);
	cairo_stroke (cr);
}
void image(float x, float y, float w, float h, const char *path) {
	if (!syq) return;
	cairo_save (syq);
	cairo_surface_t *image;
	if (loadedjpg) {
		image = loadedjpg;
	} else {
		loadedjpg = image = cairo_image_surface_create_from_jpeg (path);
	}
	int imgw = cairo_image_surface_get_width (image);
	int imgh = cairo_image_surface_get_height (image);

	float pct = 1, axis = imgh;
	if (imgw > imgh) axis = imgw; // fit
//	if (imgw < imgh) axis = imgw; // cover
	if (axis > w) pct = w / axis * zoom;
	else pct = axis / w * zoom;

//	cairo_translate (syq, px, py);

	cairo_translate (syq, imgw, imgh);
	cairo_scale (syq, pct, pct);
	cairo_translate (syq, -(imgw/pct) - imgw/2, -(imgh/pct) - imgh/2);

//	cairo_rotate (syq, rotang * M_PI/180.0);
//	rotang+=.25;

	cairo_set_source_surface (syq, image, px/pct, py/pct);

	cairo_paint (syq);
//	cairo_surface_destroy (image);

	cairo_restore (syq);
}
int get_color() {
	int stride = cairo_image_surface_get_stride (surface);
	u_char *dst = cairo_image_surface_get_data (surface);
	return argb32torgb( dst + ly*stride + lx*4 );
}
char drawraees() {
	cairo_save(cr);
	cairo_select_font_face(cr, "Ubuntu Mono", 0, 0);

	msfoof *r = &WJHH.raees.mtrx;
	int w = WJHH.raees.mtrx.w, h = WJHH.raees.mtrx.h;
	int active = 0;
	nuqtahf nq = { 0 };

	int pixelcolor = get_color();

	cairo_set_source_rgb (cr, .5, .5, .5);
	cairo_paint (cr);

/*
//	circle(px, py);
//	cairo_set_source_rgb (cr, 0, 1, 0);
//	cairo_set_line_width (cr, linewidth);
//	cairo_move_to (cr, lx, ly);
//	cairo_line_to (cr, fx, fy);
//	cairo_stroke (cr);
	*/
	image(w/2, h/2, w, h, "/home/nano/Pictures/Wallpapers/free-beautiful-landscape-desktop-wallpaper-06-2010_1366x768_81822.jpg");


	char *tmpstr = helper_stringify("%.6x %d %d, %d", pixelcolor, w, h, fps);
	dotext(4, 4, tmpstr);
	free(tmpstr);

	cairo_surface_flush(surface);

//	int stride = cairo_image_surface_get_stride (surface);
//	int width  = cairo_image_surface_get_width  (surface);
//	int height = cairo_image_surface_get_height (surface);
//	u_char *dst = cairo_image_surface_get_data  (surface);
//	u_char *pixel = 0;

//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width; ++x) {
//			pixel = dst + y*stride + x*4;
////			msfoof_set_blend(r, x, y, argb32torgb( pixel ), 1.0f-( argb32_get_a(pixel) /255.0f));
//			msfoof_set(r, x, y, argb32torgb( pixel ));
//		}
//	}
	
	if (beat(1000)) {
		fps = frame;
		frame = 0;
	}
	
	++frame;

	cairo_restore(cr);
	return active ? 2 : 1;
}
void drawmusheer() {
	if (!musheerdrawn) {
		msfoof *m = &WJHH.mshr.mtrx;
		int w = WJHH.mshr.mtrx.w, h = WJHH.mshr.mtrx.h;

		cairo_arc (cr2, w/2, h/2, w/2, 0, 2*M_PI);
		cairo_clip (cr2);
		cairo_new_path (cr2); // path not consumed by clip()

		cairo_surface_t *image;
		image = cairo_image_surface_create_from_png ("data/new-logo-t.png");
		int imgw = cairo_image_surface_get_width (image);
		int imgh = cairo_image_surface_get_height (image);

		float pct = 1, axis = imgh;
//		if (imgw > imgh) axis = imgw; // fit
		if (imgw < imgh) axis = imgw; // cover
		if (axis > w) pct = w / axis;
		else pct = axis / w;

		cairo_scale (cr2, pct, pct);

		cairo_set_source_surface (cr2, image, 0, 0);

		cairo_paint (cr2);
		cairo_surface_destroy (image);
		
		cairo_surface_flush(surface2);


		int stride = cairo_image_surface_get_stride (surface2);
		int width = cairo_image_surface_get_width (surface2);
		int height = cairo_image_surface_get_height (surface2);
		u_char *dst = cairo_image_surface_get_data (surface2);
		u_char *pixel = 0;

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				pixel = dst + y*stride + x*4;
				msfoof_set_blend(m, x, y, argb32torgb( pixel ), 1.0f-( argb32_get_a(pixel) /255.0f));
//				msfoof_set(m, x, y, argb32torgb( pixel ));
			}
		}

		musheerdrawn = 1;
	}
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		if (w->ism == MUDEER && w->miftaah == ISHAARAH) {
//			amr_tb3_waaqi3ah(w);
			char yes = 0;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			
			if (drag) {
				if (m.x > lx) px += (m.x-lx);
				if (m.x < lx) px -= (lx-m.x);

				if (m.y > ly) py += (m.y-ly);
				if (m.y < ly) py -= (ly-m.y);
			}

			if (m.state == -1) {
				drag = 1;
			} else if (m.state == 1) {
				drag = 0;
			}

			lx = m.x; ly = m.y;
			yes = 1;
			if (yes) {
				WJHH.mshr.mutadarrar = 1;
				WJHH.raees.mutadarrar = 1;
				mashghool = 1;
			}
		}
		if (w->ism == MUDEER && w->miftaah == LOWHAH) {
			char yes = 0; float delta = .125;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			if (m.state) {
				if (m.key == KEY_LEFT ) goleft  = 1, yes = 1;
				if (m.key == KEY_RIGHT) goright = 1, yes = 1;
				if (m.key == KEY_UP   ) goup    = 1, yes = 1;
				if (m.key == KEY_DOWN ) godown  = 1, yes = 1;
				
				if (m.key == KEY_TAB) {
					if (m.shift) {
						if (targetbody > 0) targetbody--;
						else targetbody = tpe_world.bodyCount-1;
					} else {
						if (targetbody < tpe_world.bodyCount-1) targetbody++;
						else targetbody = 0;
					}
					
					yes = 1;
				}

				if (m.shift) delta = 5;
				if (m.key == KEY_J && zoom > delta) linewidth -= delta, yes = 1;
				if (m.key == KEY_K) linewidth += delta, yes = 1;
				
				if (m.key == KEY_MINUS && zoom > delta) zoom -= delta, yes = 1;
				if (m.key == KEY_EQUAL) zoom += delta, yes = 1;
				
				if (m.key == KEY_S) yes = 1;
				if (m.key == KEY_Q) exit(0);
				
				if (linewidth < 0) linewidth = 0;
			} else {
				if (m.key == KEY_LEFT ) goleft  = 0, yes = 1;
				if (m.key == KEY_RIGHT) goright = 0, yes = 1;
				if (m.key == KEY_UP   ) goup    = 0, yes = 1;
				if (m.key == KEY_DOWN ) godown  = 0, yes = 1;
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
				WJHH.mshr.mutadarrar = 1;

				if (surface2) cairo_surface_destroy(surface2);
				if (cr2) cairo_destroy(cr2);
				surface2 = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WJHH.mshr.mtrx.w, WJHH.mshr.mtrx.h);
				cr2 = cairo_create (surface2);
			}
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				if (WJHH.mshr.mutadarrar) {
					syq = cr2;
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

				if (surface) cairo_surface_destroy(surface);
				if (cr) cairo_destroy(cr);
				
				u_char *mdata = (u_char *) WJHH.raees.mtrx.barzax;
				
				surface = cairo_image_surface_create_for_data (
					mdata,
					CAIRO_FORMAT_ARGB32,
					WJHH.raees.mtrx.w,
					WJHH.raees.mtrx.h,
					WJHH.raees.mtrx.stride
				);
				cr = cairo_create (surface);
			}
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
				if (WJHH.raees.mutadarrar) {
					syq = cr;
					char damage = drawraees();
					if (damage != 2)
						WJHH.raees.mutadarrar = 0;
					amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
				}
			}
		}
	}

	return 0;
}
int main(int argc, char **argv) {

	/*
	// we'll create the first body "by hand", just two joints (spheres) with one
	// connection: 
	tpe_joints[0] = TPE_joint(TPE_vec3(3 * ROOM_SIZE / 4,ROOM_SIZE / 2, 0), TPE_F / 2);
	tpe_joints[1] = 
	TPE_joint(TPE_vec3(3 * ROOM_SIZE / 4 + TPE_F * 4, ROOM_SIZE / 2, 0), TPE_F / 2);
	
	tpe_connections[0].joint1 = 0;
	tpe_connections[0].joint2 = 1;
	
	TPE_bodyInit(&bodies[0], tpe_joints, 2, tpe_connections, 1, TPE_F);
	
	// the other (a "box" approximated by spheres) will be made by the library
	// function:
	TPE_makeBox(tpe_joints + 2, tpe_connections + 1, 2 * TPE_F, 2 * TPE_F, 2 * TPE_F, TPE_F);
	TPE_bodyInit(&bodies[1], tpe_joints + 2, 8, tpe_connections + 1, 16, TPE_F);
	TPE_bodyMoveTo(&bodies[1], TPE_vec3(ROOM_SIZE / 2, ROOM_SIZE / 2, 0));
	
	TPE_tpe_worldInit(&tpe_world, bodies, 2, environmentDistance);
	
	// give some initial velocities and spins to the bodies:
	
	TPE_bodyAccelerate(&tpe_world.bodies[0],TPE_vec3(-1 * TPE_F / 8,TPE_F / 3,0));
	TPE_bodySpin(&tpe_world.bodies[0],TPE_vec3(0,0,-1 * TPE_F / 25));
	TPE_bodyAccelerate(&tpe_world.bodies[1],TPE_vec3(-1 * TPE_F / 2,50,0));
	TPE_bodySpin(&tpe_world.bodies[1],TPE_vec3(0,0,TPE_F / 23));
	*/

	tarmeez_core();

	amr_towheed("canvas");
	amr_wijhah(1);

	amr_axath(&handler);

	cairo_destroy (cr2);
	cairo_surface_destroy (surface2);
	cairo_destroy (cr);
	cairo_surface_destroy (surface);

    return EXIT_SUCCESS;
}
