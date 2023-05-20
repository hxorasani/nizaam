#include "sdl.h"

#define XATAA 0
#define ENABLE_NUK 1
#define s2c(key, rep) if (k == key) b = rep;

SDL_Window *sdl_window = NULL;
SDL_GLContext glContext;
static composer *sdl_composer;
static mafateeh wm = { 0 };
static char gl_loaded = 0;

static int (*sdl_composer_event_handler)() = NULL;
static int frame_rate = 70;
static float frame_delay = 14; // 1000 / frame_rate

void sdl_emit_event(int name, int key, int value) {
	waaqi3ah w = { 0 };
	w.ism = name; w.miftaah = key; w.qadr = value;
	if (sdl_composer_event_handler) sdl_composer_event_handler(&w, NULL);
}
void sdl_render(SDL_Window *sdl_window) { // TODO efficiency
	if (XATAA) printf("render\n");
	sdl_emit_event(RAEES, INSHA, 1);

	if (ENABLE_NUK) nuk_render();
	SDL_GL_SwapWindow(sdl_window); // Display the result
}
void sdl_matrix_canvas_window(int width, int height) {
	if (XATAA) printf("matrix_canvas_window\n");
	msfoof *m = &WJHH.raees.mtrx;

	// these are used by composer for reporting window props to dukjs
	m->w = width;
	m->h = height;
	m->stride = width * 4;
	WJHH.raees.zaahir = 1;
	WJHH.raees.mutadarrar = 1;
}
int  sdl_key_to_composer(int k) {
	int b = k;
	s2c(SDLK_ESCAPE, KEY_ESC)

	s2c(SDLK_0, KEY_0) s2c(SDLK_1, KEY_1) s2c(SDLK_2, KEY_2) s2c(SDLK_3, KEY_3)
	s2c(SDLK_4, KEY_4) s2c(SDLK_5, KEY_5) s2c(SDLK_6, KEY_6) s2c(SDLK_7, KEY_7)
	s2c(SDLK_8, KEY_8) s2c(SDLK_9, KEY_9)

	s2c(SDLK_MINUS, KEY_MINUS) s2c(SDLK_EQUALS, KEY_EQUAL)
	s2c(SDLK_BACKSPACE, KEY_BACKSPACE) s2c(SDLK_TAB, KEY_TAB)

	s2c(SDLK_q, KEY_Q) s2c(SDLK_w, KEY_W) s2c(SDLK_e, KEY_E) s2c(SDLK_r, KEY_R)
	s2c(SDLK_t, KEY_T) s2c(SDLK_y, KEY_Y) s2c(SDLK_u, KEY_U) s2c(SDLK_i, KEY_I)
	s2c(SDLK_o, KEY_O) s2c(SDLK_p, KEY_P)

	s2c(SDLK_LEFTBRACKET, KEY_LEFTBRACE) s2c(SDLK_RIGHTBRACKET, KEY_RIGHTBRACE)
	s2c(SDLK_RETURN, KEY_ENTER) s2c(SDLK_LCTRL, KEY_LEFTCTRL)

	s2c(SDLK_a, KEY_A) s2c(SDLK_s, KEY_S) s2c(SDLK_d, KEY_D) s2c(SDLK_f, KEY_F)
	s2c(SDLK_g, KEY_G) s2c(SDLK_h, KEY_H) s2c(SDLK_j, KEY_J) s2c(SDLK_k, KEY_K)
	s2c(SDLK_l, KEY_L)

	s2c(SDLK_SEMICOLON, KEY_SEMICOLON) s2c(SDLK_QUOTE, KEY_APOSTROPHE)
	s2c(SDLK_BACKQUOTE, KEY_GRAVE) s2c(SDLK_LSHIFT, KEY_LEFTSHIFT)
	s2c(SDLK_BACKSLASH, KEY_BACKSLASH)

	s2c(SDLK_z, KEY_Z) s2c(SDLK_x, KEY_X) s2c(SDLK_c, KEY_C) s2c(SDLK_v, KEY_V)
	s2c(SDLK_b, KEY_B) s2c(SDLK_n, KEY_N) s2c(SDLK_m, KEY_M)

	s2c(SDLK_COMMA, KEY_COMMA) s2c(SDLK_PERIOD, KEY_DOT)
	s2c(SDLK_SLASH, KEY_SLASH) s2c(SDLK_RSHIFT, KEY_RIGHTSHIFT)
	s2c(SDLK_KP_MULTIPLY, KEY_KPASTERISK) s2c(SDLK_LALT, KEY_LEFTALT)
	s2c(SDLK_SPACE, KEY_SPACE) s2c(SDLK_CAPSLOCK, KEY_CAPSLOCK)

	s2c(SDLK_F1, KEY_F1) s2c(SDLK_F2, KEY_F2) s2c(SDLK_F3, KEY_F3)
	s2c(SDLK_F4, KEY_F4) s2c(SDLK_F5, KEY_F5) s2c(SDLK_F6, KEY_F6)
	s2c(SDLK_F7, KEY_F7) s2c(SDLK_F8, KEY_F8) s2c(SDLK_F9, KEY_F9)
	s2c(SDLK_F10, KEY_F10)

	s2c(SDLK_NUMLOCKCLEAR, KEY_NUMLOCK) s2c(SDLK_SCROLLLOCK, KEY_SCROLLLOCK)

	s2c(SDLK_KP_7, KEY_KP7) s2c(SDLK_KP_8, KEY_KP8) s2c(SDLK_KP_9, KEY_KP9)
	s2c(SDLK_KP_MINUS, KEY_KPMINUS) s2c(SDLK_KP_4, KEY_KP4) s2c(SDLK_KP_5, KEY_KP5)
	s2c(SDLK_KP_6, KEY_KP6) s2c(SDLK_KP_PLUS, KEY_KPPLUS) s2c(SDLK_KP_1, KEY_KP1)
	s2c(SDLK_KP_2, KEY_KP2) s2c(SDLK_KP_3, KEY_KP3) s2c(SDLK_KP_0, KEY_KP0)
	s2c(SDLK_KP_PERIOD, KEY_KPDOT)
//	ZENKAKUHANKAKU '102ND' n/a
	s2c(SDLK_F11, KEY_F11) s2c(SDLK_F12, KEY_F12)
//	89 - 96 n/a skipped
	s2c(SDLK_RCTRL, KEY_RIGHTCTRL) s2c(SDLK_KP_DIVIDE, KEY_KPSLASH)
	s2c(SDLK_SYSREQ, KEY_SYSRQ) s2c(SDLK_RALT, KEY_RIGHTALT)
	s2c(SDLK_HOME, KEY_HOME) s2c(SDLK_UP, KEY_UP) s2c(SDLK_PAGEUP, KEY_PAGEUP)
	s2c(SDLK_LEFT, KEY_LEFT) s2c(SDLK_RIGHT, KEY_RIGHT) s2c(SDLK_END, KEY_END)
	s2c(SDLK_DOWN, KEY_DOWN) s2c(SDLK_PAGEDOWN, KEY_PAGEDOWN) s2c(SDLK_INSERT, KEY_INSERT)
	s2c(SDLK_DELETE, KEY_DELETE) s2c(SDLK_MUTE, KEY_MUTE)
	s2c(SDLK_VOLUMEDOWN, KEY_VOLUMEDOWN) s2c(SDLK_VOLUMEUP, KEY_VOLUMEUP)
	s2c(SDLK_POWER, KEY_POWER) s2c(SDLK_KP_EQUALS, KEY_KPEQUAL)
	s2c(SDLK_PAUSE, KEY_PAUSE) s2c(SDLK_KP_COMMA, KEY_KPCOMMA)
//	122 - 124 n/a skipped
	s2c(SDLK_LGUI, KEY_LEFTMETA) s2c(SDLK_RGUI, KEY_RIGHTMETA)
	s2c(SDLK_MENU, KEY_COMPOSE)
	s2c(SDLK_APPLICATION, KEY_COMPOSE)
	s2c(SDLK_STOP, KEY_STOP)
	s2c(SDLK_AGAIN, KEY_AGAIN)
//	130
	s2c(SDLK_UNDO, KEY_UNDO)
//	132
	s2c(SDLK_COPY, KEY_COPY)
//	134
	s2c(SDLK_PASTE, KEY_PASTE)
	s2c(SDLK_FIND, KEY_FIND)
	s2c(SDLK_CUT, KEY_CUT)
	s2c(SDLK_HELP, KEY_HELP)

	s2c(SDLK_F13, KEY_F13) s2c(SDLK_F14, KEY_F14) s2c(SDLK_F15, KEY_F15)
	s2c(SDLK_F16, KEY_F16) s2c(SDLK_F17, KEY_F17) s2c(SDLK_F18, KEY_F18)
	s2c(SDLK_F19, KEY_F19) s2c(SDLK_F20, KEY_F20) s2c(SDLK_F21, KEY_F21)
	s2c(SDLK_F22, KEY_F22) s2c(SDLK_F23, KEY_F23) s2c(SDLK_F24, KEY_F24)
	s2c(SDLK_UNKNOWN, KEY_UNKNOWN)

	return b;
}
int  sdl_event_handler(void *data, SDL_Event *e) {
	if (XATAA) printf("event handler\n");
	waaqi3ah w = { 0 }; char yes = 0;
	wm.key = 0;
	wm.w = 0;
	matn_nazaf_str(wm.ism, 32);
	matn_nazaf_str(wm.huroof, 32);
	msfoof *m = &WJHH.raees.mtrx;
	
	if (e->type == SDL_TEXTINPUT) {
		strcpy(wm.ism, e->text.text);
		strcpy(wm.huroof, e->text.text);

		w.ism = MUDEER; w.miftaah = LOWHAH; yes = 1;
	} else if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP) {
		int k = e->key.keysym.sym;
		if (!(k == SDLK_LSHIFT || k == SDLK_RSHIFT
		||  k == SDLK_LALT   || k == SDLK_RALT
		||  k == SDLK_LCTRL  || k == SDLK_RCTRL
		||  k == SDLK_LGUI   || k == SDLK_RGUI  ))
			wm.key = sdl_key_to_composer( k );

		wm.state = 0;
		if (e->key.state == SDL_RELEASED) wm.state = 0;
		if (e->key.state == SDL_PRESSED) wm.state = 1;
		if (e->key.repeat) wm.state = 2;
		
		SDL_Keymod mod = e->key.keysym.mod;
        wm.ctrl  = mod & KMOD_CTRL	? 1 : 0;
        wm.shift = mod & KMOD_SHIFT	? 1 : 0;
        wm.alt   = mod & KMOD_ALT	? 1 : 0;
        wm.meta  = mod & KMOD_GUI	? 1 : 0;

		w.ism = MUDEER; w.miftaah = LOWHAH; yes = 1;
	} else if (e->type == SDL_MOUSEBUTTONUP || e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) wm.key = 1;
		if (e->button.button == SDL_BUTTON_RIGHT) wm.key = 2;
		if (e->button.button == SDL_BUTTON_MIDDLE) wm.key = 3;
		if (e->type == SDL_MOUSEBUTTONUP) wm.state = 1;
		if (e->type == SDL_MOUSEBUTTONDOWN) wm.state = -1;

		w.ism = MUDEER; w.miftaah = ISHAARAH; yes = 1;
	} else if (e->type == SDL_MOUSEWHEEL) {
		wm.wx = e->wheel.x;
		wm.wy = e->wheel.y;
		
		if (e->wheel.y > 0) wm.w = -1;
		if (e->wheel.y < 0) wm.w =  1;

		w.ism = MUDEER; w.miftaah = ISHAARAH; yes = 1;
	} else if (e->type == SDL_DROPFILE) {
		char *dropped_filedir = e->drop.file;
		if (sdl_composer && sdl_composer->on_drop) sdl_composer->on_drop(dropped_filedir);
		SDL_free(dropped_filedir);
	} else if (e->type == SDL_MOUSEMOTION) {
		wm.x = e->motion.x;
		wm.y = e->motion.y;
		wm.key = 0;
		wm.state = 0;

		w.ism = MUDEER; w.miftaah = ISHAARAH; yes = 1;
	} else if (e->type == SDL_FINGERMOTION || e->type == SDL_FINGERDOWN || e->type == SDL_FINGERUP) {
		wm.x = e->tfinger.x * m->w;
		wm.y = e->tfinger.y * m->h;
		wm.w = e->tfinger.fingerId;
		wm.wx = e->tfinger.dx * m->w;
		wm.wy = e->tfinger.dy * m->h;
		
		w.ism = MUDEER; w.miftaah = LAMSAH; yes = 1;
	} else if (e->type == SDL_MULTIGESTURE) {
		// TODO make a session out of these, like lock it b/w lifts
		if ( fabs( e->mgesture.dTheta ) > 3.14 / (360*2) ) { // rotation
			wm.x = e->mgesture.x * m->w;
			wm.y = e->mgesture.y * m->h;
			wm.w = e->mgesture.dTheta;

			w.ism = MUDEER; w.miftaah = ROTATE; yes = 1;
		}
		else if ( fabs( e->mgesture.dDist ) ) { // pinch
			wm.x = e->mgesture.x * m->w;
			wm.y = e->mgesture.y * m->h;
			wm.w = e->mgesture.dDist;
			if ( wm.w > 0 ) { // open
				wm.state = 1;
			} else { // close
				wm.state = -1;
			}
			w.ism = MUDEER; w.miftaah = PINCH; yes = 1;
		}
	} else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
