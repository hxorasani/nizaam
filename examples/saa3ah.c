/* saa3ah: 
 * TODO:
 * 	give amr a hint that we want musheer and raees before connection
 * 	then amr should handle all the actual logic and errors
 * */

#include "saa3ah.h"

#define XATAA 0

int curcolor = 0, dodo = 0, dirr = 0, started = 0;
struct timespec waqt;
kaatib ktb;

void drawraees() {
	msfoof *mshr = &WJHH.mshr.mtrx;
	msfoof *r = &WJHH.raees.mtrx;
	int w = 4, h = 4;

	msfoof_clear(r, 0);

//	void impress(msfoof * m, int x, int y) {
//		msfoof_rect(m, x*w, 200+(y*h), w, h, -1, msfoof_get(mshr, x, y) );
//	};

	WJHH.raees.mtrx.strokew = 1;

	int color = 0xffffff, c2 = 0x0000ff;

	msfoof_rect(r, 100, 100, 50, 50, color, c2);
	msfoof_circle(r, 50, 125, 25, color, c2);
	msfoof_triangle(r, 175, 100, 175, 150, 200, 150, color, c2);
	
	kaatib_draw(&ktb);

	ihaatah i = {0, 0, 300, 200};
	nuqtahf n = {0, i.h, 0, 2, 2};
	msfoof_scale(r, r, &i, &n);
	
//	msfoof_each(r, &impress, NULL);

	started = 1;

//	unsigned char *p = ihaatah2str(ktb.darar);
//	if (p) {
//		amr_irsal_ilaa_str(MUDEERUID, RAEES, TABA3, ihaatah2str(ktb.darar));
//		free(p);
//		ihaatah_nazaf(&ktb.darar);
//	}
}
void changecolor() {
	msfoof *m = &WJHH.mshr.mtrx;
	
	msfoof_clear(m, 0);
	
	int pad = 8, w = WJHH.mshr.mtrx.w, h = WJHH.mshr.mtrx.h,
		color = curcolor ? 0x999999 : 0xffffff;

	float x0 = 0, y0 = 0, x1 = dodo, y1 = h;
	
	if (dirr) {
		if (dodo > w/4) dirr = 0; else dodo++;
	} else {
		if (dodo < 0) dirr = 1; else dodo--;
	}

	msfoof_triangle(m,
					pad-dodo, dodo+w-pad,
					dodo+w-pad, w-pad-dodo,
					w-pad-dodo, 0+pad-dodo,
					color, 0x666666);

	curcolor = !curcolor;
//	if (started) sleep(4);
//	else
//		started = 1;
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		if (w->ism == MUSHEER) {
//			if (!started)
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				changecolor();
				amr_irsal_ilaa(MUDEERUID, MUSHEER, DARAR, 1);
			}
			if (w->miftaah == MST3D) {
				WJHH.mshr.mtrx.smooth = 1;
			}
		}

		if (w->ism == RAEES) {
			WJHH.raees.mtrx.smooth = 1;

			if (!started)
			if (w->ism == RAEES) {
				if (w->miftaah == INSHA && WJHH.raees.zaahir) {
					if (WJHH.raees.mutadarrar) {
//						printf("qamar raees mutadarrar\n");
						WJHH.raees.mutadarrar = 0;
						drawraees();
						amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
					}
				}
			}
			if (w->miftaah == MST3D) {
				if (w->qadr == 1 && ktb.mst3d == 0) {
					ktb.g.w = WJHH.raees.mtrx.w;
					ktb.g.h = WJHH.raees.mtrx.h;
					ktb.mtrx = WJHH.raees.mtrx;
//					ktb.mtrx.clearfn = &fillerfunc;
					kaatib_xataa(ktb);
					ktb.mst3d = 1;
				} else {
					ktb.mst3d = 0;
				}
			}
		}
	}

	return 0;
}
int main(int argc, char **argv) {
	tarmeez_core();
//	malaf mlf;
//	malaf_init(&mlf, "./data/test.matn");
//	matn_nazaf(&ktb.mtn);
//	malaf_ilaa_matn(&mlf, &ktb.mtn.barzax, &ktb.mtn.length);
//	kaatib_init(&ktb); // will do matn -> huroof
	ktb.hrfmsfh = 4;
	ktb.g.s = 2;
	kaatib_init(&ktb);
	unsigned char *ss = "saa3ah taTbeeq";
	for (int i = 0; i < strlen(ss); ++i) {
		kaatib_adaaf(&ktb, ss[i]);
	}

	amr_towheed("saa3ah");
	amr_wijhah(1);

	amr_axath(&handler);

    return EXIT_SUCCESS;
}
