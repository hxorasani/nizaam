#ifndef _malaf_h
#define _malaf_h

#include <wctype.h>
#include <locale.h>
#include <wchar.h>

#include <stdlib.h>		// exit
#include <string.h>		// strerror
#include <stdio.h>		// perror printf
#include <sys/stat.h>	// file stats
#include <errno.h>		// errno
#include <unistd.h>		// close sleep
#include <fcntl.h>		// open O_RDWR
#include <dirent.h>		// scandir

#include "helper.h"		// helper_stringify

/* i had casted barzax to unsigned char earlier which was a mistake
 * it caused utf8 decoding problems because utf8 expects binary data while
 * (unsigned char) caused data loss, so now it is just void
 * 
 * we no longer assume the malaf is matn, we assume it is raw binary
 * 
 * this means functions like malaf_append become matn_append
 * they're moved to the matn module and matn to malaf conversion is done
 * before saving a file.
 * */
typedef struct malaf {
	int fd;
	u_char *name;
	u_char *type; // it'll help other mods convert from it easily
	long length;
	void *barzax;
} malaf;

typedef struct {
	long total; // array length
	malaf *files; // array of strings (file names)
} files_list_t;
int files_list(const char *path, files_list_t *list);
void files_list_free(files_list_t *list);

int malaf_ilaa_matn(malaf *mlf, u_char **barzax, long *length);
void malaf_destroy(malaf *m);
// TODO
int malaf_inzar(char *path);
int malaf_hfiz(malaf *m, char *path);
int malaf_init(malaf *m, char *path);

#endif