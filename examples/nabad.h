#ifndef _nabad_h
#define _nabad_h

#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void nabad_iqtal();
void nabad_waqfah(int yes);
void nabad( void (*handler)(), int ms );

#endif