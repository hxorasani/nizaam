#include "msfoof.h"
#include "alwaan.h"
#include "helper.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

int main(int argc, char **argv) {

	msfoof m = msfoof_new(3, 3, NULL);

	msfoof_set_blend(&m, 1, 1, 0xff*0.25, 0);
//	msfoof_set_blend(&m, 1, 1, 0xffffff, 0.125);
//	msfoof_set_blend(&m, 1, 1, 0xffffff, 0.125);
//	msfoof_set_blend(&m, 1, 1, 0xffffff, 0.125);

	void hh( msfoof *m, int x, int y, int c ) {
		printf("%8X ", c);
		if (y+1 == m->w) printf("\n");
	}

	msfoof_each(&m, hh, 0);
	printf("\n\n\n");

    exit(EXIT_SUCCESS);
}
