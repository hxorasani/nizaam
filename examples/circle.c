/* anti: 
 * TODO:
 * 	give amr a hint that we want musheer and raees before connection
 * 	then amr should handle all the actual logic and errors
 * */

#include "circle.h"

#define XATAA 0

int mowjoodah = 0, mashghool = 0, timess = 0;
struct timespec waqt; float cell = 2;
kaatib ktb; ihaatah glbl = {32, 32, 480, 640, 100};
nuqtahf nqth = {240, 320, 1, 3}, nqth2 = {240, 320, 1}, nqth3 = {240, 320, 1},
//nuqtahf nqth = {32, 32, 1, 3}, nqth2 = {34, 45, 1}, nqth3 = {0, 47, 1},
	aas = {2, 2} // anti aliasing samples
;
polygonf poly;

void polygon_adaaf(float x, float y) {
	if (poly.length < 256) {
		poly.nuqaat[poly.length].x = x;
		poly.nuqaat[poly.length].y = y;
		poly.length++;
	}
}
void polygon_ja3al(int i, float x, float y) {
	if (i < poly.length) {
		poly.nuqaat[i].x = x;
		poly.nuqaat[i].y = y;
	}
}
float satar(msfoof *m, float x0, float y0, float x1, float y1, int f) {
	float
//		w = m->strokew,
		w = nqth.cx/2.0,
		sw = w, tr = .875;

	float	dx		= fabsf(x1 - x0)		,
			dy		= fabsf(y1 - y0)		,
			sx		= (x0 < x1) ? .25 : -.25,
			sy		= (y0 < y1) ? .25 : -.25,
			err		= dx - dy				,
			ang		= 0,
			mx = x1 > x0, // is x1 bigger
			my = y1 > y0; // is y1 bigger

	nuqtahf n = {0};
	ang = coordtoangle(x0, y0, x1, y1);

	while (1) {
		for (float i = -sw; i <= sw; i+=.25) {
			angletocoord(x0, y0, i, ang-90, &n);
			if (m->smooth) msfoof_set_blend(m, n.x, n.y, f, tr);
			else		   msfoof_set(m, n.x, n.y, f);
			angletocoord(x0, y0, i, ang+90, &n);
			if (m->smooth) msfoof_set_blend(m, n.x, n.y, f, tr);
			else		   msfoof_set(m, n.x, n.y, f);
		}

//		if (x0 == x1 && y0 == y1) break;
		if (mx && x0 > x1) break;
		else if (!mx && x0 < x1) break;
		if (my && y0 > y1) break;
		else if (!my && y0 < y1) break;

		float err2 = 2*err;
		if (err2 > -dy) { err -= dy; x0 += sx; }
		if (err2 <  dx) { err += dx; y0 += sy; }
	}

	return ang;
}
void mudalla3(msfoof *m, polygonf *p, int o, int f, char taba3) {
	int length = p->length;
	if (length) {
		nuqtahf *nqt = p->nuqaat;
		if (f != -1) {
			float minX = nqt[0].x, maxX = nqt[0].x,
				  minY = nqt[0].y, maxY = nqt[0].y,
				  incx = m->smooth ? 1.0/aas.x : 1,
				  incy = m->smooth ? 1.0/aas.y : 1,
				  tr = (1.0/aas.x);

			for (int n = 1; n < length; n++) {
				minX = fmin(nqt[n].x, minX);
				maxX = fmax(nqt[n].x, maxX);
				minY = fmin(nqt[n].y, minY);
				maxY = fmax(nqt[n].y, maxY);
			}
			
			int c = 0;
			for (float y = minY; y < maxY; y+=incy) { // from minY to maxY
				for (float x = minX; x < maxX; x+=incx) { // each vert line
					// continue if x, y are inside the bounding box
					if (x < minX || x > maxX || y < minY || y > maxY) return;

					c = 0;
					int j = length - 1;
					for (int i = 0; i < length; ++i) {
						if ( (x == nqt[i].x) && (y == nqt[i].y) ) {
							// point is a corner
							c = 1;
						} else
						if ( (nqt[i].y > y) != (nqt[j].y > y) ) {
							int slope = (x-nqt[i].x)*(nqt[j].y-nqt[i].y)-(nqt[j].x-nqt[i].x)*(y-nqt[i].y);
							if (slope == 0) {
								// point is on boundary
								c = 1;
							} else
							if ( (slope < 0) != (nqt[j].y < nqt[i].y) ) {
								c = !c;
							}
						}

						j = i;
					}
					if (c) {
						if (m->smooth) msfoof_set_blend(m, x, y, f, tr);
						else 		   msfoof_set(m, x, y, f);
					}
				}
			}
		}
		if (o != -1) {
			for (int n = 1; n < length; n++) {
				satar(m, nqt[n-1].x, nqt[n-1].y, nqt[n].x, nqt[n].y, o);
			}
			satar(m, nqt[0].x, nqt[0].y, nqt[length-1].x, nqt[length-1].y, o);
		}
	}
}
void circle(msfoof *m, float x, float y, float r, float sa, float ea, int o, int f) {
	float
		sw = nqth.cx,
		incx = m->smooth ? 1.0/m->nmvj.x : 1,
		incy = m->smooth ? 1.0/m->nmvj.y : 1,
		tr = 1.0/(m->nmvj.x*2),
		ea2 = 0, sw2 = sw*3;
	if (sa > 0) {
		if (sa > 360.0) sa = sa-360.0;
		ea = sa+ea;
		if (ea > 360.0) ea2 = fabs(360.0-ea);
	}
	if (sa == ea) return;
	if (f == -1 && o == -1) return;

	float a = 0, d = 0, r2 = r*2,
		xmin = x-r, ymin = y-r, rmin = 0,
		xmax = x+r, ymax = y+r, rmax = r;
	int c, yes = 1;
	if (o != -1 && f == -1) {
		rmin = r;
	}
	if (o != -1) {
		xmin = x-r-sw, ymin = y-r-sw,
		xmax = x+r+sw, ymax = y+r+sw, rmax = r+sw;
	}
	ihaatahf ih = {xmin, ymin, xmax, ymax};
	ihaatah ih2 = m->maqtoo3;
	if (ihaatah_yajid(&ih2)) {
		if (ih.x < ih2.x) ih.x = ih2.x;
		if (ih.y < ih2.y) ih.y = ih2.y;
		if (ih.w > ih2.w) ih.w = ih2.w+ih2.x;
		if (ih.h > ih2.h) ih.h = ih2.h+ih2.y;
		if (ih.x > ih2.w || ih.y > ih2.h) yes = 1;
	}
	if (yes)
	for (float j = ih.x; j < ih.w; j+=incx)
		for (float k = ih.y; k < ih.h; k+=incy) {
			d = distancef(j, k, x, y);
			if (d >= rmin && d <= rmax) {
				if (d > r) c = o;
				else if (d <= r) c = f;
				a = coordtoangle(j, k, x, y);

				if ((a >= sa && a <= ea) || (ea2 && a >= 0 && a <= ea2))
					if (m->smooth/* && d > r-1*/)
						msfoof_set_blend(m, j, k, c, tr);
					else
						msfoof_set(m, j, k, c);
			}
		}
}
void drawraees() {
	timess = axav_waqt();

	msfoof *mshr = &WJHH.mshr.mtrx;
	msfoof *r = &WJHH.raees.mtrx;
	msfoof_clear(r, 0);
	int w = r->w, h = r->h, bland = 0x222222,
	c1 = 0x999999, c2 = 0xffffff, c3 = 0xff0000,
	cw = 7, yes = 0;
	float mw = mshr->w, mh = mshr->h;

	r->strokew = nqth.cx;
	matrix_circle(r, nqth.x, nqth.y, glbl.s, 0, nqth2.y, nqth2.c ? c1:-1, nqth3.c ? c2:-1);
	r->strokew = 1;

	timess = axav_waqt()-timess;

	u_char *tmp = helper_stringify("%.1f %.1f , %.1f %.1f , %.1f %.1f \n %d , w %.1f , p %.1f\0",
		nqth.x, nqth.y, nqth3.x, nqth3.y, nqth2.x, nqth2.y, timess, glbl.s, .25+nqth.cx
	);
	kaatib_baddal(&ktb, tmp);
	free(tmp);
//	kaatib_idhan_ikraah(&ktb, *r);

//	void ikbar(msfoof * m, int x, int y) {
//		msfoof_rect(r, x*cw, 100+y*cw, cw, cw, -1, msfoof_get(mshr, x, y));
//	};
//	msfoof_each(mshr, &ikbar, NULL);
//	msfoof_rect(r, 0, 100, 64*cw, 64*cw, bland, -1);
}
void drawmusheer() {
//	msfoof *m = &WJHH.mshr.mtrx;
//	msfoof_clear(m, 0);

//	int c1 = 0xffffff, c2 = 0xff0000, c3 = 0xffffff;
//	float w = m->w, h = m->h;

}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		if (w->ism == MUDEER && w->miftaah == LOWHAH) {
			char yes = 0;
			float delta = 15;
//			float delta = .5;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			if (m.state) {
				if (m.key == KEY_U && mowjoodah > 0) mowjoodah--, yes = 1;
				if (m.key == KEY_O && mowjoodah < poly.length-1) mowjoodah++, yes = 1;

				if (m.key == KEY_J) poly.nuqaat[mowjoodah].x-=delta, yes = 1;
				if (m.key == KEY_L) poly.nuqaat[mowjoodah].x+=delta, yes = 1;
				if (m.key == KEY_I) poly.nuqaat[mowjoodah].y-=delta, yes = 1;
				if (m.key == KEY_K) poly.nuqaat[mowjoodah].y+=delta, yes = 1;

				if (m.key == KEY_X && nqth.cx < 300) nqth.cx+=.25, yes = 1;
				if (m.key == KEY_Z && nqth.cx >= .25) nqth.cx-=.25, yes = 1;

				if (m.alt) {
					if (m.key == KEY_LEFT && nqth2.x > 0) nqth2.x-=delta, yes = 1;
					if (m.key == KEY_RIGHT && nqth2.x < glbl.w) nqth2.x+=delta, yes = 1;
					
					if (m.key == KEY_UP && nqth2.y > 0) nqth2.y-=delta, yes = 1;
					if (m.key == KEY_DOWN && nqth2.y < glbl.h) nqth2.y+=delta, yes = 1;

				} else if (m.shift) {
					if (m.key == KEY_UP && glbl.s < 300) glbl.s+=delta, yes = 1;
					if (m.key == KEY_DOWN && glbl.s > delta) glbl.s-=delta, yes = 1;
				} else if (m.ctrl) {
					if (m.key == KEY_LEFT && nqth.x > 0) nqth.x-=delta, yes = 1;
					if (m.key == KEY_RIGHT && nqth.x < glbl.w) nqth.x+=delta, yes = 1;
					
					if (m.key == KEY_UP && nqth.y > 0) nqth.y-=delta, yes = 1;
					if (m.key == KEY_DOWN && nqth.y < glbl.h) nqth.y+=delta, yes = 1;

				} else {
					if (m.key == KEY_LEFT && nqth3.x > 0) nqth3.x-=delta, yes = 1;
					if (m.key == KEY_RIGHT && nqth3.x < glbl.w) nqth3.x+=delta, yes = 1;
					
					if (m.key == KEY_UP && nqth3.y > 0) nqth3.y-=delta, yes = 1;
					if (m.key == KEY_DOWN && nqth3.y < glbl.h) nqth3.y+=delta, yes = 1;
				}
				
				if (m.key == KEY_F) nqth2.c = !nqth2.c, yes = 1;
				if (m.key == KEY_B) nqth3.c = !nqth3.c, yes = 1;
//				if (m.key == KEY_S) WJHH.mshr.mtrx.smooth = !WJHH.mshr.mtrx.smooth, yes = 1;
				if (m.key == KEY_S) WJHH.raees.mtrx.smooth = !WJHH.raees.mtrx.smooth, yes = 1;

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
//				nqth.x = WJHH.mshr.mtrx.w;
//				nqth.y = WJHH.mshr.mtrx.h;
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
				if (w->qadr == 1) {
//					ktb.g.y = 480;
					ktb.g.w = WJHH.raees.mtrx.w;
					ktb.g.h = WJHH.raees.mtrx.h;
					ktb.mtrx = WJHH.raees.mtrx;
//					ktb.mtrx.clearfn = &fillerfunc;
					kaatib_xataa(&ktb);
					ktb.mst3d = 1;
				} else {
					ktb.mst3d = 0;
				}
			}
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
				if (WJHH.raees.mutadarrar) {
					char darar = 0;
					drawraees();
					amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);

					if (kaatib_mutadarrar(&ktb)) darar = 2;

					// if animating, return 2 until anim is complete
					if (darar != 2) {
						WJHH.raees.mutadarrar = 0;
					}
				}
			}
		}
	}

	return 0;
}
int main(int argc, char **argv) {
	
	poly.length = 0;
	poly.nuqaat = (nuqtahf *) calloc(256, sizeof(nuqtahf));
	polygon_adaaf(nqth .x, nqth .y);
	polygon_adaaf(nqth2.x, nqth2.y);
	polygon_adaaf(nqth3.x, nqth3.y);
	
	tarmeez_core();
	kaatib_init(&ktb); // will do matn -> huroof
	ktb.hrfmsfh = 3;
	ktb.g.s = 1.5;
	ktb.cursor = -2;
	ktb.taht = 0;

	amr_towheed("circle");
	amr_wijhah(1);

	amr_axath(&handler);

    return EXIT_SUCCESS;
}
