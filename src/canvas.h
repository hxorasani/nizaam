#ifndef _canvas_h
#define _canvas_h

#include "msfoof.h"

/* canvas is a struct that contains pointers to drawing functions
 * these functions are opaque and should be overriden by a renderer
 * composer elements depend on canvas for actual drawing calls
 * this is done to make the elements rendering backend agnostic
 * */

typedef struct canvas { // canvas
	void (*flush)();
	void (*blur)(double);

	void (*xat_wijhah)(const char *); // font face
	void (*font_size)(float);
	void (*font_bounds)(ihaatahf *);

	void (*darar)(ihaatah); // report damage in a rect
	// void (*darar)(ihaatahf, *niqaab); // report damage in a masked rect
	
	void (*set_pixel)(u_int x, u_int y, u_char a, u_char r, u_char g, u_char b);

	void (*harf_hajm)(ihaatahf *, int); // harf extents/bound
	void (*huroof_hajm)(ihaatahf *, int *); // huroof extents/bound
	void (*text_bounds)(ihaatahf *, char *); // ascii text bounds

	void (*matn_harf)(int); // render char
	void (*matn_huroof)(int *); // render chars
	void (*show_text)(char *);

	void (*move_to)(float, float); // move to
	
	void (*line_width)(float);
	void (*set_dash)(double *, int, double);

	void (*satar)(float, float); // line
	void (*mustateel)(ihaatahf); // rect
	void (*circle)(float, float, float, float, float);

	void (*lwn_rgb)(float, float, float); // set color
	void (*lwn_rgba)(float, float, float, float); // set color with alpha

	void (*stroke)(char); // stroke
	void (*fill)(char); // fill
	void (*renew_path)(); // renew path

	void (*translate)(float, float);
	void (*rotate)(float);
	void (*scale)(float, float);
	void (*clip)();
	void (*unclip)();

	void (*user_to_device)(double *, double *);
	void (*device_to_user)(double *, double *);

	void (*save)();
	void (*restore)();

//	void (*soorah_axav)(ihaatahf); // image

	void (*paint)(); // paint
} canvas;

#endif