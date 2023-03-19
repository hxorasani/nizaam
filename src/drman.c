#include "drman.h"

#define XATAA 1

static struct drman_dev *drman_list = NULL;
char *dri_path = "/dev/dri/card0";

int fps = 50 * 1000; //  20fps
int mspf = 400; //  20fps
enum {
	DEPTH = 24,
	BPP = 32,
};
struct drman_dev *dev_head;
msfoof mtrx;
msfoof overlay;
matn mtn;
int active = 0;
long frames = 0;
long framespersec = 0;
long totaltime = 0;
struct timespec waqt;
long timelimit = 0;
struct timespec waqtlimit;
int detectedfps = 0;
float t = 0, pct = 0, f = 0;

void drman_release();
void drman_acquire();

long beat(struct timespec w) { // returns ms since last frame
	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	return sec2ms(aan.tv_sec-w.tv_sec) + nano2ms(aan.tv_nsec-w.tv_nsec);
}
uint8_t next_color(bool *up, uint8_t cur, unsigned int mod) {
	uint8_t next;

	next = cur + (*up ? 1 : -1) * (rand() % mod);
	if ((*up && next < cur) || (!*up && next > cur)) {
		*up = !*up;
		next = cur;
	}

	return next;
}
void drman_draw_dev(struct drman_dev *dev) {
	if (!active) return;
	
	struct drman_buf *buf;
	unsigned int j, k, off;
	int ret;

	dev->r = next_color(&dev->r_up, dev->r, 20);
	dev->g = next_color(&dev->g_up, dev->g, 10);
	dev->b = next_color(&dev->b_up, dev->b, 5);

	buf = &dev->bufs[dev->front_buf ^ 1];
	drm.mtrx.barzax = (int *) buf->map;

	int h = drm.mtrx.h, color = (dev->r << 16) | (dev->g << 8) | dev->b;
//	msfoof_rect(&drm.mtrx, 0, 0, drm.mtrx.w*.25, h, -1, color - 100);
//	msfoof_rect(&drm.mtrx, drm.mtrx.w*.25, 0, drm.mtrx.w*.5, h, -1, color);
//	msfoof_rect(&drm.mtrx, drm.mtrx.w*.75, 0, drm.mtrx.w*.25, h, -1, color + 100);
//	msfoof_rect(&overlay, 0, 0, overlay.w, overlay.h, -1, color);
//	msfoof_rect(&overlay, 0, 0, overlay.w, overlay.h, -1, 0);

	ihaatah tg = { 0, drm.mtrx.h-overlay.h, overlay.w, overlay.h };

	msfoof_rect(&drm.mtrx, 0, 0, tg.w, tg.h, -1, 0);
	msfoof_rect(&drm.mtrx, 0, tg.y, tg.w, tg.h, -1, 0);

	long farq = beat(waqt);
	timelimit = beat(waqtlimit);
	totaltime += farq;
	framespersec += 1;

	int pb = 2, pb4 = 8;
	msfoof_rect(&drm.mtrx, 0, 0, frames, pb, -1, 0xffffff);
	msfoof_rect(&drm.mtrx, 0, pb, frames, pb, -1, 0x999999);

	msfoof_rect(&drm.mtrx, 0, drm.mtrx.h-pb4+pb, ms2sec(timelimit)*pb4, pb, -1, 0xffffff);
	msfoof_rect(&drm.mtrx, 0, drm.mtrx.h-pb4, ms2sec(timelimit)*pb4, pb, -1, 0x999999);

	if (totaltime > 1000) detectedfps = framespersec;

	msfoof_rect(&overlay, 0, 0, overlay.w, overlay.h, -1, 0);
	mtn.mlf.barzax = (unsigned char *) helper_stringify(" %ld fps, %ld secs", detectedfps, ms2sec(timelimit));
	matn_draw(mtn);
	msfoof_composite(&overlay, &drm.mtrx, &tg);

	if (totaltime > 1000) {
		totaltime = 0;
		framespersec = 0;
	}

	msfoof_rect(&overlay, 0, 0, overlay.w, overlay.h, -1, 0);
	mtn.mlf.barzax = (unsigned char *) helper_stringify(" %ld", frames);
	tg.y = 0;
	matn_draw(mtn);
	msfoof_composite(&overlay, &drm.mtrx, &tg);

	frames++;

	if (active) {
		ret = drmModePageFlip(drm.fd, dev->crtc, buf->fb, DRM_MODE_PAGE_FLIP_EVENT, dev);
		if (ret) {
			printf("cannot flip CRTC for connector %u (%d): %m\n", dev->conn, errno);
		} else {
			dev->front_buf ^= 1;
			dev->pflip_pending = true;
		}
	}

	if ( clock_gettime(CLOCK_REALTIME, &waqt) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
}
static void drman_page_flip_event(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data) {
	struct drman_dev *dev = data;

	dev->pflip_pending = false;
	if (!dev->cleanup)
		drman_draw_dev(dev);
}
void drman_draw() {
//	XATAA && printf("drman_draw\n");
	int ret;
	fd_set fds;
	time_t start, cur;
	struct timeval v;
	drmEventContext ev;
	struct drman_dev *iter;

	/* init variables */
	srand(time(&start));
	FD_ZERO(&fds);
	memset(&v, 0, sizeof(v));
	memset(&ev, 0, sizeof(ev));
	/* Set this to only the latest version you support. Version 2
	 * introduced the page_flip_handler, so we use that. */
	ev.version = 2;
	ev.page_flip_handler = drman_page_flip_event;

	/* redraw all outputs */
	for (iter = drman_list; iter; iter = iter->next) {
		iter->r = rand() % 0xff;
		iter->g = rand() % 0xff;
		iter->b = rand() % 0xff;
		iter->r_up = iter->g_up = iter->b_up = true;

		drman_draw_dev(iter);
	}
	
	int timeleft = 30;

	/* wait 5s for VBLANK or input events */
	while (time(&cur) < start + timeleft) {
		if (active) {
			FD_SET(0, &fds);
			FD_SET(drm.fd, &fds);
			v.tv_sec = start + timeleft - cur;

			ret = select(drm.fd + 1, &fds, NULL, NULL, &v);
			if (ret < 0) {
				fprintf(stderr, "select() failed with %d: %m\n", errno);
				drman_release();
				break;
			} else if (FD_ISSET(0, &fds)) {
				fprintf(stderr, "exit due to user-input\n");
				drman_release();
				break;
			} else if (FD_ISSET(drm.fd, &fds)) {
				drmHandleEvent(drm.fd, &ev);
			}
		} else {
			printf("secs %ld\n", start + timeleft - cur);
			usleep(500 * 1000);
			teletype_acq_rel();
		}
	}
}
int drman_open(char *node) {
	XATAA && printf("open drman\n");
	int ret; uint64_t has_dumb;

	drm.fd = open(node, O_RDWR | O_CLOEXEC);
	if (drm.fd < 0) {
		ret = -errno;
		fprintf(stderr, "cannot open '%s': %m\n", node);
		return ret;
	}

	if (drmGetCap(drm.fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
		printf("'%s' does not support dumb barzaxs\n", node);
		close(drm.fd);
		return -EOPNOTSUPP;
	}

	return 0;
}
int drman_find_crtc(drmModeRes *res, drmModeConnector *conn, struct drman_dev *dev) {
	drmModeEncoder *enc;
	unsigned int i, j;
	int32_t crtc;
	struct drman_dev *iter;

	/* first try the currently conected encoder+crtc */
	if (conn->encoder_id)
		enc = drmModeGetEncoder(drm.fd, conn->encoder_id);
	else
		enc = NULL;

	if (enc) {
		if (enc->crtc_id) {
			crtc = enc->crtc_id;
			for (iter = drman_list; iter; iter = iter->next) {
				if (iter->crtc == crtc) {
					crtc = -1;
					break;
				}
			}

			if (crtc >= 0) {
				drmModeFreeEncoder(enc);
				dev->crtc = crtc;
				return 0;
			}
		}

		drmModeFreeEncoder(enc);
	}

	/* If the connector is not currently bound to an encoder or if the
	 * encoder+crtc is already used by another connector (actually unlikely
	 * but lets be safe), iterate all other available encoders to find a
	 * matching CRTC. */
	for (i = 0; i < conn->count_encoders; ++i) {
		enc = drmModeGetEncoder(drm.fd, conn->encoders[i]);
		if (!enc) {
			fprintf(stderr, "cannot retrieve encoder %u:%u (%d): %m\n",
				i, conn->encoders[i], errno);
			continue;
		}

		/* iterate all global CRTCs */
		for (j = 0; j < res->count_crtcs; ++j) {
			/* check whether this CRTC works with the encoder */
			if (!(enc->possible_crtcs & (1 << j)))
				continue;

			/* check that no other device already uses this CRTC */
			crtc = res->crtcs[j];
			for (iter = drman_list; iter; iter = iter->next) {
				if (iter->crtc == crtc) {
					crtc = -1;
					break;
				}
			}

			/* we have found a CRTC, so save it and return */
			if (crtc >= 0) {
				drmModeFreeEncoder(enc);
				dev->crtc = crtc;
				return 0;
			}
		}

		drmModeFreeEncoder(enc);
	}

	fprintf(stderr, "cannot find suitable CRTC for connector %u\n",
		conn->connector_id);
	return -ENOENT;
}
int drman_create_fb(struct drman_buf *buf) {
	struct drm_mode_create_dumb creq;
	struct drm_mode_destroy_dumb dreq;
	struct drm_mode_map_dumb mreq;
	int ret;

	/* create dumb barzax */
	memset(&creq, 0, sizeof(creq));
	creq.width = buf->width;
	creq.height = buf->height;
	creq.bpp = 32;
	ret = drmIoctl(drm.fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0) {
		fprintf(stderr, "cannot create dumb barzax (%d): %m\n",
			errno);
		return -errno;
	}
	buf->stride = creq.pitch;
	buf->size = creq.size;
	buf->handle = creq.handle;

	/* create framebarzax object for the dumb-barzax */
	ret = drmModeAddFB(drm.fd, buf->width, buf->height, 24, 32, buf->stride,
			   buf->handle, &buf->fb);
	if (ret) {
		fprintf(stderr, "cannot create framebarzax (%d): %m\n",
			errno);
		ret = -errno;
		goto err_destroy;
	}

	/* prepare barzax for memory mapping */
	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = buf->handle;
	ret = drmIoctl(drm.fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret) {
		fprintf(stderr, "cannot map dumb barzax (%d): %m\n",
			errno);
		ret = -errno;
		goto err_fb;
	}

	/* perform actual memory mapping */
	buf->map = mmap(0, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED,
		        drm.fd, mreq.offset);
	if (buf->map == MAP_FAILED) {
		fprintf(stderr, "cannot mmap dumb barzax (%d): %m\n",
			errno);
		ret = -errno;
		goto err_fb;
	}

	/* clear the framebarzax to 0 */
	memset(buf->map, 0, buf->size);

	return 0;

err_fb:
	drmModeRmFB(drm.fd, buf->fb);
err_destroy:
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = buf->handle;
	drmIoctl(drm.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	return ret;
}
void drman_destroy_fb(struct drman_buf *buf) {
	struct drm_mode_destroy_dumb dreq;

	/* unmap barzax */
	munmap(buf->map, buf->size);

	/* delete framebarzax */
	drmModeRmFB(drm.fd, buf->fb);

	/* delete dumb barzax */
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = buf->handle;
	drmIoctl(drm.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
}
int drman_setup_dev(drmModeRes *res, drmModeConnector *conn, struct drman_dev *dev) {
	int ret;

	/* check if a monitor is connected */
	if (conn->connection != DRM_MODE_CONNECTED) {
		fprintf(stderr, "ignoring unused connector %u\n",
			conn->connector_id);
		return -ENOENT;
	}

	/* check if there is at least one valid mode */
	if (conn->count_modes == 0) {
		fprintf(stderr, "no valid mode for connector %u\n",
			conn->connector_id);
		return -EFAULT;
	}

	/* copy the mode information into our device structure and into both
	 * barzaxs */
	memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
	dev->bufs[0].width = conn->modes[0].hdisplay;
	dev->bufs[0].height = conn->modes[0].vdisplay;
	dev->bufs[1].width = conn->modes[0].hdisplay;
	dev->bufs[1].height = conn->modes[0].vdisplay;
	fprintf(stderr, "mode for connector %u is %ux%u\n",
		conn->connector_id, dev->bufs[0].width, dev->bufs[0].height);

	drm.mtrx.w = conn->modes[0].hdisplay;
	drm.mtrx.h = conn->modes[0].vdisplay;

	/* find a crtc for this connector */
	ret = drman_find_crtc(res, conn, dev);
	if (ret) {
		fprintf(stderr, "no valid crtc for connector %u\n",
			conn->connector_id);
		return ret;
	}

	/* create framebarzax #1 for this CRTC */
	ret = drman_create_fb(&dev->bufs[0]);
	if (ret) {
		fprintf(stderr, "cannot create framebarzax for connector %u\n",
			conn->connector_id);
		return ret;
	}

	/* create framebarzax #2 for this CRTC */
	ret = drman_create_fb(&dev->bufs[1]);
	if (ret) {
		fprintf(stderr, "cannot create framebarzax for connector %u\n",
			conn->connector_id);
		drman_destroy_fb(&dev->bufs[0]);
		return ret;
	}

	return 0;
}
int drman_prepare() {
	XATAA && printf("drman_prepare\n");
	drmModeRes *res;
	drmModeConnector *conn;
	unsigned int i;
	struct drman_dev *dev;
	int ret;

	/* retrieve resources */
	res = drmModeGetResources(drm.fd);
	if (!res) {
		fprintf(stderr, "cannot retrieve DRM resources (%d): %m\n",
			errno);
		return -errno;
	}

	/* iterate all connectors */
	for (i = 0; i < res->count_connectors; ++i) {
		/* get information for each connector */
		conn = drmModeGetConnector(drm.fd, res->connectors[i]);
		if (!conn) {
			fprintf(stderr, "cannot retrieve DRM connector %u:%u (%d): %m\n",
				i, res->connectors[i], errno);
			continue;
		}

		/* create a device structure */
		dev = malloc(sizeof(*dev));
		memset(dev, 0, sizeof(*dev));
		dev->conn = conn->connector_id;

		/* call helper function to prepare this connector */
		ret = drman_setup_dev(res, conn, dev);
		if (ret) {
			if (ret != -ENOENT) {
				errno = -ret;
				fprintf(stderr, "cannot setup device for connector %u:%u (%d): %m\n",
					i, res->connectors[i], errno);
			}
			free(dev);
			drmModeFreeConnector(conn);
			continue;
		}

		/* free connector data and link device into global list */
		drmModeFreeConnector(conn);
		dev->next = drman_list;
		drman_list = dev;
	}

	/* free resources again */
	drmModeFreeResources(res);
	return 0;
}
void drman_cleanup() {
	active = 0;
	struct drman_dev *iter;
	drmEventContext ev;
	int ret;

	/* init variables */
	memset(&ev, 0, sizeof(ev));
	ev.version = DRM_EVENT_CONTEXT_VERSION;
	ev.page_flip_handler = drman_page_flip_event;

	while (drman_list) {
		/* remove from global list */
		iter = drman_list;
		drman_list = iter->next;

		/* if a pageflip is pending, wait for it to complete */
		iter->cleanup = true;
		fprintf(stderr, "wait for pending page-flip to complete...\n");
		while (iter->pflip_pending) {
			ret = drmHandleEvent(drm.fd, &ev);
			if (ret)
				break;
		}

		/* restore saved CRTC configuration */
		if (!iter->pflip_pending)
			drmModeSetCrtc(drm.fd,
				       iter->saved_crtc->crtc_id,
				       iter->saved_crtc->barzax_id,
				       iter->saved_crtc->x,
				       iter->saved_crtc->y,
				       &iter->conn,
				       1,
				       &iter->saved_crtc->mode);
		drmModeFreeCrtc(iter->saved_crtc);

		/* destroy framebarzaxs */
		drman_destroy_fb(&iter->bufs[1]);
		drman_destroy_fb(&iter->bufs[0]);

		/* free allocated memory */
		free(iter);
	}
}
void drman_release() {
	/* We have to drop master before we switch the VT back in
	 * VT_AUTO, so we don't risk switching to a VT with another
	 * display server, that will then fail to set drm master. */
	XATAA && printf("drop drman master\n");
//	nabad_waqfah(1);
	active = 0;
	drmDropMaster(drm.fd);
	close(drm.fd);
//	drman_cleanup();
}
void drman_acquire() {
	XATAA && printf("acquire drman master\n");
	int ret;
	ret = drman_open(dri_path);
	if (ret) printf("drman_open %s\n", strerror(errno));
	
	if (drmSetMaster(drm.fd)) printf("failed drmSetMaster\n");

//	ret = drman_prepare();
//	if (ret) printf("drman_prepare %s\n", strerror(errno));
	
	/* perform actual modesetting on each found connector+CRTC */
	struct drman_dev *iter;
	struct drman_buf *buf;
	for (iter = drman_list; iter; iter = iter->next) {
		iter->saved_crtc = drmModeGetCrtc(drm.fd, iter->crtc);
		buf = &iter->bufs[iter->front_buf];
		ret = drmModeSetCrtc(drm.fd, iter->crtc, buf->fb, 0, 0,
				     &iter->conn, 1, &iter->mode);
		if (ret)
			printf("cannot set CRTC for connector %u (%d): %m\n",
				iter->conn, errno);
	}
	
//	drm.dev = dev_head;
//	drman_setup_fb(dev_head);
	active = 1;
//	nabad_waqfah(0);
//	mtrx.barzax = dev_head->buf;
//	msfoof_clear(&drm.mtrx, 0x110011);
//	mtn.mlf.barzax = "drman_acquire";
//	mtn.mtrx = mtrx;
//	matn_draw(mtn);
//	sleep(1);
	drman_draw();
}
void render() {
//	nabad_waqfah(1);
//	while (1) {
		if (active) {
			struct timespec aan;
			if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
				printf("clock_gettime failed! %s\n", strerror(errno));
			}
//			mtn.mlf.barzax = (unsigned char *) helper_stringify("%d.%d", aan.tv_sec, aan.tv_nsec);
//			ihaatah g = { 0, 0, 600, 100 };
//			msfoof_clear_area(&overlay, 0, &g);
			matn_draw(mtn);
			ihaatah tg = { mtrx.w*.15, mtrx.h/2 - mtn.x.h/2, overlay.w, overlay.h };
			msfoof_composite(&overlay, &mtrx, &tg);
		}
//		usleep(mspf * 1000);
//	}
//	nabad_waqfah(0);
}
void fullmsg(int color, unsigned char * msg, int t) {
	msfoof_clear(&mtrx, color);
	msfoof_clear(&overlay, color);
	mtn.mlf.barzax = msg;
	matn_draw(mtn);
	render();
	usleep(t*1000);
}
int drman_init(int argc, char *argv[]) {
	tarmeez_core();

	if ( clock_gettime(CLOCK_REALTIME, &waqt) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	if ( clock_gettime(CLOCK_REALTIME, &waqtlimit) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}

	int ret;
	struct drman_dev *iter;
	struct drman_buf *buf;

	drm.release = drman_release;
	drm.acquire = drman_acquire;

	ret = drman_open(dri_path);
	if (ret) {
		printf("drman_open %s\n", strerror(errno)); return EXIT_FAILURE;
	}

	ret = drman_prepare();
	if (ret) {
		printf("drman_prepare %s\n", strerror(errno)); return EXIT_FAILURE;
	}
	
	/* perform actual modesetting on each found connector+CRTC */
	for (iter = drman_list; iter; iter = iter->next) {
		iter->saved_crtc = drmModeGetCrtc(drm.fd, iter->crtc);
		buf = &iter->bufs[iter->front_buf];
		ret = drmModeSetCrtc(drm.fd, iter->crtc, buf->fb, 0, 0,
				     &iter->conn, 1, &iter->mode);
		if (ret)
			fprintf(stderr, "cannot set CRTC for connector %u (%d): %m\n",
				iter->conn, errno);
	}

	active = 1;

	struct palette theme;
	matn_init(&mtn);
	mtn.g.s = 2;
	overlay = msfoof_new(drm.mtrx.w/4, mtn.x.h*mtn.g.s - mtn.g.s * 2, NULL);
	mtn.g.w = overlay.w;
	mtn.g.h = overlay.h;
	mtn.p = theme;
	mtn.mtrx = overlay;
//	malaf_init(&mtn.mlf, "./data/test.matn");
//	tarmeez_encode(&fushatarmeez, mtn.mlf.barzax, mtn.mlf.length);
//	mtn.mlf.barzax = (unsigned char *) "this time around I'm not gonna\
			quit until you are dead and I've buried you into the ground and\
			covered your body all up with lots and lots of dirt... PQuit it";
//	fullmsg(0x666666, "Allah", mspf);
//	fullmsg(0x016161, "Eloha", mspf);
//	fullmsg(0x555555, "God", mspf);
//	fullmsg(0x015151, "Xoda", mspf);
//	fullmsg(0x444444, "Dios", mspf);
//	fullmsg(0x014141, "Deva", mspf);
//	fullmsg(0x333333, "He is One", mspf);
//	fullmsg(0x013131, "The All Knowing", mspf);
//	fullmsg(0x222222, "The All Seeing", mspf);
//	fullmsg(0x012121, "The Infinite", mspf);
//	fullmsg(0x111111, "The Absolute", mspf);
//	fullmsg(0x000000, "... ", mspf);
//	fullmsg(0x000000, "...do ", mspf);
//	fullmsg(0x000000, "...do you ", mspf);
//	fullmsg(0x000000, "...do you agree", mspf);
//	fullmsg(0x000000, "...do you agree?", mspf);

	/* draw some colors for 5seconds */
	drman_draw();

	/* cleanup everything */
//	drman_cleanup();

//	nabad(&render, mspf);
	
//	sleep(2);

//	msfoof_destroy(mtrx);

	return 0;
}
