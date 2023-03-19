#ifndef _madxal_h
#define _madxal_h

#include "amr.h"
#include "matn.h"
#include "helper.h"
#include "masaadir.h"
#include "malaf.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>

struct madxal {
	void (*onevent)();
	int destroyed;
};
void madxal_destroy();
int madxal_init(struct madxal handler);
static struct madxal mdxl;

#endif