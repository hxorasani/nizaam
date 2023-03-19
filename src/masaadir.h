#ifndef _masaadir_h
#define _masaadir_h

#include "helper.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/input.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

enum {
	UNKNOWN		= 0x00,
	KEYBOARD	= 0x01,
	MOUSE		= 0x02,
	TOUCHPAD	= 0x03,
	TOUCHSCREEN	= 0x04,
	PEN			= 0x05,
};
struct label {
    const char *name;
    int value;
};
typedef struct masdar {
	char *name;
	char *location;
	unsigned char *path;
	pthread_t thread;
	int fd;
	int type;
	struct input_absinfo x;
	struct input_absinfo y;
	int resolution[2];
	int maxtouches;
	char *properties;
} masdar;

void masaadir_destroy();
masdar * masaadir_get();
int masaadir_init();

#endif