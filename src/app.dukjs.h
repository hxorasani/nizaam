#ifndef _app_dukjs_h
#define _app_dukjs_h

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/input.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "amr.h"
#include "matn.h"
#include "alwaan.h"
#include "duktape.h"

#include <sys/prctl.h>
#include <sys/wait.h>	// waitpid

#include "composer.h"

void on_reload();
void load_js_file(char *path);

#endif
