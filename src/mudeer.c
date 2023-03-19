/* mudeer: wijhaat/ruasaa for umoor & renders them to maxraj
 * also handles madxal waaqi3aat
 * 
 * 
 * 
 * TODO:
 * * mudeer should have a taxeer function, umoor should be able to schedule events
 *   add shims for setTimeout, setInterval, clearTimeout, clearInterval
 * * darar regions, collect ruasaa, musheeroon, each darar that is reported
 *   adds to the mutadarrar regions
 * */

#include "mudeer.h"
#define XATAA 0
// how many wijhaat can run in the bg simultaneously
#define MAX_WIJHAAT 32
#define MAX_TASHGHEEL 10

int size = 0, width = 0, height = 0, allowrendering = 0,
	mspf = 20*1000, // ms per frame
	kulwijhaat = 0, mutadarrar = 0, markooz = 0, zaahir = 0,
	markoozraees = -1, ayyiframe = 0,
	musheerdim = 48, // indicators
	raeesw = 0, raeesh = 0,
	waspointerin = 0;

// input convergence
char shift = 0, ctrl = 0, alt = 0, meta = 0;

wijhah wijhaat[MAX_WIJHAAT] = { 0 };
int tashgheel[MAX_TASHGHEEL] = { 0 };

long frames = 0; float fps = 0; void *xitaab;
msfoof overlay, overlay2; kaatib ktb; struct timespec waqt;
msfoof *mowjoodah;

palette theme = {
	.accent=		0xffff9900,
	.accentt=		0x99ff9900,
	.accentdim=		0xff996600,
	.accentdimt=	0x99660099,
	.primary=		0xff000000,
	.primaryt=		0x99000000,
	.secondary=		0xff666666,
	.secondaryt=	0x99666666,
	.gray=			0xff454545,
	.grayt=			0x99454545,
	.tertiary=		0xff333333,
	.tertiaryt=		0x99333333,
	.textbright=	0xffffffff,
	.textbrightt=	0x99ffffff,
	.text=			0xffdddddd,
	.textt=			0x99dddddd,
	.textdim=		0xff999999,
	.textdimt=		0x99999999,
};

