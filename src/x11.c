/*
 * this is an x11 maxraj, it opens an x11 window, registers a dhikr 'maxraj'
 * 
 * all waaqi3aat it emits are MAXRAJ.*.*
 * 
 * whenever it detects a
 * MUDEER.BADAA			// waajihah has accepted an amr to act as a mudeer
 * then it transmits dimensions... using
 * MAXRAJ.MST3D.0		// not ready
 * MAXRAJ.MKAAN[X,Y]	// where x11 window is, helps madxal translate waaqi3aat
 * MAXRAJ.HAJM[H,W]		// 
 * MAXRAJ.MST3D.1		// ready to receive rendering requests
 * 
 * it handles a waaqi3ah
 * MUDEER.MST3D.1		// mudeer says it's ready for the first frame
 * then it starts the heartbeat by emitting
 * 
 * all waaqi3aat it emits are MAXRAJ.*.*
 * MAXRAJ.INSHA			// for mudeer
 * 
 * it expects to receive a waaqi3ah
 * NAQAF.MAXRAJ				// to flip/render the framebarzax to screen/server
 * */

#include "msfoof.h"
#include "alwaan.h"

#include "amr.h"
#include "matn.h"
#include "helper.h"
#include "xat.h"

#include <linux/input-event-codes.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#define WIDTH		240*2.5
#define HEIGHT		320*2.5
#define XATAA 		0

// duplicated from masaadir.h :((
enum {
	UNKNOWN		= 0x00,
	KEYBOARD	= 0x01,
	MOUSE		= 0x02,
	TOUCHPAD	= 0x03,
	TOUCHSCREEN	= 0x04,
	PEN			= 0x05,
};

Display *dis; int screen; Window win; GC gc; Pixmap pm; XImage *img;
XShmSegmentInfo shminfo;
XSetWindowAttributes attr; XVisualInfo vinfo; GC NormalGC;
int ox = 0, oy = 0, width = WIDTH, height = HEIGHT,
	mspf = 20*1000, // ms per frame
	mousedown = 0,
	damaged = 1, markooz = 0, zaahir = 0, heartbeat = 1;
ihaatah sgeo = { 0 }; // screen geometry
ihaatah wgeo = { 0, 0, WIDTH, HEIGHT }; // window geometry
nuqtah pointer;
nuqtah ovgeo; // overlay
msfoof mtrx, overlay;
struct timespec waqt;
long eventmask = ExposureMask | ButtonPressMask | ButtonReleaseMask |
				 LeaveWindowMask |
				 /*KeyPressMask | KeyReleaseMask | */FocusChangeMask |
				 PointerMotionMask | StructureNotifyMask;
int size = WIDTH*HEIGHT*sizeof(int);
void *addr;
char *appname = "nizaam-x11";

