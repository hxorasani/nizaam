/* app: 
 * 
 * */

#include "app.h"

#define XATAA 0

int main(int argc, char **argv) {
	if (argc >= 2) {
		prctl(PR_SET_NAME, (unsigned long) argv[1], 0, 0, 0);
		u_char *tmp = helper_stringify("apps/%s.js", argv[1]);
		composer t = {
			.ism = argv[1],
			.xitaab = tmp,
		};
		composer_init(t);

		composer_tamaam();
		free(tmp);
	} else {
		printf("%s\n", "could not find app");
	}

    return EXIT_SUCCESS;
}