long beat() {
	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	long farq = sec2ms(aan.tv_sec-waqt.tv_sec) + nano2ms(aan.tv_nsec-waqt.tv_nsec);
	return farq;
}
void starttimer() {
	frames = 0;
	if ( clock_gettime(CLOCK_REALTIME, &waqt) )
		printf("clock_gettime failed! %s\n", strerror(errno));
}
void drawmusheeroon() {
	for (int i = 0; i < kulwijhaat; ++i) {
		msfoof_rect(mowjoodah, i*musheerdim, height-musheerdim,
					musheerdim, musheerdim,
					theme.tertiary, theme.primary);
	}
}
void collectmusheeroon() {
	for (int i = 0; i < kulwijhaat; ++i) {
		if (wijhaat[i].rabt > 0) {
			/* TESTING NEW
			 * as long as an amr is mutadarrar, don't send an INSHA event
			 * composite it first and mark it as non-mutadarrar first
			 * then the next INSHA event can go through
			 * */
			if (wijhaat[i].mshr.zaahir && !wijhaat[i].mshr.mutadarrar)
				amr_irsal_ilaa(wijhaat[i].rabt, MUSHEER, INSHA, 1);
			if (wijhaat[i].raees.zaahir && !wijhaat[i].raees.mutadarrar)
				amr_irsal_ilaa(wijhaat[i].rabt, RAEES, INSHA, 1);
		}
	}
}
void applymusheeroon() {
	for (int i = 0; i < kulwijhaat; ++i) {
		if (wijhaat[i].rabt > 0) {
			if (wijhaat[i].mshr.xitaab && wijhaat[i].mshr.mutadarrar && wijhaat[i].mshr.zaahir) {
				ihaatah ihth2 = { i*musheerdim, height-musheerdim, musheerdim, musheerdim };
				XATAA && printf("mtrx mshr %s %d %p\n", wijhaat[i].ism, i, wijhaat[i].mshr.xitaab);
				msfoof_composite(&wijhaat[i].mshr.mtrx, mowjoodah, &ihth2);
				wijhaat[i].mshr.mutadarrar = 0;
			}
			if (wijhaat[i].raees.xitaab && wijhaat[i].raees.mutadarrar && wijhaat[i].raees.zaahir) {
				XATAA && printf("mtrx raees %s %d %p\n", wijhaat[i].ism, i, wijhaat[i].raees.xitaab);
				ihaatah *ihth = &wijhaat[i].raees.ihth;
				if (ihth->w > 0) {
					ihth->w = ihth->w-ihth->x;
					ihth->h = ihth->h-ihth->y;
//					printf("mudeer mutadarrar: %d %d %d %d\n",
//							ihth->x, ihth->y, ihth->w, ihth->h);
					ihaatah ihth2 = { ihth->x, 1+ihth->y, ihth->w, ihth->h };
					msfoof_composite_clipped(&wijhaat[i].raees.mtrx, mowjoodah, ihth, &ihth2);
					overlay.dotted = 7;
					msfoof_rect_g(mowjoodah, &ihth2, 0xff0000, -1);
					overlay.dotted = 0;
					ihaatah_nazaf(ihth);
				} else {
					ihaatah ihth2 = { 0, 1, raeesw, raeesh-2 };
					msfoof_composite(&wijhaat[i].raees.mtrx, mowjoodah, &ihth2);
				}
//				taba3_waqt("done compiz'd raees -> overlay");
				wijhaat[i].raees.mutadarrar = 0;
			}
		}
	}
}
void vikrmusheeroon() {
	int fd,
		hajm = musheerdim*musheerdim*sizeof(int);
	for (int i = 0; i < kulwijhaat; ++i) {
		wijhaat[i].shaxsiyyah = i;

		if (wijhaat[i].mshr.xitaab != NULL) {
			amr_dhikr_ixttm(wijhaat[i].mshr.xitaab, hajm); // close from current process
			wijhaat[i].mshr.mtrx.barzax = NULL;
			wijhaat[i].mshr.xitaab = NULL;
		}

		char *p = helper_stringify("musheer%d", i);
		if (p) {
			amr_dhikr((u_char *) p, hajm, &fd);
			
			free(p);
			
			amr_dhikr_axath(fd, hajm, &wijhaat[i].mshr.xitaab);
			wijhaat[i].mshr.mtrx = msfoof_new(musheerdim, musheerdim, NULL);
			msfoof_set_barzax(&wijhaat[i].mshr.mtrx, (int *) wijhaat[i].mshr.xitaab);
		}
	}
}
void vikrruasaa() {
	int fd,
		raeesh2 = raeesh-2,
		hajm = raeesw*raeesh2*sizeof(int);

	for (int i = 0; i < kulwijhaat; ++i) {
		if (wijhaat[i].raees.xitaab != NULL) {
			amr_dhikr_ixttm(wijhaat[i].raees.xitaab, hajm); // close from current process
			wijhaat[i].raees.mtrx.barzax = NULL;
			wijhaat[i].raees.xitaab = NULL;
		}

		char *p = helper_stringify("raees%d", i);
		if (p) {
			
			amr_dhikr((u_char *) p, hajm, &fd);
			
			free(p);
			
			amr_dhikr_axath(fd, hajm, &wijhaat[i].raees.xitaab);
			wijhaat[i].raees.mtrx = msfoof_new(raeesw, raeesh2, NULL);
			msfoof_set_barzax(&wijhaat[i].raees.mtrx, (int *) wijhaat[i].raees.xitaab);
		}
//		wijhaat[i].raees.xitaab = NULL;
	}
}
int setupmemory() {
	int fd;

	size = width*height*sizeof(int);

	if (xitaab != NULL) {
		amr_dhikr_ixttm(xitaab, size); // close from current process
		if (overlay.barzax != NULL) {
			// free old overlay barzax, alif is never assigned
//			printf("overlay.barzax %p\n", overlay.barzax);
			msfoof_set_barzax(&overlay, NULL);
		}
		if (overlay2.barzax != NULL) {
			msfoof_set_barzax(&overlay2, NULL);
		}
		xitaab = NULL;
	}

	amr_dhikr("maxraj", size, &fd);
	amr_dhikr_axath(fd, size, &xitaab);

	overlay = msfoof_new(width, height, NULL);
//	overlay2 = msfoof_new(width, height, NULL);
	
	mowjoodah = &overlay;
	
	kulwijhaat = width / musheerdim;

//	kaatib_init(&ktb);
//	ktb.g.s = 1.25;
//	ktb.g.w = 400;
//	ktb.g.h = 200;
//	ktb.p = theme;
//	ktb.mtrx = overlay;
//	kaatib_xataa(ktb);
	
	/* EXPLAINED
	 * when maxraj reconnects after crash, this gets called again, so
	 * so we close all old xutabaa from this amr and reopen them here
	 * barzaxoon don't need free'ing because they're all shared mem
	 * and alif is not assigned at all anywhere
	 * */
	vikrmusheeroon();
	vikrruasaa();

	drawmusheeroon();
	
	amr_irsal(MUDEER, MST3D, 1); // ready, hint maxraj to start heartbeat
}
void tb3matn(int x, int y, unsigned char *p) {
	if (p) {
		ktb.mtn.barzax = p;
		ktb.mtn.length = strlen(p);
		ktb.g.x = x;
		ktb.g.y = raeesh-(ktb.x.h * ktb.g.s)-y;
		kaatib_init(&ktb);
		kaatib_draw(&ktb);
	}
}
void safaa() {
	msfoof_rect(mowjoodah, 0, 0, width, height/2, theme.primary, 0);
}
void safaaraees() {
	if (allowrendering) {
		msfoof_rect(mowjoodah, 0, 1, width, raeesh-2, 0, theme.primary);
	}
}
int axathwijhahxaali() {
	for (int i = 0; i < kulwijhaat; ++i) {
		if (wijhaat[i].rabt <= 0) {
			return i;
		}
	}
	return -1;
}
void setmusheer(int i, int amr) {
	XATAA && printf("[%d] (%d) +musheer\n", i, amr);
	wijhaat[i].rabt = amr;
	wijhaat[i].mshr.zaahir = 1;
}
void drawmusheerline(int i, int active) {
	int pad = musheerdim/2.5;
	int x = i*musheerdim +pad;
	int y = height-musheerdim-1;
	msfoof_line(mowjoodah, x, y,
				x+musheerdim -pad*2, y,
				active ? theme.accent : theme.primary);
}
void iqtalraees(int i) {
	if (i > -1 && i < kulwijhaat) {
		if (wijhaat[i].rabt) {
			amr_irsal_ilaa(wijhaat[i].rabt, MUDEER, QATAL, 1);
		}
	}
}
void ixtafraees(int i) {
	if (i > -1 && i < kulwijhaat) {
		drawmusheerline(i, 0);

		if (markoozraees == i) markoozraees = -1;

		if (wijhaat[i].rabt && wijhaat[i].raees.zaahir) {
			amr_irsal_ilaa(wijhaat[i].rabt, RAEES, IXTAF, 0);
			wijhaat[i].raees.zaahir = 0;
			safaaraees();
		}
	}
}
void ixtafruasaa() {
	XATAA && printf("ixtafruasaa\n");
	markoozraees = -1;
	for (int i = 0; i < kulwijhaat; ++i) {
		ixtafraees(i);
	}
}
void izharraees(int i) {
	XATAA && printf("izharraees %d\n", i);
	if (i < kulwijhaat) {
		if (wijhaat[i].rabt) {
			wijhaat[i].raees.zaahir = 1;
			wijhaat[i].mshr.mutadarrar = 1; // TODO BUG 
			wijhaat[i].raees.mutadarrar = 1;
			amr_irsal_ilaa(wijhaat[i].rabt, RAEES, IZHAR, 1);
		}
		drawmusheerline(i, 1);
	}
}
void taba3raees(int i, waaqi3ah *w) { // expands ihaatah
	if (i < kulwijhaat && wijhaat[i].rabt) {
		if (XATAA) amr_tb3_waaqi3ah(w);

		ihaatah ihth = { 0 };
		str2ihaatah(w->qadrstr, &ihth);
		ihaatah *ihth2 = &wijhaat[i].raees.ihth;
		
		/*unsigned char *p = helper_stringify("x(%d) y(%d) w(%d) (h%d)",
							ihth.x, ihth.y, ihth.w-ihth.x, ihth.h-ihth.y);
		if (p) {
			tb3matn(0, 40, p);
			printf("%s\n", p);
			free(p);
		}*/
		
		// validate dimensions, enforce limits
		if (ihth.x < 0) ihth.x = 0;
		if (ihth.y < 0) ihth.y = 0;
		if (ihth.w > raeesw) ihth.w = raeesw;
		if (ihth.h > raeesh) ihth.h = raeesh;

		// to avoid huge loops if garbage ints are received
		if (ihth.x >= 0 && ihth.y >= 0 && ihth.w <= raeesw && ihth.h <= raeesh) {
			if (ihth2->w == 0) ihth2->x = ihth.x;
			else if (ihth.x < ihth2->x) ihth2->x = ihth.x;

			if (ihth2->h == 0) ihth2->y = ihth.y;
			else if (ihth.y < ihth2->y) ihth2->y = ihth.y;

			if (ihth.w > ihth2->w) ihth2->w = ihth.w;
			if (ihth.h > ihth2->h) ihth2->h = ihth.h;
		}
	}
}
void rakkazraees(int i) {
	if (i >= 0 && i < kulwijhaat) {
		ixtafruasaa(); // why was it outside this if clause before?
		izharraees(i);
		markoozraees = i;
		mutadarrar = 1;
	}
}
void dararraees(int i) {
	if (i < kulwijhaat && wijhaat[i].rabt) {
		wijhaat[i].raees.mutadarrar = 1;
		mutadarrar = 1;
	}
}
void setraees(int i, int amr) {
	XATAA && printf("[%d] (%d) +raees\n", i, amr);
//	if (wijhaat[i].raees.xitaab == NULL) {
		amr_irsal_ilaa(amr, RAEES, MST3D, 0);
		amr_irsal_ilaa(amr, RAEES, NAFS, i);
		amr_irsal_ilaa(amr, RAEES, HAJMW, raeesw);
		amr_irsal_ilaa(amr, RAEES, HAJMH, raeesh-2);
		amr_irsal_ilaa(amr, RAEES, MST3D, 1);
		
		rakkazraees(i);
//	} else {
//		XATAA && printf("(%d) already has a raees\n", amr);
//	}
}
void setamrism(int amr, u_char ism[256]) { // is amr in wijhaat
	for (int i = 0; i < kulwijhaat; ++i) {
		if (wijhaat[i].rabt == amr) {
			strcpy(wijhaat[i].ism, ism);
			return;
		}
	}
}
int filwijhaat(int amr) { // is amr in wijhaat
	for (int i = 0; i < kulwijhaat; ++i) {
		if (wijhaat[i].rabt == amr) {
			return i;
		}
	}
	return -1;
}
void rakkazraeessaabiqah() {
	XATAA && printf("rakkazraeessaabiqah\n");
	for (int i = 0; i < kulwijhaat; ++i) {
		if (wijhaat[i].rabt && wijhaat[i].raees.zaahir == 0) {
			rakkazraees(i);
//			wijhaat[i].raees.zaahir = 1;
//			amr_irsal_ilaa(wijhaat[i].rabt, RAEES, IZHAR, 1);
			return;
		}
	}
}
void clearmusheer(int i) {
	XATAA && printf("]%d[ (%d) -musheer\n", i, wijhaat[i].rabt);
	wijhaat[i].mshr.mutadarrar = 0;
	wijhaat[i].mshr.zaahir = 0;
	msfoof_rect(mowjoodah, i*musheerdim, height-musheerdim,
				musheerdim, musheerdim,
				theme.tertiary, theme.primary);
}
void clearraees(int i) {
	XATAA && printf("]%d[ (%d) -raees\n", i, wijhaat[i].rabt);
	wijhaat[i].raees.mutadarrar = 0;
	/* TODO BUG
	 * closing it from current process seems inefficient since it is needed right
	 * when another amr connects and requests a raees, this is shared memory
	 * closing it can only be beneficial on extremely lowmem devices with <256MB
	 * for some reason it is causing a bug when closed repeatedly
	 * 
	 * leaving it open:
	 * * doesn't leak memory
	 * * makes the umoor run out of ruasaa, need better tracking
	 * */
//	int hajm = raeesw*raeesh*sizeof(int);
//	amr_dhikr_ixttm(wijhaat[i].raees.xitaab, hajm); // close from current process
//	wijhaat[i].raees.xitaab = NULL;
	msfoof_rect(mowjoodah, 0, 0, width, raeesh, theme.tertiary, theme.primary);
}
void clearwijhah(int amr) {
	XATAA && printf("clearwijhah %d,  kul %d\n", amr, kulwijhaat);
	for (int i = 0; i < kulwijhaat; ++i) {
		if (wijhaat[i].rabt == amr) {
			strcpy(wijhaat[i].ism, "\0");
			wijhaat[i].rabt = 0;
			clearmusheer(i);
			clearraees(i);
			mutadarrar = 1;
		}
	}
	rakkazraeessaabiqah();
}
void ahkaam(waaqi3ah *w) { // commands, processes mafateeh
	if (markooz == 0 || zaahir == 0) return;
	if (w->miftaah == LOWHAH) {
		mafateeh m = { 0 };
		str2mafateeh(w->qadrstr, &m);
		shift = m.shift; meta = m.meta;
		ctrl = m.ctrl; alt = m.alt;
		int handled = 0;
		switch (m.key) {
			case KEY_F1:
			case KEY_F2:
			case KEY_F3:
			case KEY_F4:
			case KEY_F5:
			case KEY_F6:
			case KEY_F7:
			case KEY_F8:
			case KEY_F9:
			case KEY_F10:
			case KEY_F12:
//			case KEY_ESC:
				handled = 1;
				break;
		}
		if (m.state == 1) {
			if (m.key == KEY_F1 ) { rakkazraees(0); }
			if (m.key == KEY_F2 ) { rakkazraees(1); }
			if (m.key == KEY_F3 ) { rakkazraees(2); }
			if (m.key == KEY_F4 ) { rakkazraees(3); }
			if (m.key == KEY_F5 ) { rakkazraees(4); }
			if (m.key == KEY_F6 ) { rakkazraees(5); }
			if (m.key == KEY_F7 ) { rakkazraees(6); }
			if (m.key == KEY_F8 ) { rakkazraees(7); }
			if (m.key == KEY_F9 ) { rakkazraees(8); }
			if (m.key == KEY_F10) { rakkazraees(9); }
			if (m.key == KEY_F12) { iqtalraees(markoozraees); }
//			if (m.key == KEY_ESC) { dararraees(markoozraees); }
		}
		
		if (!handled)
		for (int i = 0; i < kulwijhaat; ++i) {
			if (wijhaat[i].rabt && i == markoozraees) {
				amr_irsal_ilaa_str(wijhaat[i].rabt, MUDEER, LOWHAH, w->qadrstr);
			}
		}
	} else
	if (w->miftaah == ISHAARAH) {
		mafateeh m = { 0 };
		str2mafateeh(w->qadrstr, &m);
		
		// input convergence, add kb mods to mouse events
		m.shift = shift; m.meta = meta;
		m.ctrl = ctrl; m.alt = alt;
		u_char *tmp = mafateeh2str(m);

		if (m.y > raeesh && m.state == -1) waspointerin = 1;
		if (waspointerin) rakkazraees( (int) m.x / musheerdim );
		if (m.state == 1) waspointerin = 0;

		if (m.y <= raeesh && !waspointerin)
		for (int i = 0; i < kulwijhaat; ++i) {
			if (wijhaat[i].rabt && i == markoozraees) {
				amr_irsal_ilaa_str(wijhaat[i].rabt, MUDEER, ISHAARAH, tmp);
			}
		}
		
		if (tmp) free(tmp);
	}
}
void testmtn() {
	frames++;
	long farq = beat();
	fps = 1000 / (farq / frames);
	unsigned char * p = helper_stringify("frame: %ld\neta: %f\nfps: %f", frames, (float) farq/1000, fps);
//	unsigned char * p = helper_stringify("%d\n%d\n%d\n", m.ctrl, m.shift, m.alt);
	if (p) {
		safaa();
		tb3matn(0, 0, p);
		free(p);
		for (int i = 0; i < kulwijhaat; ++i) {
			tb3matn(0, 150 + (ktb.x.h * ktb.g.s * i), wijhaat[i].ism);
		}
		mutadarrar = 1;
	}
}
int handler(waaqi3ah *w) {
	if (!w) return 0;
	if (XATAA > 1) {
		if (w->miftaah != INSHA && w->ism != MADXAL) {
			amr_tb3_waaqi3ah(w);
		}
	}
	if (w->ism == NAFS) {
		if (w->miftaah == ISM) {
			setamrism(w->li, w->qadrstr);
		}
	}
	if (w->ism == MADXAL) { ahkaam(w); }
	if (w->ism == MUSHEER) {
		if (w->miftaah == TABA3) {
			// TODO
		}
		if (w->miftaah == DARAR) {
			int mshr = filwijhaat(w->li);
			if (mshr > -1) {
				wijhaat[mshr].mshr.mutadarrar = 1;
				mutadarrar = 1;
			}
		}
		if (w->miftaah == HAAJA) {
			int wijhahxaali = axathwijhahxaali();
			if (w->li == 0) {
				XATAA && printf("unknown amr requested a musheer: ignored\n");
			} else
			if (wijhahxaali > -1) {
				if (filwijhaat(w->li) > -1) {
					XATAA && printf("(%ld) already has a musheer\n", w->li);
				} else {
					setmusheer(wijhahxaali, w->li);
					amr_irsal_ilaa(w->li, MUSHEER, MST3D, 0);
					amr_irsal_ilaa(w->li, MUSHEER, NAFS, wijhahxaali);
					amr_irsal_ilaa(w->li, MUSHEER, HAJMW, musheerdim);
					amr_irsal_ilaa(w->li, MUSHEER, HAJMH, musheerdim);
					amr_irsal_ilaa(w->li, MUSHEER, MST3D, 1);
					amr_irsal_ilaa(w->li, MUSHEER, IZHAR, 1);
					amr_irsal(ISM, MUSHEER, w->li); // ask waajihah for ism of this amr
				}
			} else {
				XATAA && printf("no musheer available for amr %ld\n", w->li);
				// notify amr that musheer stopped or not available
				amr_irsal_ilaa(w->li, MUSHEER, MOQOOF, 0);
			}
		}
	}
	if (w->ism == RAEES) {
		if (w->miftaah == TABA3) {
			int wjhh = filwijhaat(w->li);
			if (wjhh > -1) {
				taba3raees(wjhh, w);
			}
		}
		if (w->miftaah == DARAR) {
			int wjhh = filwijhaat(w->li);
			if (wjhh > -1) {
				wijhaat[wjhh].raees.mutadarrar = 1;
				mutadarrar = 1;
			}
		}
		if (w->miftaah == HAAJA) {
			int wijhahxaali = axathwijhahxaali();
			if (w->li == 0) {
				XATAA && printf("unknown amr requested a raees: ignored\n");
			} else
			if (wijhahxaali > -1) {
				int saabiqah = filwijhaat(w->li);
				if (saabiqah > -1) {
					setraees(saabiqah, w->li);
				} else {
					setraees(wijhahxaali, w->li);
				}
			} else {
				XATAA && printf("no raees available for amr %ld\n", w->li);
				// notify amr that musheer stopped or not available
				amr_irsal_ilaa(w->li, RAEES, MOQOOF, 0);
			}
		}
	}
	if (w->ism == MOQOOF) {
		if (w->miftaah == UMOOR) {
			clearwijhah(w->qadr);
		}
	}
	if (w->ism == MAXRAJ) {
		if (w->miftaah == INSHA) {
			if (allowrendering) {
				collectmusheeroon(); // sends INSHA to all zaahir umoor
				usleep(mspf);
				applymusheeroon(); // only composites mutadarrar zaahir umoor
//				testmtn();
				if (mutadarrar) {
//					taba3_waqt("start copy");
					memcpy(xitaab, mowjoodah->barzax, size);
//					taba3_waqt("end copy");
					mutadarrar = 0;
					// TODO darar regions
					amr_irsal(DARAR, MUDEER, 1); // damage, mudeer to maxraj
//					taba3_waqt("done darar -> x11");

					// flip mudeer barzax
//					ayyiframe = !ayyiframe;
//					mowjoodah = ayyiframe ? &overlay : &overlay2;
				} else { // add more wait if not damaged
//					usleep(mspf);
				}
				amr_irsal(NAQAF, MAXRAJ, 1); // flip, mudeer to maxraj
			}
		}
		if (w->miftaah == MST3D) {
			if (w->qadr == 0) allowrendering = 0;
			if (w->qadr == 1) {
				starttimer();
				allowrendering = 1;
				mutadarrar = 1;
				setupmemory();
			}
		}
		if (w->miftaah == RAKKAZ) {
			markooz = w->qadr;
			if (!markooz) waspointerin = 0;
			msfoof_rect(mowjoodah, 0, 0, width, 1,
				-1, markooz ? theme.accent : theme.tertiary);
			mutadarrar = 1;

			if (markoozraees > -1) {
				for (int i = 0; i < kulwijhaat; ++i) {
					if (wijhaat[i].rabt)  {
						if (i == markoozraees) {
							amr_irsal_ilaa(wijhaat[i].rabt, MUDEER, RAKKAZ, markooz);
						} else {
							amr_irsal_ilaa(wijhaat[i].rabt, MUDEER, RAKKAZ, markooz);
						}
					}
				}
			}
		}
		if (w->miftaah == IZHAR) {
			zaahir = w->qadr;
			mutadarrar = 1;
		}
		if (w->miftaah == HAJMW) {
			width = w->qadr;
			raeesw = width;
		}
		if (w->miftaah == HAJMH) {
			height = w->qadr;
			raeesh = height - musheerdim;
		}
	}

	return 0;
}
int main() {
	tarmeez_core();
	amr_towheed("mudeer");

	for (int i = 0; i < kulwijhaat; ++i) {
		wijhaat[i].mshr.mtrx.barzax = NULL;
		wijhaat[i].raees.mtrx.barzax = NULL;
		wijhaat[i].mshr.xitaab = NULL;
		wijhaat[i].raees.xitaab = NULL;
	}

	mowjoodah = &overlay;

	amr_irsal(HAAJA, MAXRAJ, 1); // listen on maxraj haajaat
	amr_irsal(HAAJA, MADXAL, 1);
	amr_irsal(HAAJA, MUSHEER, 1);
	amr_irsal(HAAJA, RAEES, 1);

	amr_irsal(NAFS, MUDEER, 1);

	amr_axath(&handler);

    return EXIT_SUCCESS;
}
