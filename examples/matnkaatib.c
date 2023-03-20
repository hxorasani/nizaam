/* clock: 
 * 
 * */

#include "matnkaatib.h"

#define XATAA 0
int opened = 0;

void hfizmalaf() {
	matn_nazaf(&ktb.mtn);
	kaatib_ilaa_matn(&ktb);
	malaf mlf;
	mlf.length = ktb.mtn.length;
	mlf.barzax = (u_char *) ktb.mtn.barzax;
//	printf("%s\n", (u_char *) ktb.mtn.barzax);
	malaf_hfiz(&mlf, "data/test2.matn");
}
void iftahmalaf() {
	malaf mlf;
	malaf_init(&mlf, "data/test2.matn");
	matn_nazaf(&ktb.mtn);
	malaf_ilaa_matn(&mlf, &ktb.mtn.barzax, &ktb.mtn.length);
	kaatib_init(&ktb); // will do matn -> huroof
	// can't destroy this because it is needed by hfizmalaf
//	matn_destroy(&ktb.mtn);
	malaf_destroy(&mlf);
	opened = 1;
}
char on_paint_window(msfoof *m) {
//	msfoof_clear(m, 0);

	kaatib_idhan(&ktb, *m, 0);
	msfoof_rect(m, ktb.g.x, ktb.g.y, ktb.g.w, ktb.g.h, 0xff0000, -1);

//	u_char *p = ihaatah2str(ktb.darar);
//	if (p) {
//		amr_irsal_ilaa_str(MUDEERUID, RAEES, TABA3, p);
//		free(p);
//		ihaatah_nazaf(&ktb.darar);
//	}

	return 1;
}
char on_paint_indicator(msfoof *m) {
	msfoof_clear(m, 0);
	matrix_circle(m, m->w/2, m->h/2, m->w*0.45, 0, 360*((ktb.n.y+ktb.g.h)/ktb.muntahaa.h), -1, 0xff0000);
}
void on_bound_indicator(ihaatah i) {
}
void on_bound_window(ihaatah i) {
	ktb.showcursor = 1;
	ktb.g.x = 0;
	ktb.g.y = 0;
	ktb.g.w = i.w;
	ktb.g.h = i.h;
	kaatib_qadd( &ktb, 1.5 );
//	ktb.n.c = 0;
//	kaatib_xataa(&ktb);
	ktb.mu3aaq = 0;
	if (!opened) iftahmalaf();
}
char b_lowhah(mafateeh m) {
	char yes = 0;
	if (m.ctrl && m.key == KEY_O && m.state) {
		iftahmalaf();
		yes = 1;
	} else
	if (m.ctrl && m.key == KEY_S && m.state) {
		hfizmalaf();
		yes = 1;
	} else
	if (m.ctrl && m.key == KEY_J && m.state) {
		kaatib_muashshar( &ktb );
		yes = 1;
	}
	return yes;	
}
int main(int argc, char **argv) {
	composer t = {
		.ism = "matnkaatib",
		.b_lowhah = b_lowhah,
		.on_paint_window = on_paint_window,
		.on_paint_indicator = on_paint_indicator,
		.on_bound_window = on_bound_window,
		.on_bound_indicator = on_bound_indicator,
//		.xitaab = "apps/matnkaatib.js",
	};

	composer_init(t);

    return EXIT_SUCCESS;
}
