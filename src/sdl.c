#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "composer.h"

#define s2c(key, rep) if (k == key) b = rep;

SDL_Window* sdl_window;
composer *sdl_composer;
mafateeh wm = { 0 };

u_int texture_id;
u_char *sdl_buffer = NULL;
int sdl_buffer_size = 0;
int (*sdl_composer_event_handler)() = NULL;

void sdl_resize_handler (int width, int height, u_int* texture_id) {
	glViewport (0, 0, width, height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDeleteTextures (1, texture_id);
	glGenTextures (1, texture_id);
	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, *texture_id);
	glTexImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA,
		      width, height, 0,
		      GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}
void sdl_draw_texture (int width, int height, u_char* surf_data, u_int texture_id) {
	if (!surf_data) { printf ("sdl_draw_texture - no surface-data\n"); return; }

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	glPushMatrix ();

	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, texture_id);
	glTexImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA,
		      width, height, 0,
		      GL_BGRA, GL_UNSIGNED_BYTE,
		      surf_data);

	glColor4f (0, 0, 0, 1);
	glBegin (GL_QUADS);
	glTexCoord2f (0.0f, 0.0f);							glVertex2f (0.0f, 1.0f);
	glTexCoord2f ((GLfloat) width, 0.0f);				glVertex2f (1.0f, 1.0f);
	glTexCoord2f ((GLfloat) width, (GLfloat) height);	glVertex2f (1.0f, 0.0f);
	glTexCoord2f (0.0f, (GLfloat) height);				glVertex2f (0.0f, 0.0f);
	glEnd ();

	glPopMatrix ();
}
void sdl_matrix_canvas_window(int width, int height) {
	msfoof *m = &WJHH.raees.mtrx; char grown = 0;

	// if size is bigger than prev size, grow, else shrink without free
	if (!sdl_buffer || width*height > sdl_buffer_size) {
		if (sdl_buffer) free(sdl_buffer);
		sdl_buffer = malloc(4 * width * height * sizeof (u_char));
		if (!sdl_buffer) { printf ("couldn't allocate sdl_buffer\n"); exit(EXIT_FAILURE); }
		sdl_buffer_size = width*height;
		grown = 1;
	}

	m->w = width;
	m->h = height;
	m->stride = width * 4;

	if (grown) { // this check makes resizing much faster!
		m->barzax = (u_int *) sdl_buffer;
		msfoof_clear(m, 0); // TODO switch matrix to u_char *, or it doesn't work directly with cairo
	}
	
	WJHH.raees.zaahir = 1;

	sdl_resize_handler (width, height, &texture_id);
}
void sdl_emit_event(int name, int key, int value) {
	waaqi3ah w = { 0 };
	w.ism = name; w.miftaah = key; w.qadr = value;
	if (sdl_composer_event_handler) sdl_composer_event_handler(&w, NULL);
}
int sdl_key_to_composer(int k) {
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
int  sdl_event_handler(void* data, SDL_Event* e) {
	waaqi3ah w = { 0 }; char yes = 0;
	wm.key = 0;
	wm.w = 0;
	matn_nazaf_str(wm.ism, 32);
	matn_nazaf_str(wm.huroof, 32);
	
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
	} else if (e->type == SDL_MOUSEMOTION) {
		wm.x = e->motion.x;
		wm.y = e->motion.y;
		wm.key = 0;
		wm.state = 0;

		w.ism = MUDEER; w.miftaah = ISHAARAH; yes = 1;
	} else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
		WJHH.raees.zaahir = 1;
		
		w.ism = MUDEER; w.miftaah = RAKKAZ; w.qadr = 1; yes = 1;
	} else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
		WJHH.raees.zaahir = 0;

		w.ism = MUDEER; w.miftaah = RAKKAZ; w.qadr = 0; yes = 1;
	} else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_RESIZED) {
		sdl_matrix_canvas_window(e->window.data1, e->window.data2);
		
		w.ism = RAEES; w.miftaah = MST3D; w.qadr = 1; yes = 1;
	} else if (WJHH.raees.zaahir) {
		w.ism = RAEES; w.miftaah = INSHA; yes = 1;
	}

	if (yes && sdl_composer_event_handler) sdl_composer_event_handler(&w, &wm);
}
void sdl_render(SDL_Window* sdl_window) { // TODO efficiency
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sdl_emit_event(RAEES, INSHA, 1);
	sdl_draw_texture(WJHH.raees.mtrx.w, WJHH.raees.mtrx.h, sdl_buffer, texture_id);
	SDL_GL_SwapWindow(sdl_window); // Display the result
}
void sdl_main_loop(SDL_Window* sdl_window) {
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) if (ev.type == SDL_QUIT) return;
		
		sdl_render(sdl_window);

		SDL_Delay(10); // try to get a redraw-rate of 50 Hz
	}
}
void sdl_init_gl(SDL_Window *sdl_window) {
	if (SDL_GL_CreateContext(sdl_window) == NULL) { printf("Error: SDL_GL_CreateContext: %s\n", SDL_GetError()); exit(EXIT_FAILURE); }

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) { printf("Error: glewInit: %s\n", glewGetErrorString(glew_status)); exit(EXIT_FAILURE); }

	glEnable(GL_BLEND); // Enable alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
}
void sdl_set_window(u_char *name) {
	int width = 240*2.5, height = 320*2.5;
	
	SDL_Init(SDL_INIT_VIDEO);
	sdl_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (sdl_window == NULL) { printf("Error: can't create sdl_window: %s\n", SDL_GetError()); exit(EXIT_FAILURE); }

    SDL_AddEventWatch(sdl_event_handler, sdl_window);
	
	sdl_init_gl(sdl_window);
	sdl_matrix_canvas_window(width, height);
}
void sdl_set_event_provider( int (*onwaaqi3ah)() ) {
	sdl_composer_event_handler = onwaaqi3ah;

	sdl_emit_event(RAEES, MST3D, 1);

	sdl_main_loop(sdl_window);
}
void sdl_composer_bridge(composer *cmpsr) {
	// app.dukjs will provide this to sdl
	cmpsr->set_window = &sdl_set_window;
	cmpsr->event_provider = &sdl_set_event_provider;
}
void sdl_composer_destroy() { // TODO tie into cmpsr.on_destroy
	glDeleteTextures (1, &texture_id);
	if (sdl_buffer) { free(sdl_buffer); sdl_buffer = NULL; }
}




