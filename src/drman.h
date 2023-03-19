#ifndef _drman_h
#define _drman_h

#include "helper.h"
#include "nabad.h"
#include "msfoof.h"
#include "tarmeez.h"
#include "matn.h"
#include "teletype.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

struct drman_buf {
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t size;
	uint32_t handle;
	uint8_t *map;
	uint32_t fb;
};
struct drman_dev {
	struct drman_dev *next;

	unsigned int front_buf;
	struct drman_buf bufs[2];

	drmModeModeInfo mode;
	uint32_t conn;
	uint32_t crtc;
	drmModeCrtc *saved_crtc;

	bool pflip_pending;
	bool cleanup;

	uint8_t r, g, b;
	bool r_up, g_up, b_up;
};
struct drman {
	int fd;
	struct drman_dev *dev;
	msfoof mtrx;
	void (*acquire)();
	void (*release)();
	int destroyed;
};
void drman_destroy();
int drman_init(int argc, char *argv[]);
struct drman drm;

#endif