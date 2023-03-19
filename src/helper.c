#include "helper.h"

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
struct timespec waqt2;
int mabdoo = 0;

void make_socket_non_blocking(int sockfd) {
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl F_GETFL"); exit(EXIT_FAILURE);
	}
	
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL O_NONBLOCK"); exit(EXIT_FAILURE);
	}
}
long nano2ms(long nano) {
	return nano / 1000000;
}
long ms2nano(long ms) { // milliseconds to nanoseconds
	return ms * 1000000;
}
long sec2ms(long s) {
	return s * 1000;
}
long ms2sec(long ms) {
	return ms / 1000;
}
static void print_siginfo(siginfo_t *si) {
   timer_t *tidp;
   int or;

   tidp = si->si_value.sival_ptr;

   printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
   printf("    *sival_ptr = 0x%lx\n", (long) *tidp);

   or = timer_getoverrun(*tidp);
   if (or == -1)
	   printf("timer_getoverrun");
   else
	   printf("    overrun count = %d\n", or);
}
void blah() {
//   printf("Caught signal %d\n", sig);
//   print_siginfo(si);
//   signal(sig, SIG_IGN);
}
void helper_interval_stop(timer_t timerid) {
	if (timer_delete( timerid ))
		printf("helper_interval_stop timer_delete %s\n", strerror(errno));
}
timer_t helper_interval(void (*handler)(), long ms) {
	if (handler == NULL) {
		printf("helper_interval needs a handler\n"); return NULL;
	}

	timer_t timerid;
	struct sigevent sev;
	struct itimerspec its;

//	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = handler;
	sev.sigev_signo = SIG;
	sev.sigev_value.sival_ptr = &timerid;
	if (timer_create(CLOCKID, &sev, &timerid)) {
		printf("timer_create %s\n", strerror(errno)); return NULL;
	}

	printf("timer ID is 0x%lx\n", (long) timerid);

	ms = ms2nano(ms);
	its.it_value.tv_sec = ms / 1000000000;
	its.it_value.tv_nsec = ms % 1000000000;
	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

	if (timer_settime(timerid, 0, &its, NULL) == -1) {
		printf("timer_settime %s\n", strerror(errno)); return NULL;
	}

	return timerid;
}
void helper_init() {
//	sigset_t mask;
//	struct sigaction sa;
//
//	// Establish handler for timer signal
//	sa.sa_flags = SA_SIGINFO;
//	sa.sa_sigaction = blah;
//	sigemptyset(&sa.sa_mask);
//	if (sigaction(SIG, &sa, NULL) == -1)
//		printf("sigaction %s\n", strerror(errno));
}
u_char * helper_stringify(const char *fmt, ...) {
	int size = 0;
	unsigned char *p = NULL;
	va_list ap;
	
	// Determine required size
	va_start(ap, fmt);
	size = vsnprintf(p, size, fmt, ap);
	va_end(ap);
	if (size < 0) return NULL;
	
	size++; // For '\0'
	p = malloc(size);
	if (p == NULL) return NULL;
	
	va_start(ap, fmt);
	size = vsnprintf(p, size, fmt, ap);
	va_end(ap);
	if (size < 0) {
		free(p);
		return NULL;
	}
	
	return p;
}

void taba3_waqt(u_char *str) {
//	if ( !mabdoo && clock_gettime(CLOCK_REALTIME, &waqt) ) {
//		printf("clock_gettime failed! %s\n", strerror(errno));
//		mabdoo = 1;
//	}

	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}

	long msecs = nano2ms(aan.tv_nsec-waqt2.tv_nsec);
	
	struct tm *tm;
	tm = localtime(&aan.tv_sec);
//	tm->tm_year + 1900;
//	tm->tm_mon + 1
	printf("%s%2d:%2d:%2d.%3ld%s %s\n", DIM,
//	tm->tm_mday,
	tm->tm_hour,
	tm->tm_min,
	tm->tm_sec,
	msecs,
	RST, str);
}
long axav_waqt() {
	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}

	return sec2ms(aan.tv_sec-waqt2.tv_sec) + nano2ms(aan.tv_nsec-waqt2.tv_nsec);
}


