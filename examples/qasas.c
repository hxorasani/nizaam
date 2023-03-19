/* qasas: 
 * TODO:
 * 	give amr a hint that we want musheer and raees before connection
 * 	then amr should handle all the actual logic and errors
 * */

#include "qasas.h"

#define XATAA 0

int musheerinit = 0, raeesinit = 0, elapsed = 0, playing = 0, bufSize = 64, period = 10, error;
int curcolor = 0;
int x = 0, y = 0, menuvisible = 0, direction = 0;
int px = 0, py = 0; int px2 = 0, py2 = 0, factor = 4, freq = 4;
float volume = 100, center = 0;
struct timespec waqt;
kaatib ktb;

void drawraees() {
	msfoof *r = &WJHH.raees.mtrx;
	int w = WJHH.raees.mtrx.w, h = WJHH.raees.mtrx.h,
		c1 = curcolor ? 0xffffff : 0xff00ff, c2 = 0x111111,
		hh = h/2,
		wh = w/2,
		hq = h/4,
		wq = w/4,
		cell = w/bufSize,
		pad = 10, rows = 3;

	ihaatah ihth = {0, 0, 90, 120};
	msfoof_clear_area(r, 0x111111, &ihth);

	if (menuvisible) {
		msfoof_rect(r, wh, pad, wh-(pad*2), h-(pad*2), c1, c2);
	}

	double sinresult = sin(center/factor);
	int curx = sinresult * wq;
	int curtanx = atan(sinresult) * freq;
	
	msfoof_set(r, wh, hh+center, c1);
	
	if (elapsed) {
		msfoof_circle(r, wh+curx, hh+center, 2, -1, c1);
		msfoof_line(r, wh+curx, hh+center, px, py, c1);

		msfoof_line(r, px+curtanx, py+center, px+px2, py+py2, c1);
	}

	px = wh+curx;
	py = hh+center;

	px2 = wh+curtanx;
	py2 = hh+center;
	
	if (direction) center += 1; else center -= 1;
	if (center < -hq) direction = 1, curcolor = !curcolor;
	if (center > hq) direction = 0, curcolor = !curcolor;
	
	kaatib_tasteer(&ktb,
		helper_stringify(
			"%d\n%.0f\n%d\n%d\0",
			elapsed, center, curx, factor
		)
	);
	kaatib_draw(&ktb);
	++elapsed;
}
void clearall() {
	msfoof *r = &WJHH.raees.mtrx;
	msfoof_clear(r, 0);
}
void togglemenu() {
	menuvisible = !menuvisible;
}
void drawmusheer() {
	msfoof *m = &WJHH.mshr.mtrx;
	msfoof_clear(m, 0);
	int w = WJHH.mshr.mtrx.w, h = WJHH.mshr.mtrx.h,
		c1 = 0x3399ff, c2 = curcolor ? 0x999900 : 0xffff00;

	msfoof_circle(m, w/2, h/2, w/3, c1, c2);
	curcolor = !curcolor;
	musheerinit = 1;
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		if (w->ism == MUDEER && w->miftaah == LOWHAH) {
			mafateeh m = { 0 }; char yes = 0;
			str2mafateeh(w->qadrstr, &m);
			if (m.state && !playing) {
				if (m.key == KEY_1) freq = 100, yes = 1;
				if (m.key == KEY_2) freq = 200, yes = 1;
				if (m.key == KEY_3) freq = 300, yes = 1;
				if (m.key == KEY_4) freq = 400, yes = 1;
				if (m.key == KEY_5) freq = 500, yes = 1;
				if (m.key == KEY_6) freq = 600, yes = 1;
				if (m.key == KEY_7) freq = 700, yes = 1;
				if (m.key == KEY_8) freq = 800, yes = 1;
				if (m.key == KEY_9) freq = 900, yes = 1;

				if (m.key == KEY_UP) factor += 4;
				if (m.key == KEY_DOWN) factor -= 4;
				if (factor <= 0) factor = 4;
				if (factor > 64) factor = 64;
				if (m.key == KEY_RIGHT) freq += 4;
				if (m.key == KEY_LEFT) freq -= 4;
				if (freq <= 0) freq = 4;
				if (freq > 64) freq = 64;

//				if (m.key == KEY_UP) y -= 10, yes = 2;
//				if (m.key == KEY_DOWN) y += 10, yes = 2;
//				if (m.key == KEY_RIGHT) x += 10, yes = 2;
//				if (m.key == KEY_LEFT) x -= 10, yes = 2;
//				if (m.key == KEY_COMPOSE) togglemenu(), yes = 1;
				if (m.key == KEY_COMPOSE) clearall();
				if (yes) {
					if (yes == 2) {
						if (x < 0) x = 0;
						if (y < 0) y = 0;
//						unsigned char *tmp = helper_stringify("%d, %d\0", x, y);
//						kaatib_baddal(&ktb, tmp);
//						free(tmp);
						ihaatah ihth = { x, y };
						unsigned char *p = ihaatah2str(ihth);
						if (p) {
							amr_irsal_str(ISHAARAH, MAXRAJ, p);
							free(p);
						}
					}
					drawraees();
					amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
				}
			}
		}
		if (w->ism == MUSHEER) {
			if (!musheerinit)
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				drawmusheer();
				amr_irsal_ilaa(MUDEERUID, MUSHEER, DARAR, 1);
			}
		}
		if (w->ism == RAEES) {
			if (w->miftaah == MST3D) {
				if (w->qadr == 1 && ktb.mst3d == 0) {
					ktb.g.w = WJHH.raees.mtrx.w;
					ktb.g.h = ktb.x.h*ktb.g.s;
					kaatib_msfoof(&ktb, WJHH.raees.mtrx);
					kaatib_xataa(ktb);
					ktb.mst3d = 1;
				} else {
					ktb.mst3d = 0;
				}
			}
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
//				if (WJHH.raees.mutadarrar) {
//					WJHH.raees.mutadarrar = 0;
					drawraees();
					amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
//				}
			}
		}
	}

	return 0;
}
int main(int argc, char **argv) {
	tarmeez_core();

	kaatib_init(&ktb); // will do matn -> huroof
	ktb.hrfmsfh = 2;
	ktb.g.s = 1;
	
	amr_towheed("qasas");
	amr_wijhah(1);

	amr_axath(&handler);

    return EXIT_SUCCESS;
}
