#include "arc.h"

#define XATAA 0
#define swap_(a, b) do{ __typeof__(a) tmp;  tmp = a; a = b; b = tmp; }while(0)

float ang0 = 0, ang1 = 360, ktbscl = 1.75, mstrokw = .25, msfh = 3;
int musheerdrawn = 0, started = 0, cell = 3, dodo = 0, dirr = 0, mashghool = 0, timess = 0, smoothing = 0;
struct timespec waqt;
kaatib ktb, ktb2; ihaatah glbl = {24, 24, 48, 48, 1};
nuqtahf nqth = {11.5, 0, 1, 3}, nqth2 = {22, 48, 1}, nqth3 = {0, 0, 1};
struct timespec waqt; long since = 0; float qdm;
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
void arc(msfoof *m, float x, float y, float r, float sa, float ea, int o, int f) {
	// center, radius, start-end angle
	if (sa != ea) {
		if (r > 15) qdm = 180.0/r;
		else qdm = 60.0/r;
		x *= 3; y *= 3; r *= 3;
		unsigned char fg = hl2char(0, 15);
//		ihaatah ihth;
		nuqtahf n = {0}, p = {0}, q = {0};
		if (sa > ea) sa = sa - 360.0;
		float sw = nqth.cx;
		float lx = -1, ly = -1;
		for (float a = sa; a <= ea+qdm; a += qdm) {
			angletocoord(x, y, r, a, &n);
			if (lx != -1) {
				if (f != -1) {
					
				}
				if (o != -1) {
					msfoof_line_alif(m, lx, ly, n.x, n.y, fg);
//					ihaatah_ws3(&ihth, n.x, n.y, 0, 0);
					
//					msfoof_set_alif(m, n.x, n.y, fg);
					
					angletocoord(x, y, r-20, a, &p);
					msfoof_set_alif(m, p.x, p.y, fg);
//
//					angletocoord(x, y, r+12, a, &q);
//					msfoof_set_alif(m, q.x, q.y, hl2char(9, 9));
				}
			}
			lx = n.x;
			ly = n.y;
		}
		char yes = 0;
		u_char lv = 0;
		void shahan(msfoof * m, int x, int y, u_char v) {
			if (x == 0) yes = 0;
			if (v == fg && lv != v) yes = !yes;
//			else if (yes && v != fg) yes = 0;

			msfoof_set_alif(m, x, y, yes ? fg : 0);
			
			lv = v;
		};

//		msfoof_each_alif(m, shahan, NULL);
	}
}
float line(msfoof *m, float x0, float y0, float x1, float y1, int f, ihaatah *xrj) {
	x0 *= 3; y0 *= 3; x1 *= 3; y1 *= 3;
	float w = (nqth.cx *3), sw = w/2.0;

	float	dx		= fabsf(x1 - x0)		,
			dy		= fabsf(y1 - y0)		,
			sx		= (x0 < x1) ? .1 : -.1,
			sy		= (y0 < y1) ? .1 : -.1,
			err		= dx - dy				,
			ang		= 0,
			mx = x1 > x0, // is x1 bigger
			my = y1 > y0; // is y1 bigger

	u_char fg = hl2char(0, 15);
	nuqtahf n = {0};
	ihaatah g = {0};
	ang = coordtoangle(x0, y0, x1, y1);

	nuqtahf a, b, c, d;
	angletocoord(x0, y0, w, ang+90, &a );
	angletocoord(x0, y0, w, ang-90, &b );
	angletocoord(x1, y1, w, ang-90, &c );
	angletocoord(x1, y1, w, ang+90, &d );

	g.x = ( (x0 < x1 ? x0 : x1) - sw );
	g.y = ( (y0 < y1 ? y0 : y1) - sw );
	g.w = (dx+w + 6);
	g.h = (dy+w + 5);

	while (1) {
		for (float i = -sw; i <= sw; i+=1) {
			angletocoord(x0, y0, i, ang-90, &n);
			msfoof_set_alif(m, n.x, n.y, fg);
			angletocoord(x0, y0, i, ang+90, &n);
			msfoof_set_alif(m, n.x, n.y, fg);
		}

		if (x0 == x1 && y0 == y1) break;
		if (mx && x0 >= x1) break;
		else if (!mx && x0 < x1) break;
		if (my && y0 >= y1) break;
		else if (!my && y0 < y1) break;

		float err2 = 2*err;
		if (err2 > -dy) { err -= dy; x0 += sx; }
		if (err2 <  dx) { err += dx; y0 += sy; }
	}

	if (xrj) {
		ihaatah_ws3(xrj, g.x, g.y, g.w, g.h);
	} else {
		matrix_print_alpha_ihaatah(m, &g, f);
		msfoof_clear_area_alif(m, 0, &g);
	}

	return ang;
}
void circle(msfoof *m, float x, float y, float r, float sa, float ea, int c, int f, char taba3) {
	if (sa == ea) return;
	if (sa > ea) sa = sa - 360.0;
	float sw = glbl.s;
	x *= 3; y *= 3; r *= 3;
	float a = 0, d = 0;
	if (c != -1) {
		for (int j = x-r-sw; j <= x+r+sw; j++)
			for (int k = y-r-sw; k <= y+r+sw; k++) {
				d = distancef(j, k, x, y);
				if (d > r && d <= r+sw) {
					a = coordtoangle(j, k, x, y);

					if (a >= sa && a <= ea)
					msfoof_set_alif(m, j, k, 1);
				}
			}
		if (taba3) {
			ihaatah g = {x-r-sw, y-r-sw, x+r+sw, y+r+sw};
			matrix_print_alpha_ihaatah(m, &g, c);
			msfoof_clear_area_alif(m, 0, &g);
		}
	}

	if (f != -1) {
		for (int j = x-r; j <= x+r; j++)
			for (int k = y-r; k <= y+r; k++) {
				d = distancef(j, k, x, y);
				if (d <= r) {
					a = coordtoangle(j, k, x, y);

					if (a >= sa && a <= ea)
					msfoof_set_alif(m, j, k, 1);
				}
			}
		
		if (taba3) {
			ihaatah g = {x-r, y-r, x+r, y+r};
			matrix_print_alpha_ihaatah(m, &g, f);
			msfoof_clear_area_alif(m, 0, &g);
		}
	}
}
void curve_poly(msfoof *m, float x0, float y0, float x1, float y1, float x2, float y2, int color, char taba3) {
	// x2, y2 is curve nuqtah
	// if poly is null, and stroke w is >1, then line is actually drawn
	ihaatah g = {0};
	float parts = sqrtf( distancef(x1, y1, x2, y2) ) * 2;
//	float parts = 3;
	float lastx = x0, lasty = y0;
	for (float t = 0; t < parts; t+=.25){
		float T = t / parts, pow1 = powf(1-T,2), pow2 = powf(T,2);
		float x = pow1 * x0 + 2 * (1-T) * T * x1 + pow2 * x2;
		float y = pow1 * y0 + 2 * (1-T) * T * y1 + pow2 * y2;

		if ( ! ( x == lastx && y == lasty ) ) {
			line(m, lastx, lasty, x, y, 0, &g);
			lastx = x;
			lasty = y;
		}
	}
	if ( ! ( lastx == x2 && lasty == y2 ) ) {
		line(m, lastx, lasty, x2, y2, 0, &g);
	}

	if (taba3) {
		matrix_print_alpha_ihaatah(m, &g, color);
		msfoof_clear_area_alif(m, 0, &g);
	}
}
void drawclock(msfoof *m, float w, float h, int size) {
}
void drawraees() {
	msfoof *mshr = &WJHH.mshr.mtrx;
	msfoof *r = &WJHH.raees.mtrx;
	msfoof_clear_area_f(r, 0, &ktb.g);
	int w = WJHH.raees.mtrx.w, h = WJHH.raees.mtrx.h, bland = 0x333333, c1 = 0xffffff, cw = 10;
	float mw = WJHH.mshr.mtrx.w, mh = WJHH.mshr.mtrx.h, cw3 = cw/3.0;

	unsigned char *tmp = helper_stringify("%.1f %.1f , %.1f %.1f , %.1f %.1f \n %d , w %.1f , p %.1f, q %.1f\0",
		nqth.x, nqth.y, nqth3.x, nqth3.y, nqth2.x, nqth2.y, timess, glbl.s, .25+nqth.cx, qdm
//		nqth.x, nqth.y, nqth3.x, nqth3.y, nqth2.x, nqth2.y, timess, glbl.s, ang0, ang1
	);
	kaatib_baddal(&ktb, tmp);
	free(tmp);
	kaatib_draw(&ktb);

	void ikbar(msfoof * m, int x, int y, unsigned char v) {
		msfoof_rect(r, x*cw3, 100+y*cw3, cw3, cw3, -1, char2rgb(v));
	};

	msfoof_each_alif(mshr, ikbar, NULL);

	r->dotted = 3;
	for (int x = 0; x < w; x+=cw) msfoof_line(r, x, 100  , x, h    , bland);
	for (int y = 0; y < h; y+=cw) msfoof_line(r, 0, 100+y, w, 100+y, bland);
	r->dotted = 0;

	timess++;
	started = 1;
}
void drawmusheer() {
	if (!musheerdrawn) {
//		musheerdrawn = 1;
		msfoof *m = &WJHH.mshr.mtrx;
		msfoof_clear(m, 0);
		msfoof_clear_alif(m, 0);

		int c1 = 0xffffff, c2 = 0x999999;
		float w = WJHH.mshr.mtrx.w, h = WJHH.mshr.mtrx.h;
		
		kaatib_baddal(&ktb2, "tmp");
		kaatib_idhan(&ktb2, *m, 0);

//		if (m->smooth)
//			curve_poly(m, nqth.x, nqth.y, nqth3.x, nqth3.y, nqth2.x, nqth2.y, c1, 0);
//			arc(m, nqth.x, nqth.y, glbl.s, 360*nqth3.x/48.0, 360*nqth3.y/48.0, nqth2.c ? c1:-1, nqth3.c ? c2:-1);
//			ang0 = line(m, nqth.x, nqth.y, nqth3.x, nqth3.y, nqth2.c ? c1:c2, 0);
//			ang1 = line(m, nqth2.x, nqth2.y, nqth3.x, nqth3.y, nqth2.c ? c1:c2, 0);
//			if (ang0 < 0)
//			if (ang1 <= 180) {
//				ang0 = ang1;
//				ang1 = 360;
//			}
//			circle(m, nqth3.x, nqth3.y, nqth.cx/2, 0, 360, nqth2.c ? c1:-1, nqth3.c ? c2:-1, 0);
//		else
//			msfoof_arc(m, nqth.x, nqth.y, glbl.s, 360*nqth3.x/48.0, 360*nqth3.y/48.0, c1, 0);
		
		
//		lwnf l = {0};
//		lwnh l2 = { 0.4, 1.0, 1.0 };
//		void ikbar(msfoof * m, int x, int y, int color) {
//			msfoof_get_owst_alif(m, x, y, &l);
////			printf("%d, %d, %.1f\n", x, y, num);
////			if (l.a) msfoof_set_blend(m, x, y, lwnf2rgb(&l), l.a);
//			if (l.a) {
////				l2.h = haddqanaatf((y+.5)/h);
//				l2.v = haddqanaatf((x+.5)/h);
//				
//				msfoof_set(m, x, y, alwaan_pct(lwnl2rgb(&l2), l.a));
//			}
//		};
//	
//		msfoof_each(m, ikbar, NULL);
//		matrix_print_alpha(m, c1);
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
				if (m.key == KEY_X && nqth.cx < 10) nqth.cx+=delta, yes = 1;
				if (m.key == KEY_Z && nqth.cx >= delta) nqth.cx-=delta, yes = 1;

				if (m.alt) {
					if (m.key == KEY_LEFT && nqth2.x > 0) nqth2.x-=delta, yes = 1;
					if (m.key == KEY_RIGHT && nqth2.x < glbl.w) nqth2.x+=delta, yes = 1;
					
					if (m.key == KEY_UP && nqth2.y > 0) nqth2.y-=delta, yes = 1;
					if (m.key == KEY_DOWN && nqth2.y < glbl.h) nqth2.y+=delta, yes = 1;
				} else if (m.shift) {
					if (m.key == KEY_UP && glbl.s < 24) glbl.s+=delta, yes = 1;
					if (m.key == KEY_DOWN && glbl.s > delta) glbl.s-=delta, yes = 1;
				} else if (m.ctrl) {
					if (m.key == KEY_LEFT && nqth.x > 0) nqth.x-=delta, yes = 1;
					if (m.key == KEY_RIGHT && nqth.x < glbl.w) nqth.x+=delta, yes = 1;
					
					if (m.key == KEY_UP && nqth.y > 0) nqth.y-=delta, yes = 1;
					if (m.key == KEY_DOWN && nqth.y < glbl.h) nqth.y+=delta, yes = 1;
				} else {
					if (m.key == KEY_LEFT && nqth3.x > -24) nqth3.x-=delta, yes = 1;
					if (m.key == KEY_RIGHT && nqth3.x < glbl.w) nqth3.x+=delta, yes = 1;
					
					if (m.key == KEY_UP && nqth3.y > -24) nqth3.y-=delta, yes = 1;
					if (m.key == KEY_DOWN && nqth3.y < glbl.h) nqth3.y+=delta, yes = 1;

					ktb2.g.x = nqth3.x;
					ktb2.g.y = nqth3.y;
				}
				
				if (m.key == KEY_J) ktbscl -= delta, ktb2.g.s = ktbscl, yes = 1;
				if (m.key == KEY_K) ktbscl += delta, ktb2.g.s = ktbscl, yes = 1;
				if (m.key == KEY_U) mstrokw -= delta, WJHH.mshr.mtrx.strokew = mstrokw, yes = 1;
				if (m.key == KEY_I) mstrokw += delta, WJHH.mshr.mtrx.strokew = mstrokw, yes = 1;
				if (m.key == KEY_N) msfh -= delta, ktb2.hrfmsfh = msfh, yes = 1;
				if (m.key == KEY_M) msfh += delta, ktb2.hrfmsfh = msfh, yes = 1;

				if (m.key == KEY_F) nqth2.c = !nqth2.c, yes = 1;
				if (m.key == KEY_B) nqth3.c = !nqth3.c, yes = 1;
				if (m.key == KEY_S) {
					WJHH.mshr.mtrx.smooth = !WJHH.mshr.mtrx.smooth;
//					WJHH.raees.mtrx.smooth = !WJHH.raees.mtrx.smooth;
					yes = 1;
				}

				if (m.key == KEY_Q) exit(0);
			}
			if (yes) {
				kaatib_msfoof(&ktb, &WJHH.raees.mtrx);
				WJHH.mshr.mutadarrar = 1;
				WJHH.raees.mutadarrar = 1;
				mashghool = 1;
			}
		}
		if (w->ism == MUSHEER) {
			if (w->miftaah == MST3D) {
				WJHH.mshr.mtrx.smooth = 1;
				if (w->qadr == 1 && ktb2.mst3d == 0) {
					ktb2.g.w = WJHH.mshr.mtrx.w;
					ktb2.g.h = WJHH.mshr.mtrx.h;
					kaatib_msfoof(&ktb2, &WJHH.mshr.mtrx);
					kaatib_xataa(&ktb2);
					ktb2.mst3d = 1;
				} else {
					ktb2.mst3d = 0;
				}
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
				WJHH.raees.mtrx.smooth = 0;
				WJHH.raees.mutadarrar = 1;
				if (w->qadr == 1 && ktb.mst3d == 0) {
//					ktb.g.y = 480;
					ktb.g.w = WJHH.raees.mtrx.w;
					ktb.g.h = 640-480;
					kaatib_msfoof(&ktb, &WJHH.raees.mtrx);
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
	tarmeez_core();
	kaatib_init(&ktb); // will do matn -> huroof
	ktb.hrfmsfh = msfh;
	ktb.g.s = ktbscl;
	ktb.cursor = -2;
	ktb.showcursor = 0;

	kaatib_init(&ktb2); // will do matn -> huroof
	ktb2.hrfmsfh = msfh;
	ktb2.g.s = ktbscl;
	ktb2.cursor = -2;
	ktb2.showcursor = 0;

	amr_towheed("arc");
	amr_wijhah(1);

	amr_axath(&handler);

    return EXIT_SUCCESS;
}
