#include "msfoof.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

msfoof mtrx;
int width = 1000, height = 700;
Display *dis;
XVisualInfo vinfo;
int depth;
XImage *img;
XShmSegmentInfo shminfo;
Window parent;

XGCValues gcv;
unsigned long gcm;
GC NormalGC;

void drawgrad(int i, int j) {
	void drawcolor(msfoof *m, int x, int y, long color) {
		msfoof_set( m, x, y, 0xff00ff00+(x*y/90)-(i) );
	}
	msfoof_each(&mtrx, &drawcolor, NULL);
}
void resize() {
	if (!img) {
		img = XShmCreateImage(dis, vinfo.visual, vinfo.depth, ZPixmap, NULL, &shminfo, width, height); 

		shminfo.shmid = shmget(IPC_PRIVATE, img->bytes_per_line * img->height, IPC_CREAT|0777);

		shminfo.shmaddr = img->data = shmat(shminfo.shmid, 0, 0);
		shminfo.readOnly = False;

		XShmAttach(dis, &shminfo);

		XShmGetImage(dis, RootWindow(dis,0), img, 0, 0, AllPlanes);
	}

    if (img == NULL) { printf("img is null!\n"); }

	mtrx.barzax = NULL; // not doing this results in segfault
	msfoof_radd_barzax_alif(&mtrx);

	mtrx = msfoof_new(width, height, NULL);
	msfoof_set_barzax(&mtrx, (int *) img->data);
	msfoof_radd_barzax_alif(&mtrx);
//	msfoof_clear_alif(&mtrx, 0);
    msfoof_clear(&mtrx, 0);

    //gcm = GCForeground | GCBackground | GCGraphicsExposures;
    //gcv.foreground = BlackPixel(dis, parent);
    //gcv.background = WhitePixel(dis, parent);
    gcm = GCGraphicsExposures;
    gcv.graphics_exposures = 0;
    NormalGC = XCreateGC(dis, parent, gcm, &gcv);

}
int main(int argc, char **argv) {
    XVisualInfo *visual_list;
    XVisualInfo visual_template;
    int nxvisuals;
    int i;
    XSetWindowAttributes attrs;
    Visual *visual;
    Window win;
    int *framebuf;
    XEvent e;

    dis = XOpenDisplay(NULL);

    nxvisuals = 0;
    visual_template.screen = DefaultScreen(dis);
    visual_list = XGetVisualInfo (dis, VisualScreenMask, &visual_template, &nxvisuals);

    //Change to this line
//    if (!XMatchVisualInfo(dis, XDefaultScreen(dis), 32, TrueColor, &vinfo))
    if (!XMatchVisualInfo(dis, XDefaultScreen(dis), 24, TrueColor, &vinfo))
    {
        fprintf(stderr, "no such visual\n");
        return 1;
    }

    parent = XDefaultRootWindow(dis);

    XSync(dis, True);

    printf("creating RGBA child\n");

    visual = vinfo.visual;
    depth = vinfo.depth;

    attrs.colormap = XCreateColormap(dis, XDefaultRootWindow(dis), visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;
    
	mtrx = msfoof_new(width, height, NULL);
	framebuf = mtrx.barzax;

    win = XCreateWindow(dis, parent, 100, 100, width, height, 0, vinfo.depth, InputOutput,
                        visual, CWBackPixel | CWColormap | CWBorderPixel, &attrs);

    //Change to this line
//    img = XCreateImage(dis, vinfo.visual, 32, XYPixmap, 0, (char *)framebuf, width, height, 8, width*4);
//    img = XCreateImage(dis, vinfo.visual, vinfo.depth, ZPixmap, 0, (char *)framebuf, width, height, 8, width*4);
	img = XShmCreateImage(dis, vinfo.visual, 24, ZPixmap, NULL, &shminfo, width, height); 
	shminfo.shmid = shmget(IPC_PRIVATE, img->bytes_per_line * img->height, IPC_CREAT|0777);
	shminfo.shmaddr = img->data = shmat(shminfo.shmid, 0, 0);
	shminfo.readOnly = False;
	XShmAttach(dis, &shminfo);
	XShmGetImage(dis, RootWindow(dis,0), img, 0, 0, AllPlanes);
//	resize();

    XSync(dis, True);

    XSelectInput(dis, win, ExposureMask | KeyPressMask | PointerMotionMask | StructureNotifyMask);

    XMapWindow(dis, win);

    while(!XNextEvent(dis, &e))
    {
		if (e.type == ConfigureNotify) {
			if (e.xconfigurerequest.send_event == 1) {
//				printf("resize!\n");
				width = e.xconfigurerequest.width;
				height = e.xconfigurerequest.height;
				resize();
			}
		} else
		if (e.type == MotionNotify) {
			int x = e.xbutton.x, y = e.xbutton.y;
			msfoof_clear(&mtrx, 0);
			mtrx.strokew = 9;
			float pct = (float) x / width;
			float pct2 = (float) y / height;
//			matrix_circle(&mtrx, width/2, height/2, 240*pct2, 0, 390.0*pct, 0x00ffff, 0x009900, 1);
			msfoof_mustateel(&mtrx, 0, 0, width*pct, height*pct2, 0x00ffff, 0x009900, 1);
//			msfoof_rect(&mtrx, x, y, 90, 90, 0xffff0000, 0xff990000);
//			printf("motion!\n");
//			drawgrad(x, y);
//			XPutImage(dis, win, NormalGC, img, 0, 0, 0, 0, width, height);
			XShmPutImage(dis, win, NormalGC, img, 0, 0, 0, 0, width, height, 0);
			XSync(dis, True);
		}
        switch(e.type) {
			case Expose:
//				printf("I have been exposed!\n");
//				msfoof_rect(&mtrx, 0, 0, 90, 90, 0xffff0000, 0xff990000);
//				XPutImage(dis, win, NormalGC, img, 0, 0, 0, 0, width, height);
				XShmPutImage(dis, win, NormalGC, img, 0, 0, 0, 0, width, height, 0);
				XSync(dis, True);
				break;
		}
    }

    printf("No error\n");

    return 0;
}
