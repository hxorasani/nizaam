/* nabad, pulse, heartbeat
 * either beats at a regular millisecond interval or at the vsync
 * */

#include "helper.h"
#include "nabad.h"

int nabad_waqfah_yes = 0;
int nabad_hayaat = 1;
int nabad_nanos = 50;
pthread_t nabad_id;
pid_t nabad_pid; // process that owns nabad
void nabad_error(int err, char *str) {
	printf("%s, %s\n", strerror(err), str);
	exit(EXIT_FAILURE);
}
struct timespec nabad_waqt;
long tick() { // returns 1 if has beaten, 0 if waiting for next tick
	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	long farq = sec2ms(aan.tv_sec-nabad_waqt.tv_sec) + nano2ms(aan.tv_nsec-nabad_waqt.tv_nsec);

	if (nabad_waqt.tv_sec)
		printf("Thread tick %ld\n", farq);

	if ( clock_gettime(CLOCK_REALTIME, &nabad_waqt) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	return farq;
}
static void * thread_start(void *arg) {
//    printf("Thread started\n");

	while (nabad_hayaat) {
		usleep(nabad_nanos);
		if (!nabad_waqfah_yes) kill(nabad_pid, SIGCONT);
//		tick();
	}

//	printf("Thread dying\n");
}
void nabad_iqtal() {
	nabad_hayaat = 0;
}
void nabad_waqfah(int yes) {
	nabad_waqfah_yes = yes;
}
void nabad( void (*handler)(), int ms ) {
	// TODO, if ms == -1, try to beat at vsync
	tick();
    int s;
    pthread_attr_t attr;
	struct sigaction sa;
	
	if (ms >= 10) nabad_nanos = ms * 1000;

	// Establish handler for timer signal
	sa.sa_flags = SIGCONT;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGCONT, &sa, NULL) == -1)
		printf("sigaction %s\n", strerror(errno));

    // Initialize thread creation attributes
    s = pthread_attr_init(&attr);
    if (s != 0) nabad_error(s, "pthread_attr_init");

    // Create one thread for each command-line argument
	s = pthread_create(&nabad_id, &attr, &thread_start, NULL);
	if (s != 0) nabad_error(s, "pthread_create");

    // Destroy the thread attributes object, since it is no longer needed
    s = pthread_attr_destroy(&attr);
    if (s != 0) nabad_error(s, "pthread_attr_destroy");

    // Now join with thread, and display its returned value
//	s = pthread_join(nabad_id, NULL);
//	if (s != 0) nabad_error(s, "pthread_join");
}
