/* madxal: multiplexes linux input waaqi3aat into a single socket stream
 * 
 * madxal runs as a waajihah amr
 * it irsals LOWHAH ([key]board), ISHAARAH (pointer,gesture) waaqi3aat
 * 
 * these are not raw waaqi3aat, they're processed in madxal
 * 
 * certain kinds of events are combined into standard formats:
 * keyboard, hardware buttons... >> LOWHAH
 * touchscreen, touchpad >> ISHAARAH
 * mouse, pen, joystick >> ISHAARAH ?
 * */

#include "madxal.h"

#define   XATAA 0
//#define	  MAX_EVENTS 10

int destruction, ctrl, alt = -100, meta, shift, capsl = 0, language = 1; // en
int mousex = 0, mousey = 0, mousew = 0;
float touchpadx = 0, touchpady = 0, touchpadw = 0;
int touchpadfingers = 0;
//struct thread_info *threads = { 0 };
struct masdar *fdtomasdar[256];
typedef struct miftaah {
	int i; // where to insert in harf
	unsigned char huroof[4];
} miftaah;
miftaah _$la		[512],
		_$lashift	[512],
		_$ar		[512],
		_$arshift	[512],
		_$alt		[512];

const static struct {
	const int m;
	const char *s;
} conversion [] = {
	#include "miftaah.h"
};
const int str2miftaah (const char *str) {
	for (int j = 0; j <  sizeof conversion/sizeof conversion[0]; ++j) {
		if (!strcmp(str, conversion[j].s))
			return conversion[j].m;
	}
	printf("str2miftaah: no enum for %s\n", str);
	return -1;
}
void madxal_fak_adaaf(int dest, int key, unsigned char mfth) {
//	printf("madxal_fak_adaaf %d %d %d\n", dest, key, mfth);
	if (dest == 0) _$la[key].huroof[ _$la[key].i++ ] = mfth;
	if (dest == 1) _$lashift[key].huroof[ _$lashift[key].i++ ] = mfth;
	if (dest == 2) _$ar[key].huroof[ _$ar[key].i++ ] = mfth;
	if (dest == 3) _$arshift[key].huroof[ _$arshift[key].i++ ] = mfth;
	if (dest == 4) _$alt[key].huroof[ _$alt[key].i++ ] = mfth;
}
void madxal_fak(char *path) { // key to char defs
	XATAA && printf("madxal_fak %s\n", path);
	malaf mlf;
	malaf_init(&mlf, path);

	matn m;
	matn_init(&m);
	malaf_ilaa_matn(&mlf, &m.barzax, &m.length);

	malaf_destroy(&mlf);
	
	unsigned char c, col = 0, line = 0;
	int strlimit = 24, dest = 0;
	int str[strlimit];
	int stri = 0; // key string index
	int key = 0; // key string as int
	for (long i = 0; i < m.length+1; ++i) {
		if (i == m.length) c = 9; // to allow indexing the last column
		else c = m.barzax[i];

		if (c == 9 || c == 10) { // came across a tab or new line
			// either way
			str[stri] = '\0'; // null terminate string
			if (col == 0) { // exiting first column
				key = str2miftaah((unsigned char *) str);
				XATAA > 2 && printf("A: (%x) %ls ", str[0], str);
			} else
			if (col > 0) { // exiting column 2 and onwards
				if (key > -1) {
					unsigned char mfth = (unsigned char) wcstol(str, (wchar_t **) &str[stri], 10);
					madxal_fak_adaaf(dest, key, mfth);
				}
			}
			if (c == 10) { // only when it's a new line
				XATAA > 2 && printf("\n[%d] ", line);
				if ( utf8_cmp_uchar(str, "=la"		) ) dest = 0;
				if ( utf8_cmp_uchar(str, "=lashift"	) ) dest = 1;
				if ( utf8_cmp_uchar(str, "=ar"		) ) dest = 2;
				if ( utf8_cmp_uchar(str, "=arshift"	) ) dest = 3;
				if ( utf8_cmp_uchar(str, "=alt"		) ) dest = 4;
				
				col = 0; // back to the first column
				key = -1; // key not read yet
				line++;
			} else
			if (c == 9) { // only when it's a new tab
				col++; // going to the next column
			}

			utf8_emptystr(str, strlimit); // reset string
			stri = 0; // reset key string index
		} else {
			if (stri < strlimit) {
				XATAA > 2 && printf("(%d)", col);
				str[stri] = c;
				stri++;
			}
		}
	}
	
	matn_destroy(&m);
}

