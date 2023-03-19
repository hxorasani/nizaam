/* encode decode .tarmeez malafaat and amtaan
 * structs for use in other mods
 * */

#include "tarmeez.h"	// header file

#define XATAA 0
int tarmeez_ready = 0;

void tarmeez_copystr(tarmeez *t, int src[], unsigned char dest[], int len) {
	for (int i = 0; i < len; ++i) {
		if (src[i] != '\0') dest[i] = t->tofusha[ src[i] ];
	}
	dest[len] = '\0';
}
int tarmeez_decode(tarmeez *t, malaf *f, utf8 *u) { // binary to utf8
	XATAA && printf("tarmeez_decode\n");

	matn m;
	matn_init(&m);
	
	malaf_ilaa_matn(f, &m.barzax, &m.length);
	XATAA && printf("tarmeez_decode m.length %ld\n", m.length);

	// [fusha] tarmeez to unicode
	int code;
	for (long i = 0; i < m.length; ++i) {
		code = t->tounicode[ m.barzax[i] ];
		u->barzax[i] = code;
	}
	matn_destroy(&m);
	
	XATAA && printf("tarmeez_decode done %ld\n", u->length);
}
int tarmeez_encode(tarmeez *t, char *m, int length) {
	// unicode to [fusha] tarmeez
	for (int i = 0; i < length; ++i) {
		m[i] = t->tofusha[ m[i] ];
	}
}
int tarmeez_core() {
	if (!tarmeez_ready) {
		fushatarmeez = (tarmeez) { 0 };
		// TODO make a destroy function
		fushatarmeez.badeel = malloc(0x10ffff * 4 * sizeof(u_char));
		if (fushatarmeez.badeel == NULL) {
			printf("tarmeez_core badeel malloc failed! %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		fushatarmeez.tofusha = malloc(0x10ffff * sizeof(u_char));
		if (fushatarmeez.tofusha == NULL) {
			printf("tarmeez_core tofusha malloc failed! %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (tarmeez_init(&fushatarmeez, "./data/fusha2.tarmeez")) {
			printf("tarmeez core init error\n"); return EXIT_FAILURE;
		}
//		fusha2tarmeez = (tarmeez) { 0 };
//		if (tarmeez_init(&fusha2tarmeez, "./data/fusha2.tarmeez")) {
//			printf("tarmeez core init error\n"); return EXIT_FAILURE;
//		}
		tarmeez_ready = 1;
	}
}
int tarmeez_init(tarmeez *t, char *path) {
	XATAA && printf("tarmeez_init %s\n", path);
	malaf m;
	malaf_init(&m, path);
//	printf("%s", (unsigned char *) m.barzax);
	
	utf8 *unicode = (utf8 *) malloc( sizeof (utf8) );
	if (utf8_init(unicode, m.barzax)) {
		printf("tarmeez_init %s\n", strerror(errno)); return EXIT_FAILURE;
	}
	
	int c, col = 0, badeel = -1, line = 0;
	int strlimit = 4;
	int str[strlimit];
	int stri = 0; // key string index
	int key = 0; // key string as int
//	for (int i = 0; i < 0x10ffff; ++i) t->tounicode[i] = 0;
	for (int i = 0; i < 256; ++i) t->tounicode[i] = 0;
	for (long i = 0; i < unicode->length+1; ++i) {
		if (i == unicode->length) c = 9; // to allow indexing the last column
		else c = unicode->barzax[i];

		if (c == 9 || c == 10) { // came across a tab or new line
			// either way
			str[stri] = '\0'; // null terminate string
			if (col == 0) { // exiting first column
				if (line < 256) {
					key = wcstol(str, (wchar_t **) &str[stri], 10);
					XATAA > 1 && printf("a: %d ", key);
				} else {
					key = str[0];
					XATAA > 1 && printf("A: (%x) %ls ", str[0], str);
				}
			} else
			if (col == 1) { // exiting second column
				if (key > -1 && line < 256) {
					if (str[0] != 32) t->tounicode[ key ] = str[0];

					t->tofusha[ str[0] ] = (unsigned char) key;
					XATAA > 1 && printf("\t\tb: %ls ", str);
				} else {
					tarmeez_copystr(t, str, &t->badeel[ key ], stri);
					if (XATAA > 1) {
						printf("\t\tB: ");
						for (int j = 0; j < stri+1; ++j) {
//							printf("%d ", t->badeel[ key ][j]);
							printf("%d ", t->badeel[ key+j ]);
						}
					}
				}
			}
			utf8_emptystr(str, strlimit); // reset string
			stri = 0; // reset key string index

			if (c == 10) { // only when it's a new line
				XATAA > 1 && printf("\n[%d] ", line);
				col = 0; // back to the first column
				key = -1; // key not read yet
				badeel = -1; // badeel not read yet
				line++;
			} else
			if (c == 9) { // only when it's a new tab
				col++; // going to the next column
			}
		} else {
			if (stri < strlimit) {
				XATAA >= 2 && printf("(%d)", col);
				str[stri] = c;
				stri++;
			}
		}
	}
	t->tounicode[  9 ] = t->tofusha[  9 ] =  9;
	t->tounicode[ 10 ] = t->tofusha[ 10 ] = 10;
	t->tounicode[ 13 ] = t->tofusha[ 13 ] = 13;
	t->tounicode[ 32 ] = t->tofusha[ 32 ] = 32;

	utf8_destroy(unicode);
	malaf_destroy(&m);
	
	return EXIT_SUCCESS;
}
