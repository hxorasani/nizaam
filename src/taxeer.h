#ifndef _taxeer_h
#define _taxeer_h

#include <stdlib.h>		// exit
#include <string.h>		// strerror
#include <errno.h>		// errno
//#include <sys/types.h>	// ??
#include <sys/stat.h>	// file stats
#include <stdio.h>		// perror printf
#include <unistd.h>		// close sleep
#include <fcntl.h>		// open O_RDWR

typedef struct taxeer {
	long  uid  ;
	long  rid  ; // relative id, like a nickname by the app
	long  rabt ;
	float waqt ; // ms
} taxeer;
int taxeer_core();
int taxeer_init(taxeer *t, char *path);

#endif