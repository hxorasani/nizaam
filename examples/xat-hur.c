/* .xat .tarmeez file encode decode files
 * convert .xat file from one tarmeez to another
 * */

#include "xat.h"		// header file

int finduni(tarmeez *t, int uni) {
	if (uni > 0)
		for (int i = 0; i < 0x10ffff; ++i) {
			if (t->tounicode[i] == uni) return i;
		}
	return -1;
}
int main(int argc, char *argv[]) {
	if (argc > 1) {
		tarmeez_core();
		tarmeez *b = (tarmeez *) malloc(sizeof (tarmeez));
		if (tarmeez_init(b, "./data/fusha.tarmeez")) {
			printf("tarmeez core init error\n"); return EXIT_FAILURE;
		}
		xat x = { 0 };
		xat_init(&x, argv[1]);
		for (int i = 0; i < x.length; ++i) {
			if (x.eqonaat[i].id > -1) {
				int j = x.eqonaat[i].id;
				x.eqonaat[i].id = finduni(&fushatarmeez, b->tounicode[j] );
			}
		}

		matn m;
		xat_encode(x, &m);
		
		for (int i = 0; i < m.length; ++i) {
			printf("%c", m.barzax[i]);
		}

//		malaf mlf;
//		mlf.barzax = m.barzax;
//		mlf.length = m.length;
//		malaf_hfiz(&mlf, "./data/test2.xat");
	} else {
		tarmeez *a = (tarmeez *) malloc(sizeof (tarmeez));
		tarmeez *b = (tarmeez *) malloc(sizeof (tarmeez));
		memset(a, 0, sizeof (tarmeez));
		memset(b, 0, sizeof (tarmeez));
		if (tarmeez_init(a, "./data/fusha.tarmeez")) {
			printf("tarmeez core init error\n"); return EXIT_FAILURE;
		}
		if (tarmeez_init(b, "./data/fusha2.tarmeez")) {
			printf("tarmeez core init error\n"); return EXIT_FAILURE;
		}
		for (int i = 0; i < 256; ++i) {
//			printf("%d %d %lc %lc\n", i, a->tofusha[i], a->tounicode[i], b->tounicode[i]);
			printf("%d %d %d\n", i, a->tounicode[i], finduni(b, a->tounicode[i]));
		}
	}
	return EXIT_SUCCESS;
}