long beat() { // returns 1 if has beaten, 0 if waiting for next beat
	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	long farq = sec2ms(aan.tv_sec-waqt.tv_sec) + nano2ms(aan.tv_nsec-waqt.tv_nsec);
//	printf("beat %ld\n", farq);
//	if (farq > mspf) {
//		waqt.tv_sec = aan.tv_sec;
//		waqt.tv_nsec = aan.tv_nsec;
//	}
	return farq;
}
void damage() {
	damaged = 1;
}
void drawoverlay() {
	memcpy(mtrx.barzax, addr, size);
//	memcpy(overlay.barzax, addr, size);
//	ihaatah tg = { ovgeo.x, ovgeo.y, overlay.w, overlay.h };
//	msfoof_clear_area(&mtrx, 0, &tg);
//	msfoof_composite(&overlay, &mtrx, &tg);
//	msfoof_composite(&overlay, &mtrx, 0);
}
void redraw() {
//	int x = wgeo.x, y = wgeo.y, cx = 0, cy = 0;
//	if (x < 0) { cx = -x; }
//	if (y < 0) { cy = -y; }
//	if (x > wgeo.w) { cx = wgeo.w; }
//	if (y > wgeo.h) { cy = wgeo.h; }
//	XPutImage(dis, win, NormalGC, img, x, y, cx, cy, wgeo.w, wgeo.h);
	XShmPutImage(dis, win, NormalGC, img, 0, 0, ox, oy, WIDTH, HEIGHT, 0);
	XSync(dis, True);
	damaged = 0;
//	if ( clock_gettime(CLOCK_REALTIME, &waqt) ) {
//		printf("clock_gettime failed! %s\n", strerror(errno));
//	}
}
void resize(int x, int y, int w, int h) {
	if (wgeo.x != x || wgeo.y != y || wgeo.w != w || wgeo.h != h) {
		ox = (w - WIDTH)/2.0;
		oy = (h - HEIGHT)/2.0;
		wgeo.x = x + (ox > 0 ? ox : 0);
		wgeo.y = y + (oy > 0 ? oy : 0);
		if (MUDEERUID > 0) {
			amr_irsal_ilaa(MUDEERUID, MAXRAJ, MKAANX, wgeo.x);
			amr_irsal_ilaa(MUDEERUID, MAXRAJ, MKAANY, wgeo.y);
		}
//		wgeo.w = w;
//		wgeo.h = h;
//		printf("resize %d %d %d %d\n", wgeo.x, wgeo.y, wgeo.w, wgeo.h);
//		drawumoor();
//		drawoverlay();
	}
}
void render() {
//	nabad_waqfah(1);
//	long farq = beat(); // convert these terms to lastrender or smth
//	while (damaged && farq >= mspf) {
//		printf("rendered %ld\n", farq);
		if (damaged) redraw();
//		break;
//	}
//	nabad_waqfah(0);
}
void close_x() {
//	XFreeColormap(dis, cmap);
//	XFreePixmap(dis, pm);
//	XDestroyImage(img);
	XFreeGC(dis, gc);
	XDestroyWindow(dis,win);
//	XCloseDisplay(dis); // really!?
	exit(EXIT_SUCCESS);
}
void setprop(char *p, char *v) {
	Atom prop, val;
	prop = XInternAtom(dis, p, 0);
	val = XInternAtom(dis, v, 0);
	XChangeProperty(dis, win, prop, XA_ATOM, 32, PropModeReplace,
					(unsigned char *) &val, 1);
}
void init_x() {
	dis = XOpenDisplay((char *) 0);
   	screen = DefaultScreen(dis);
	Screen *s = ScreenOfDisplay(dis, screen);
//	sgeo.w = WidthOfScreen(s);
//	sgeo.h = HeightOfScreen(s);
	sgeo.w = wgeo.w;
	sgeo.h = wgeo.h;
//	wgeo.x = sgeo.w/2-(width/2);
//	wgeo.y = sgeo.h/2-(height/2);
//	wgeo.w = sgeo.w;
//	wgeo.h = sgeo.h;
	XATAA && printf("sgeo: %d %d\n", sgeo.w, sgeo.h);

	XMatchVisualInfo(dis, DefaultScreen(dis), 24, TrueColor, &vinfo);
    attr.colormap = XCreateColormap(dis, DefaultRootWindow(dis), vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0;

    XSync(dis, True);

    win = XCreateWindow(dis, DefaultRootWindow(dis), wgeo.x, wgeo.y, WIDTH, HEIGHT, 0,
			vinfo.depth, InputOutput, vinfo.visual,
			CWColormap|CWBorderPixel|CWBackPixel, &attr);

	XSizeHints *sh = XAllocSizeHints();
	sh->flags = PMinSize
//	| PMaxSize
	;
	sh->min_width =
//	sh->max_width =
	WIDTH;
	sh->min_height =
//	sh->max_height =
	HEIGHT;
//	XSetWMNormalHints(dis, win, sh);
//	XSetWMSizeHints(d, w, sh, PMinSize | PMaxSize);

	// title, minimized title
	XSetStandardProperties(dis, win, appname, 0, None, NULL, 0, sh);

	XFree(sh);

	XSelectInput(dis, win, eventmask);
    gc = XCreateGC(dis, win, 0, 0);

	XGCValues gcv;
	gcv.graphics_exposures = 0;
	unsigned long gcm = GCGraphicsExposures;
	NormalGC = XCreateGC(dis, DefaultRootWindow(dis), gcm, &gcv);

//	setprop("_NET_WM_ICON_NAME", "blender");
//	setprop("_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_DESKTOP");
//	setprop("_NET_WM_STATE", "_NET_WM_STATE_STICKY");
//	setprop("_NET_WM_STATE", "_NET_WM_STATE_BELOW");

	// this helps identify the window in task managers ...
	int _NET_WM_PID = XInternAtom(dis, "_NET_WM_PID", False), pid = getpid();
	XChangeProperty(dis, win, _NET_WM_PID, XA_CARDINAL, 32, PropModeReplace, (u_char *)&pid, 1);

	XClassHint *class_hint = XAllocClassHint();
	if (class_hint) {
		class_hint->res_name = class_hint->res_class = appname;
		XSetClassHint(dis, win, class_hint);
		XFree(class_hint);
	}

	mtrx = msfoof_new(WIDTH, HEIGHT, NULL);

//	img = XCreateImage(dis, vinfo.visual, 24, ZPixmap, 0, (char *)mtrx.barzax, sgeo.w, sgeo.h, 8, sgeo.w*4);

	img = XShmCreateImage(dis, vinfo.visual, 24, ZPixmap, NULL, &shminfo, WIDTH, HEIGHT); 
	shminfo.shmid = shmget(IPC_PRIVATE, img->bytes_per_line * img->height, IPC_CREAT|0777);
	shminfo.shmaddr = img->data = shmat(shminfo.shmid, 0, 0);
	shminfo.readOnly = False;
	XShmAttach(dis, &shminfo);
	XShmGetImage(dis, RootWindow(dis,0), img, 0, 0, AllPlanes);
    if (img == 0) { printf("img is null!\n"); }
	msfoof_set_barzax(&mtrx, (int *) img->data);

	msfoof_clear(&mtrx, 0);

	XClearWindow(dis, win);
	XMapWindow(dis, win);

    XSync(dis, True);
}

int mousebutton = 0;
void mousehandler(XEvent e) {
	if (MUDEERUID > 0) {
		mafateeh wm = { 0 };
		wm.type = MOUSE;
		if (e.type == ButtonPress) {
			if (e.xbutton.button == Button1)
				mousebutton = 1;
			if (e.xbutton.button == Button2) // middle click
				mousebutton = 3; // because 3 fingers on touchpads, modern times
			if (e.xbutton.button == Button3) // right click
				mousebutton = 2; // see above
			if (e.xbutton.button == Button4) // scroll up
				wm.w = -1;
			if (e.xbutton.button == Button5) // scroll down
				wm.w = 1;
			wm.x     = e.xbutton.x;
			wm.y     = e.xbutton.y;
			mousedown = -1;
		}
		if (e.type == LeaveNotify) {
			wm.x     = e.xcrossing.x;
			wm.y     = e.xcrossing.y;
			mousedown = 1;
		}
		if (e.type == ButtonRelease) {
			wm.x     = e.xbutton.x;
			wm.y     = e.xbutton.y;
			mousedown = 1;
		}
		if (e.type == MotionNotify) {
			wm.x     = e.xmotion.x;
			wm.y     = e.xmotion.y;
			if (mousedown) {
				mousedown = 0;
			}
		}

		wm.key = mousebutton;
		if (mousedown == 1) mousebutton = 0;
		
		wm.state   = mousedown;

		wm.x -= ox;
		wm.y -= oy;

		if (wm.x <      0) wm.x =      0;
		if (wm.y <      0) wm.y =      0;
		if (wm.x > wgeo.w) wm.x = wgeo.w;
		if (wm.y > wgeo.h) wm.y = wgeo.h;

		unsigned char *str = mafateeh2str(wm);
		amr_irsal_ilaa_str(MUDEERUID, MADXAL, ISHAARAH, str);
		free(str);
	}
}
void x11processevents() {
	XEvent e;
	int yes = 0, total = XPending(dis);
	if (total > -1) {
		XATAA && printf("total events %d\n", total);
		for (int i = 0; i < total; ++i) {
			XNextEvent(dis, &e);
//			XCheckWindowEvent(dis, win, eventmask, &e);
		
			switch (e.type) {
                case ButtonPress:
                case ButtonRelease:
//				case EnterNotify:
//				case LeaveNotify:
                case MotionNotify:
                    mousehandler(e);
                    break;
			}
			if (e.type == FocusIn) {
				XATAA && printf("focus in\n");
				markooz = 1; yes = 1;
				if (MUDEERUID > 0) amr_irsal_ilaa(MUDEERUID, MAXRAJ, RAKKAZ, 1);
			} else
			if (e.type == FocusOut) {
				XATAA && printf("focus out\n");
				markooz = 0; yes = 1;
				if (MUDEERUID > 0) amr_irsal_ilaa(MUDEERUID, MAXRAJ, RAKKAZ, 0);
			} else
			if (e.type == UnmapNotify) {
				XATAA && printf("unmap\n");
				zaahir = 0;
				if (MUDEERUID > 0) amr_irsal_ilaa(MUDEERUID, MAXRAJ, IZHAR, 0);
			} else
			if (e.type == MapNotify) {
				XATAA && printf("map\n");
				zaahir = 1;
				if (MUDEERUID > 0) {
					amr_irsal_ilaa(MUDEERUID, MAXRAJ, IZHAR, 1);
					amr_irsal_ilaa(MUDEERUID, MAXRAJ, INSHA, 1);
				}
			} else
			if (e.type == ReparentNotify) {
				XATAA && printf("reparentnotify\n");
			} else
			if (e.type == ConfigureNotify) {
				XATAA && printf("configurenotify\n");
//				if (e.xconfigurerequest.send_event == 1) {
					resize(
						e.xconfigurerequest.x, e.xconfigurerequest.y,
						e.xconfigurerequest.width, e.xconfigurerequest.height
					);
					yes = 1;
//				}
			} else
			if (e.type == Expose && e.xexpose.count == 0) {
				// the == 0 ensures that we only get the last expose event
				XATAA && printf("expose\n");
				yes = 1;
			} else {
//				printf("%d\n", e.type);
			}
			
			if (yes) damage();
		}
	}
}
void announcehajm() {
	if (MUDEERUID > 0) {
		XATAA > 1 && printf("announcehajm\n");
		amr_irsal_ilaa(MUDEERUID, MAXRAJ, RAKKAZ, markooz);
		amr_irsal_ilaa(MUDEERUID, MAXRAJ, IZHAR, zaahir);
		amr_irsal_ilaa(MUDEERUID, MAXRAJ, MST3D, 0); // pending, hint mudeer to wait until maxraj is ready
		amr_irsal_ilaa(MUDEERUID, MAXRAJ, MKAANX, wgeo.x);
		amr_irsal_ilaa(MUDEERUID, MAXRAJ, MKAANY, wgeo.y);
		amr_irsal_ilaa(MUDEERUID, MAXRAJ, HAJMW, WIDTH);
		amr_irsal_ilaa(MUDEERUID, MAXRAJ, HAJMH, HEIGHT);
		amr_irsal_ilaa(MUDEERUID, MAXRAJ, MST3D, 1); // ready, hint mudeer to init mem + msfoof
	}
}
void setpointerat(waaqi3ah *w) {
	ihaatah ihth = { 0 };
	str2ihaatah(w->qadrstr, &ihth);

	if (ihth.x >= WIDTH)	ihth.x = WIDTH-1;
	if (ihth.y >= HEIGHT)	ihth.y = HEIGHT-1;

	XWarpPointer(dis, None, DefaultRootWindow(dis), 0, 0, 0, 0, wgeo.x+ihth.x, wgeo.y+ihth.y);
	XATAA > 1 && printf("setpointerat %d, %d\n", ihth.x, ihth.y);
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA > 1) amr_tb3_waaqi3ah(w);
		if (w->ism == ISHAARAH && w->miftaah == MAXRAJ) setpointerat(w);
		if (w->ism == MUDEER && w->miftaah == BADAA) announcehajm();
		if (w->ism == DARAR && w->miftaah == MUDEER) damage();
		if (w->ism == NAQAF && w->miftaah == MAXRAJ) {
			if (damaged && zaahir) {
				drawoverlay();
				redraw();
			}
//			usleep(mspf);
			if (MUDEERUID > 0 && zaahir)
				amr_irsal_ilaa(MUDEERUID, MAXRAJ, INSHA, 1);
		}
		if (MUDEERUID > 0 && w->ism == MUDEER && w->miftaah == MST3D)
			amr_irsal_ilaa(MUDEERUID, MAXRAJ, INSHA, 1);
	} else {
		x11processevents();
	}
	return 0;
}
int main(int argc, char *argv[]) {
	tarmeez_core();
	ovgeo = (nuqtah) { wgeo.x, wgeo.y };
	srand(waqt.tv_nsec);
	overlay = msfoof_new(WIDTH, HEIGHT, NULL);

	int i = 0, fd;
	int data[size];

	amr_dhikr("maxraj", size, &fd);
	amr_dhikr_axath(fd, size, &addr);
	
	amr_towheed("maxraj");

	amr_irsal(HAAJA, NAQAF, 1); // flip
	amr_irsal(HAAJA, DARAR, 1); // damage
	amr_irsal(HAAJA, ISHAARAH, 1); // set pointer @
	
	init_x();
	mtrx.strokew = 2;
	
	amr_intazar( XConnectionNumber(dis) );

	amr_axath(&handler);

    exit(EXIT_SUCCESS);
}