//		WJHH.raees.zaahir = 1;
		
		w.ism = MUDEER; w.miftaah = RAKKAZ; w.qadr = 1; yes = 1;
	} else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
//		WJHH.raees.zaahir = 0;

		w.ism = MUDEER; w.miftaah = RAKKAZ; w.qadr = 0; yes = 1;
	} else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_RESIZED) {
		sdl_matrix_canvas_window(e->window.data1, e->window.data2);
		
		w.ism = RAEES; w.miftaah = MST3D; w.qadr = 1; yes = 1;
	}

	if (yes && sdl_composer_event_handler) sdl_composer_event_handler(&w, &wm);
}
void sdl_main_loop(SDL_Window *sdl_window) {
	while (true) {
		if (ENABLE_NUK) nuk_input_start();
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				sdl_composer_bridge_destroy();
				return;
			}
			if (ENABLE_NUK) nuk_input_event(&ev);
		}
		if (ENABLE_NUK) nuk_input_end();

		if (sdl_composer && sdl_composer->on_tick) sdl_composer->on_tick();
		if (WJHH.raees.mutadarrar) sdl_render(sdl_window);

		SDL_Delay( frame_delay ); // try to get a redraw-rate of 50 Hz
	}
}
char sdl_destroy_gl() {
	if (sdl_window == NULL || glContext == NULL) return 0;
	
	if (ENABLE_NUK) nuk_destroy();
	SDL_GL_DeleteContext(glContext);
	glContext = NULL;
	gl_loaded = 0;
	
	return 1;
}
char sdl_init_gl() {
	if (gl_loaded) return 1;
	if (glContext != NULL) return 1; // already init'd
	if (sdl_window == NULL) return 0;
	
	if (XATAA) printf("init gl\n");
	
    glContext = SDL_GL_CreateContext(sdl_window);
	if (glContext == NULL) { printf("Error: SDL_GL_CreateContext: %s\n", SDL_GetError()); exit(EXIT_FAILURE); }
	gl_loaded = 1;

	GLenum glew_status = glewInit();
    glewExperimental = 1;
	if (glew_status != GLEW_OK) { printf("Error: glewInit: %s\n", glewGetErrorString(glew_status)); exit(EXIT_FAILURE); }

	if (ENABLE_NUK) nuk_start(sdl_window);
	
	return 1;
}
void sdl_set_frame_rate(int fps) {
	if (fps < 1) fps = 1;
	if (fps > 120) fps = 120;
	frame_rate = fps;
	frame_delay = (float) 1000 / fps;
}
int sdl_get_frame_rate() {
	return frame_rate;
}
void sdl_set_title(const u_char *s) {
	prctl(PR_SET_NAME, (unsigned long) s, 0, 0, 0);
	if (sdl_window) SDL_SetWindowTitle(sdl_window, s);
}
void sdl_set_window(u_char *name) {
	if (XATAA) printf("set window\n");
	int width = 240*3, height = 320*2.5;
	
    SDL_Init(SDL_INIT_VIDEO);

//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
//	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
//	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
//	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
//	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	sdl_window = SDL_CreateWindow( name,
			100, 100,
//			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
		);

	if (sdl_window == NULL) { printf("Error: can't create sdl_window: %s\n", SDL_GetError()); exit(EXIT_FAILURE); }

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	SDL_AddEventWatch(sdl_event_handler, sdl_window);

	sdl_init_gl(sdl_window);
	
	sdl_matrix_canvas_window(width, height);
}
void sdl_set_event_provider( int (*onwaaqi3ah)() ) {
	if (XATAA) printf("set_event_provider\n");
	sdl_composer_event_handler = onwaaqi3ah;
	sdl_emit_event(RAEES, MST3D, 1);
	sdl_main_loop(sdl_window);
}
void sdl_composer_bridge(composer *cmpsr) {
	// app.dukjs will provide this to sdl
	sdl_composer = cmpsr;
	cmpsr->title = &sdl_set_title;
	cmpsr->set_window = &sdl_set_window;
	cmpsr->event_provider = &sdl_set_event_provider;
}
void sdl_composer_bridge_destroy() {
	sdl_composer = NULL;
	sdl_destroy_gl();
}




