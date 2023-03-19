/* maxraj: manages an output backend, restarts it on freeze, quits it gracefully
 * x11, drm, weston, fbdev, console etc
 * */

#include "maxraj.h"

int size = 10;
void *addr;

long beat() { // returns ms since last frame
	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	return sec2ms(aan.tv_sec) + nano2ms(aan.tv_nsec);
}
int main(int argc, char **argv) {
	int i = 0, fd;
	char data[size];

	amr_dhikr("maxraj", size, &fd);
	amr_dhikr_axath(fd, size, &addr);
	printf("maxraj dhikr: %p\n", addr);
	
	amr_ibtada();
	amr_ittasal();
	amr_ism("maxraj");

	while (i < 15) {
		i++;
		printf("%d\n", i);
		sprintf(data, "%d", i);
		memcpy(addr, data, size);
		amr_irsal(RAKKAZ, RAEES, beat()); //usleep(200000);
		sleep(2);
	}
	
	amr_dhikr_ixttm(addr, size);
	amr_dhikr_hadhaf("maxraj");
	
	sleep(3);
}