void parsekbevent(struct input_event e, void (*handler)()) {
	if (e.type == EV_KEY) {
		if (e.code == KEY_CAPSLOCK && e.value == 0) {
			capsl = !capsl;
		}
		mafateeh wm;
		wm.type = KEYBOARD;
		if (alt == -100) alt = 0;
		wm.key = 0;
		wm.state = 0;
		strcpy(wm.huroof, "\0");
		char caught = 0;
		if (e.code == KEY_LEFTCTRL || e.code == KEY_RIGHTCTRL) {
			ctrl = e.value;
		} else if (e.code == KEY_LEFTSHIFT || e.code == KEY_RIGHTSHIFT) {
			shift = e.value;
		} else if (e.code == KEY_LEFTALT || e.code == KEY_RIGHTALT) {
			alt = e.value;
		} else if (e.code == KEY_LEFTMETA || e.code == KEY_RIGHTMETA) {
			meta = e.value;
		} else {
			wm.key = e.code; wm.state = e.value;
			if (meta && e.code == KEY_SPACE) {
				caught = 1;
				if (e.value == 0) {
					language = !language;
					XATAA && printf("language %d\n", language);
				}
			} else
			if (e.code < 512) {
				u_char huroof[4], ism[4];
				ism[0] = '\0';
				huroof[0] = '\0';

				if (alt) {
					if (_$alt[e.code].i) {
						strcpy(huroof, _$alt[e.code].huroof);
					}
				} else
				if (shift || capsl) {
					if (language) {
						if (_$lashift[e.code].i)
						strcpy(huroof, _$lashift[e.code].huroof);
					} else {
						if (_$arshift[e.code].i)
						strcpy(huroof, _$arshift[e.code].huroof);
					}
				} else {
					if (language) {
						if (_$la[e.code].i)
						strcpy(huroof, _$la[e.code].huroof);
					} else {
						if (_$ar[e.code].i)
						strcpy(huroof, _$ar[e.code].huroof);
						
					}
				}
				
				if (huroof[0] > 0 && huroof[0] < 256) strcpy(wm.huroof, huroof);
				else strcpy(wm.huroof, "\0");

				// ism is always latin based for compat with kaafir keyboards
				if (_$la[e.code].i)
				strcpy(ism, _$la[e.code].huroof);

				if (ism[0] > 0 && ism[0] < 256) strcpy(wm.ism, ism);
				else strcpy(wm.ism, "\0");
			}
		}
		wm.ctrl  = ctrl;
		wm.shift = shift;
		wm.alt   = alt;
		wm.meta  = meta;
//		handler.onevent(wm);
		if (!caught) handler(wm);
	}
}
void parsetouchscreen(struct input_event e) {
	printf("parsetouchscreen\n");
}
void parsetouchpad(struct masdar *m, struct input_event e, void (*handler)()) {
	if (e.type == EV_ABS) {
		switch (e.code) {
			case ABS_X:
				touchpadx = e.value;
				break;
			case ABS_Y:
				touchpady = e.value;
				break;
			case ABS_PRESSURE:
			case ABS_MT_PRESSURE:
				touchpadw = e.value;
		}
	} else if (e.type == EV_KEY) {
		switch (e.code) {
			case BTN_TOUCH:
				touchpadfingers = e.value ? 1 : 0;
				break;
			case BTN_TOOL_DOUBLETAP:
				touchpadfingers = e.value ? 2 : 1;
				break;
			case BTN_TOOL_TRIPLETAP:
				touchpadfingers = e.value ? 3 : 2;
				break;
			case BTN_TOOL_QUADTAP:
				touchpadfingers = e.value ? 4 : 3;
				break;
			case BTN_TOOL_QUINTTAP:
				touchpadfingers = e.value ? 5 : 4;
				break;
		}
	} else if (e.type == EV_SYN) {
		mafateeh wm;
		wm.type  = TOUCHPAD;
		wm.x     = touchpadx / (float) m->x.maximum;
		wm.y     = touchpady / (float) m->y.maximum;
		wm.w     = touchpadw / 255.0f;
		wm.key   = touchpadfingers;
		XATAA > 1 && printf("X%f Y%f W%f K%d\n", wm.x, wm.y, wm.w, wm.key);
		handler(wm);
	}
}
void parsemouse(struct input_event e) {
//	printf("parsemouse\n");
	if (e.type == EV_SYN) {
	} else
	if (e.type == EV_KEY) {
	} else
	if (e.type == EV_REL) {
		mafateeh wm;
		wm.type = MOUSE;
		switch (e.code) {
			case REL_X:
				mousex = e.value;
				break;
			case REL_Y:
				mousey = e.value;
				break;
			case REL_WHEEL:
				mousew = e.value;
				break;
		}
		wm.x   = mousex;
		wm.y   = mousey;
		wm.w   = mousew;
		printf("X%f Y%f W%d\n", wm.x, wm.y, wm.meta);
	}
}
void madxal_destory() {
	destruction = 1;
}
void onevent(mafateeh wm) {
//	printf("onevent %ld: C%d S%d M%d A%d K%d S%d\n", sizeof wm, wm.ctrl, wm.shift, wm.meta,
//				wm.alt, wm.key, wm.state);
	
	unsigned char *str = mafateeh2str(wm);
//	printf("%s %d %d\n", str, wm.huroof[0], wm.huroof[1]);
	if (wm.type == KEYBOARD) {
		amr_irsal_str(MADXAL, LOWHAH, str);
	}
	if (wm.type == TOUCHPAD) {
		amr_irsal_str(MADXAL, LAMSAH, str);
	}
	free(str);
}
int handler(waaqi3ah *w, int fd) {
	if (w) {
	} else {
		struct input_event e;
		int ret;
		struct masdar *m = fdtomasdar[fd];
		while (1) {
			ret = read(fd, &e, sizeof e);
			if (ret != sizeof e) break;
			if (m->type == TOUCHPAD) { parsetouchpad(m, e, &onevent); }
//			if (m->type == MOUSE) { parsemouse(e); }
//			if (m->type == TOUCHSCREEN) { parsetouchscreen(e); }
			if (m->type == KEYBOARD) { parsekbevent(e, &onevent); }
		}
	}
}
int main (int argc, int **argv) {
	/*if (argc == 2) {
		printf("%d\n", str2miftaah( (const char *) argv[1] ) );
	}
	for (int i = 0; i < sizeof _$la / sizeof _$la[0]; ++i) {
		if (_$la[i].i) {
			printf("la %d %s\n", i, _$la[i].huroof);
		}
	}*/
	/*for (int i = 0; i < sizeof _$alt / sizeof _$alt[0]; ++i) {
		if (_$alt[i].i) {
			printf("alt %d %s\n", i, _$alt[i].huroof);
		}
	}

	return 0;*/
//	for (int i = 0; i < sizeof _$arshift / sizeof _$arshift[0]; ++i) {
//		if (_$arshift[i].i) {
//			printf("arshift %d %s\n", i, _$arshift[i].huroof);
//		}
//	}

	madxal_fak("./data/fusha.mafateeh");
	
	masdar *masaadir;
	int s, kul, threadcount = 0;
	kul = masaadir_init();
	XATAA > 1 && printf(" kul masaadir %d\n", kul);
	masaadir = masaadir_get();
	for (int i = 0; i < kul; ++i) {
		masdar *m = &masaadir[i];
		// except for ALSA masaadir
		if ( strcmp(m->location, "ALSA") ) {
			u_char *type_debug = "";
			if (m->type == KEYBOARD) type_debug = "keyboard";
			if (m->type == MOUSE) type_debug = "mouse";
			if (m->type == TOUCHSCREEN) type_debug = "touchscreen";
			if (m->type == TOUCHPAD) type_debug = "touchpad";

			if (m->type == KEYBOARD || m->type == MOUSE || m->type == TOUCHSCREEN || m->type == TOUCHPAD) {
				m->fd = open(m->path, O_RDONLY | O_NOCTTY | O_NONBLOCK);
				XATAA && printf("adding %s (%d) %s [%dx%d]\n", m->name, m->fd, type_debug, m->x.maximum, m->y.maximum);
				fdtomasdar[m->fd] = m;
			}
		}
	}
	amr_towheed("madxal");
	for (int i = 0; i < kul; ++i) {
		struct masdar m = masaadir[i];
		if (m.type == KEYBOARD || m.type == MOUSE || m.type == TOUCHSCREEN || m.type == TOUCHPAD) {
			XATAA > 1 && printf("------ %s (%d)\n", m.name, m.fd);
			if (m.fd) amr_intazar( m.fd );
		}
	}
	amr_axath(&handler);

	return EXIT_SUCCESS;
}