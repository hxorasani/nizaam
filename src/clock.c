/* clock: 
 * 
 * */

#include "clock.h"

#define XATAA 0

int main(int argc, char **argv) {
	composer t = {
		.ism = "clock",
		.xitaab = "apps/clock.js",
	};
	composer_badaa(t);

    composer_tamaam();

    return EXIT_SUCCESS;
}
