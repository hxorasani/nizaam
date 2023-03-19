#ifndef _tarmeez_h
#define _tarmeez_h

#include "malaf.h"
#include "matn.h"
#include "utf8.h"

#include <stdlib.h>		// exit
#include <string.h>		// strerror
#include <errno.h>		// errno
//#include <sys/types.h>	// ??
#include <sys/stat.h>	// file stats
#include <stdio.h>		// perror printf
#include <unistd.h>		// close sleep
#include <fcntl.h>		// open O_RDWR

typedef struct tarmeez {
	int tounicode[256]; // fusha char to unicode int
//	u_char badeel[0x10ffff][4]; // unicode int to char[4]
//	u_char tofusha[0x10ffff]; // unicode int gives you fusha char
	u_char *badeel; // unicode int to char[4]
	u_char *tofusha; // unicode int gives you fusha char
} tarmeez;
tarmeez fushatarmeez; // default global tarmeez
tarmeez fusha2tarmeez; // default global tarmeez
int tarmeez_decode(tarmeez *t, malaf *f, struct utf8 *u);
int tarmeez_encode(tarmeez *t, char *m, int length);
int tarmeez_core();
int tarmeez_init(tarmeez *t, char *path);

#endif