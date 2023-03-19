/* manage delayed waaqi3aat
 * indexes taxeeraat by amr rabt id
 * each taxeer is in milliseconds
 * handles RAKKAZ to toggle modes of tashgheel
 * 	1. all events run
 * 	2. only important events run that can wake up nizaam
 * handles MOQOOF to remove all taxeeraat by a certain amr
 * */

#include "taxeer.h"	// header file

#define XATAA 0
#define MAX_WAAQI3AAT 512
int taxeer_ready = 0;

int taxeer_core() {
	if (!taxeer_ready) {
		taxeer_ready = 1;
	}
}
int taxeer_init(taxeer *t, char *path) {
	XATAA && printf("taxeer_init %s\n", path);
	return EXIT_SUCCESS;
}
