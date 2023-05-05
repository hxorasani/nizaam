/* composer - gui abstraction layer, can use multiple backends
 * sdl, glfw, nizaam, x11, wayland, ...
 * */

#include "composer.h"

#define XATAA 0

composer trkb;
canvas qmc = { 0 };

double composer_ease_by_name(int i, double t, double b, double c, double d) {
	double (*func)(double, double, double, double) = NULL;
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

char composer_cursor(char i) {
	if (i == -1) return SDL_ENABLE == SDL_ShowCursor(SDL_QUERY);
	if (i ==  0) SDL_ShowCursor(SDL_DISABLE);
	if (i ==  1) SDL_ShowCursor(SDL_ENABLE) ;
	return i;
}
void composer_canvas_context_window() {
	current_matrix = &WJHH.raees.mtrx;
}
void composer_canvas_context_indicator() {
	current_matrix = &WJHH.mshr.mtrx;
}
void composer_load_app(char i) { // get app
	if (trkb.on_reload) trkb.on_reload();

	// redo events
	if (i && trkb.on_bound_window  ) {
		composer_canvas_context_window();
		trkb.on_bound_window  ((ihaatahf) {0, 0, WJHH.raees.mtrx.w, WJHH.raees.mtrx.h});
	}
	if (i && trkb.on_bound_indicator) {
		composer_canvas_context_indicator();
		trkb.on_bound_indicator((ihaatahf) {0, 0, WJHH.mshr .mtrx.w, WJHH.mshr .mtrx.h});
	}

	WJHH.raees.mutadarrar = 1;
	WJHH.mshr.mutadarrar = 1;
}
void composer_clear() {
	int ret;
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        ret = system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        ret = system("cls");
    #endif
}
void composer_destroy() { // end, finish, destroy
}
int composer_handler(waaqi3ah *w, mafateeh *mif) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);
		if (w->ism == MUDEER && w->miftaah == RAKKAZ) {
			if (trkb.on_focus) {
				char yes = trkb.on_focus(w->qadr);
				if (yes) WJHH.raees.mutadarrar = 1;
			}
		}
		if (w->ism == MUDEER && w->miftaah == ROTATE) {
			char yes = 0;
			mafateeh m = { 0 };
			if (mif) m = *mif; else str2mafateeh(w->qadrstr, &m);

			if (!yes && trkb.on_rotate) yes = trkb.on_rotate(m);
			if (yes) {
				if (!WJHH.mshr.mutadarrar ) WJHH.mshr.mutadarrar  = 1;
				if (!WJHH.raees.mutadarrar) WJHH.raees.mutadarrar = 1;
			}
		}
		if (w->ism == MUDEER && w->miftaah == LAMSAH) {
			char yes = 0;
			mafateeh m = { 0 };
			if (mif) m = *mif; else str2mafateeh(w->qadrstr, &m);

			if (!yes && trkb.on_touch) yes = trkb.on_touch(m);
			if (yes) {
				if (!WJHH.mshr.mutadarrar ) WJHH.mshr.mutadarrar  = 1;
				if (!WJHH.raees.mutadarrar) WJHH.raees.mutadarrar = 1;
			}
		}
		if (w->ism == MUDEER && w->miftaah == PINCH) {
			char yes = 0;
			mafateeh m = { 0 };
			if (mif) m = *mif; else str2mafateeh(w->qadrstr, &m);

			if (!yes && trkb.on_pinch) yes = trkb.on_pinch(m);
			if (yes) {
				if (!WJHH.mshr.mutadarrar ) WJHH.mshr.mutadarrar  = 1;
				if (!WJHH.raees.mutadarrar) WJHH.raees.mutadarrar = 1;
			}
		}
		if (w->ism == MUDEER && w->miftaah == ISHAARAH) {
			char yes = 0;
			mafateeh m = { 0 };
			if (mif) m = *mif; else str2mafateeh(w->qadrstr, &m);
			
//			printf("[%d] %d %.1f %.1f\n", m.state, m.key, m.x, m.y);

			if (!yes && trkb.on_pointer) yes = trkb.on_pointer(m);
			if (yes) {
				if (!WJHH.mshr.mutadarrar ) WJHH.mshr.mutadarrar  = 1;
				if (!WJHH.raees.mutadarrar) WJHH.raees.mutadarrar = 1;
			}
		}
		if (w->ism == MUDEER && w->miftaah == LOWHAH) {
			char yes = 0;
			mafateeh m = { 0 };
			if (mif) m = *mif; else str2mafateeh(w->qadrstr, &m);

//			printf("[%d] c%d s%d a%d m%d %d %s\n", m.state, m.ctrl, m.shift, m.alt, m.meta, m.key, m.ism);

			if (!m.state) {
				if (m.ctrl) {
					if (m.key == KEY_L) composer_clear(), yes = 1;
					if (m.key == KEY_R) composer_load_app(1), yes = 1;
					if (m.key == KEY_Q) exit(0), yes = 1;
				}
			}
			if (!yes && trkb.b_lowhah) {
				yes = trkb.b_lowhah(m);
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

				composer_canvas_context_indicator();

				if (w->qadr == 1)
					if (trkb.on_bound_indicator) trkb.on_bound_indicator( (ihaatahf) {0, 0, width, height} );
			}
			if (w->miftaah == INSHA && WJHH.mshr.zaahir) {
				current_matrix = &WJHH.mshr.mtrx;
				if (WJHH.mshr.mutadarrar) {
					char darar = 0;
					// if has C func override, call that first
					if (trkb.on_paint_indicator) darar = trkb.on_paint_indicator();

					if (darar) {
						if (MUDEERUID > -1) // IMPORTANT when not on nizaam-waajihah backend
						amr_irsal_ilaa(MUDEERUID, MUSHEER, DARAR, 1);
					}

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
				
				if (w->qadr == 1)
					if (trkb.on_bound_window) trkb.on_bound_window( (ihaatahf) {0, 0, width, height} );
			}
			if (w->miftaah == INSHA && WJHH.raees.zaahir) {
				current_matrix = &WJHH.raees.mtrx;
				if (WJHH.raees.mutadarrar) {
					char darar = 0;
					// if has C func override, call that first
					if (trkb.on_paint_window) darar = trkb.on_paint_window();

					if (darar) {
						if (trkb.after_canvas_raees)
							trkb.after_canvas_raees(&WJHH.raees.mtrx);

						if (MUDEERUID > -1) // IMPORTANT when not on nizaam-waajihah backend
						amr_irsal_ilaa(MUDEERUID, RAEES, DARAR, 1);
					}

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

	trkb.ism					= t.ism					;
	trkb.xitaab					= t.xitaab				;
	trkb.on_touch				= t.on_touch			;
	trkb.on_pinch				= t.on_pinch			;
	trkb.on_rotate				= t.on_rotate			;
	trkb.b_lowhah				= t.b_lowhah			;
	trkb.on_pointer				= t.on_pointer			;
	trkb.on_reload				= t.on_reload			;
	trkb.on_paint_indicator		= t.on_paint_indicator	;
	trkb.on_paint_window		= t.on_paint_window		;
	trkb.on_bound_indicator		= t.on_bound_indicator	;
	trkb.on_bound_window		= t.on_bound_window		;
	trkb.after_canvas_raees		= t.after_canvas_raees	;
	trkb.on_tick				= t.on_tick				;
	trkb.on_drop				= t.on_drop				;
	trkb.on_focus				= t.on_focus			;
	trkb.set_window				= t.set_window			;
	trkb.event_provider			= t.event_provider		;
	
	current_canvas = &qmc;

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
	
	composer_destroy();
}

