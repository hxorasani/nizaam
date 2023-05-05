/*
 * this is not utf8, this is unicode (hence the int *)
 * this needs to be renamed
 * */

#include "utf8.h"

#define XATAA 0

int uni2chars(char *dest, int uni, int size) { // unicode int to multibyte chars
	int src[2] = { uni, '\0' };
	return wcstombs(dest, src, size);
}
char *unis2chars(int *uni) { // unicode ints to multibyte chars
	int size = wcstombs(NULL, uni, 0)+1;
	char *dest = calloc(size, sizeof(char));
	if (dest == NULL) { perror("unis2chars calloc"); exit(EXIT_FAILURE); }
	wcstombs(dest, uni, size);
	return dest;
}
int chars2unis(int *dest, char *uni, int size) { // multibyte chars to unicode ints
	return mbstowcs(dest, uni, size);
}
void utf8_clean(utf8 *u) { // remove double spaces & line breaks
	// TODO deprecate this
	XATAA && printf("utf8_clean\n");
	
	utf8 t;
	t.length = 0;
	t.barzax = calloc(u->length + 1, sizeof(wchar_t));
	if (t.barzax == NULL) {
		perror("utf8_clean calloc");
	}

	long xc = 0, c = 0;
	for (long i = 0; i < u->length; ++i) {
		c = u->barzax[i];
		if (i) xc = u->barzax[i-1];

		if (xc == ' ' && c == ' ') {
			// ignore consecutive spaces
		} else
		if (xc == '\n' && c == '\n') {
			// ignore consecutive new lines
		} else {
			t.barzax[t.length] = u->barzax[i];
			t.length++;
		}
	}
	t.barzax[t.length++] = '\0';
	
	free(u->barzax);
	XATAA && printf("utf8_clean %ld -> %ld\n", u->length, t.length);

	u->length = t.length;
	u->barzax = t.barzax;
}
void utf8_nazzaf(utf8 *u) {
	utf8_clean(u);
}
void utf8_emptystr(int *str, int len) {
	for (int i = 0; i < len; ++i) {
		str[i] = '\0';
	}
}
long utf8_strlen(int * barzax) {
	long i = 0;
	for (; barzax[i] != '\0'; ++i);
	return i;
}
int utf8_cmp(int *a, int *b) {
	int len = utf8_strlen(a);
	if (len != utf8_strlen(b)) return 0;
	for (int i = 0; i < len; ++i) {
		if (a[i] != b[i]) return 0;
	}
	return 1;
}
int utf8_cmp_uchar(int *a, unsigned char *b) {
	int len = utf8_strlen(a);
	if (len != strlen(b)) return 0;
	for (int i = 0; i < len; ++i) {
		if (a[i] != b[i]) return 0;
	}
	return 1;
}
void utf8_destroy(utf8 *u) {
	if (u && u->barzax) {
		free(u->barzax);
		u->barzax = NULL;
	}
}
int utf8_iqtabas(utf8 *s, utf8 *t, long start, long end) {
	if (end <= start) {
		perror("utf8_iqtabas length <= 0"); return EXIT_FAILURE;
	}
	
	t->length = end - start;

	// allocate widechar str, +1 to allow for null wide char L'\0'
	t->barzax = calloc(t->length + 1, sizeof(wchar_t));
	if (t->barzax == NULL) {
		perror("utf8_iqtabas calloc"); return EXIT_FAILURE;
	}
	
	for (long i = 0; i < t->length; ++i) {
		t->barzax[i] = s->barzax[start+i];
	}

	return EXIT_SUCCESS;
}
int utf8_iqtabas_matn(utf8 *s, unsigned char **barzax, long length, long start, long end) {
	XATAA && printf("utf8_iqtabas_matn\n");

	if (end <= start) {
		perror("utf8_iqtabas_matn length <= 0"); return EXIT_FAILURE;
	}
	
	length = end - start;

	// allocate widechar str, +1 to allow for null wide char L'\0'
	*barzax = (unsigned char *) calloc(length + 1, sizeof(unsigned char));
	if (*barzax == NULL) {
		perror("utf8_iqtabas_matn calloc"); return EXIT_FAILURE;
	}
	
	unsigned char c;
	for (long i = 0; i < length; ++i) {
		c = (unsigned char) s->barzax[start+i];
		*barzax[i] = c;
	}

	return EXIT_SUCCESS;
}
int utf8_ilaa_matn(utf8 *s, u_char **barzax, long *length ) {
	XATAA && printf("utf8_ilaa_matn %ld %p\n", s->length, s->barzax);
	if (*barzax != NULL) free(*barzax);

	*length = (long) s->length;
	XATAA && printf("utf8_ilaa_matn m.length %ld\n", *length);

	*barzax = (u_char *) malloc( *length * sizeof(u_char) + 1 );
	if (*barzax == NULL) {
		perror("utf8_ilaa_matn malloc"); return EXIT_FAILURE;
	}

	(*barzax)[ *length ] = '\0';
	for (int i = 0; i < *length; ++i) {
		(*barzax)[i] = (u_char) s->barzax[i];
	}

	XATAA && printf("utf8_ilaa_matn done %ld\n", strlen(*barzax));
	
	return EXIT_SUCCESS;
}
int utf8_init(utf8 *u, void *barzax ) {
	XATAA && printf("utf8_init %p %p\n", u, barzax);

	// if m is NULL, then only memory is allocated with size u->length
	if (barzax != NULL) {
		if (setlocale(LC_ALL, "C.UTF-8") == NULL) {
			perror("utf8_init setlocale"); return EXIT_FAILURE;
		}

		// calc the length req'd to hold string converted to a wide char string
		u->length = mbstowcs(NULL, barzax, 0);
		if (u->length == (size_t) -1) {
			perror("utf8_init 1 mbstowcs"); return EXIT_FAILURE;
		}
	}

	// allocate widechar str, +1 to allow for null wide char L'\0'
	u->barzax = calloc(u->length + 1, sizeof(wchar_t));
	if (u->barzax == NULL) {
		perror("utf8_init calloc"); return EXIT_FAILURE;
	}

	if (barzax != NULL) {
		// convert multibyte char string to a wide char string
		if (mbstowcs(u->barzax, barzax, u->length + 1) == (size_t) -1) {
			perror("utf8_init 2 mbstowcs"); return EXIT_FAILURE;
		}
	}

	XATAA && printf("utf8_init end %ld", u->length);
	XATAA && printf("\n");

	/* TEST
	 * try diff utf8 files and see if the length is correct, especially when
	 * passed to other functions like _clean
	 * */
//	printf( "%d %d %d\n", *(wchar_t *)(u->barzax + u->length-2), *(wchar_t *)(u->barzax + u->length-1), *(wchar_t *)(u->barzax + u->length) );

	return EXIT_SUCCESS;
}
