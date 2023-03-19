/* xidmaat
 * 
 * on startup check if xidmaat is already running
 * 	if yes then just send it new args
 * check list of umoor that should be running
 * check list of running processes
 * popen missing processes
 * 
 * setup listeners for exit signals
 * monitor pipes for EOF/close
 * 
 * reads xidmaat.matn
 * each xidmah is known by its waseelah, given a uid linked to it
 * watches it for changes
 * running umoor are also watched with pid linking
 * 
 * re-popen processes that die with a timeout
 * stop re-opening after retry limit is hit
 * 
 * caxsiyyah:
 * 0daroorah	necessary for nizaam to work at all
 * 1haajah		needed by nizaam for basic features, limited without it
 * 2shahwah		desired by you but nizaam can work fine without it
 * 
 * outline
 * check if this is a tty vt
 * yes? handover control to teletype, use drm or fbdev backend
 * no? check if we're inside X or weston
 * yes? run as a client compositor inside them
 * no? exit gracefully
 * */
#define _GNU_SOURCE
#include "helper.h"

#include "malaf.h"
#include "utf8.h"
#include "tarmeez.h"

#include <sys/types.h>	// 
#include <signal.h>		// kill
#include <unistd.h>
#include <stdio.h>		// perror printf
#include <stdlib.h>		// exit
#include <sched.h>		// clone
#include <string.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>	// waitpid
#include <time.h>
#include <errno.h>
#include <sys/inotify.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
char buffer[EVENT_BUF_LEN];

#ifndef CHILD_SIG
#define CHILD_SIG SIGCHLD       // signal gen'd when child terminates
#endif

#define HADD 64 // max xidmaat
char XATAA = 1;
long ppid, pid;
int inotifyfd, inotifywd, currentxidmah = 0;
char **environment;
typedef struct xidmah {
	long waqtbadaa; // time started
	long waqtqatal; // time killed
	long mowjood; // is it present in the xidmaat.matn
	pid_t pid; // process id
	u_char uid; // unique index
	u_char caxsiyyah; // 0dar 1haj 2cah
	u_char *waseelah; // path to file
	u_char *args; // 
	u_char inqilaab; // cycle, turn, how many times its been restarted
	u_char isti3aad; // restart 0laa 1na3am
	u_char **i3timaad; // dependencies
} xidmah;
xidmah xidmaat[HADD];
void errexit(u_char *p) {
	perror(p);
	exit(1);
}
static void grimReaper(int sig) { // handler for child termination signal
	/* UNSAFE: This handler uses non-async-signal-safe functions
	   (printf(), strsignal(); see Section 21.1.2) */

	int savedErrno = errno;             /* In case we change 'errno' here */

	XATAA > 2 && printf("caught signal %d (%s)\n", sig, strsignal(sig));

	errno = savedErrno;
}
int slice(u_char *s, u_char **t, long start) {
	long end = strlen(s);
//	if (end <= start || start > end) {
//	}
	
	long length = end - start;

	// allocate u_char str, +1 to allow for null char '\0'
	u_char *tmp = calloc(length + 1, sizeof(u_char));
	if (tmp == NULL) {
		perror("slice calloc"); return EXIT_FAILURE;
	}
	
	for (long i = 0; i < end; ++i) {
		tmp[i] = s[start+i];
	}
	tmp[length] = '\0';
	
	*t = tmp;

	return EXIT_SUCCESS;
}
void qulmaqboor(xidmah *x) {
	XATAA && printf("%s %d %s buried   %s%s%s\n",
	BHWHT, x->uid, RST, DIM, x->waseelah, RST);
}
void qulmaHshoor(xidmah *x) {
	XATAA && printf("%s %d %s revived  %s%s%s\n",
	BHWHT, x->uid, RST, DIM, x->waseelah, RST);
}
void qulwaqtqatal(xidmah *x, long musammaa) {
	XATAA && printf("%s %d %s time-of-death %s%s (%ld)sec%s\n",
	BHWHT, x->uid, RST, DIM, x->waseelah, musammaa, RST);
}
void qulbadaa(xidmah *x) {
	XATAA && printf("%s %d %s started     %s%s cycle: %d%s\n",
	BHWHT, x->uid, RST, DIM, x->waseelah, x->inqilaab, RST);
}
void quldaaim(xidmah *x, pid_t p) {
	XATAA && printf("%s %d %s running     %s%s      pid: %d%s\n",
	BHWHT, x->uid, RST, DIM, x->waseelah, p, RST);
}
void qulmaqtool(xidmah *x) {
	XATAA && printf("%s %d %s killed   %s%s      pid: %d%s\n",
	BHWHT, x->uid, RST, DIM, x->waseelah, x->pid, RST);
}
void qulxidmah(xidmah *x) {
	XATAA && printf("%s %d %s    %s%s      pid: %d%s  started: %ld\n",
	BHWHT, x->uid, RST, DIM, x->waseelah, x->pid, RST, x->waqtbadaa);
}
long daqqah() {
	struct timespec aan;
	if ( clock_gettime(CLOCK_REALTIME, &aan) ) {
		printf("clock_gettime failed! %s\n", strerror(errno));
	}
	long farq = sec2ms(aan.tv_sec) + nano2ms(aan.tv_nsec);
	return farq;
}
int xidmah_bahac(u_char *p) { // .waseelah to slot num
	for (int i = 0; i < HADD; ++i) {
		xidmah x = xidmaat[i];
		if (x.waseelah != NULL && strcmp(x.waseelah, p) == 0) {
			return i;
		}
	}
	return -1;
}
int xidmah_bahac_o_xaali(u_char *p) { // return running or empty slot
	/*
	 * finds a matching xidmah using .waseelah, returns if found any
	 * if not then finds an empty slot (no .waseelah) and returns
	 * */
	int i = xidmah_bahac(p);
	if (i > -1) return i;

	for (i = 0; i < HADD; ++i) {
		xidmah x = xidmaat[i];
		if (x.waseelah == NULL) return i;
	}
	return -1;
}
xidmah* xidmah_adaaf(xidmah x) {
	/*
	 * finds a previously stored instance of this xidmah or a free slot
	 * if already present, updates it with new values
	 * returns its index from xidmaat
	 * */
//	XATAA && printf("xidmah_adaaf(%s)\n", x.waseelah);
	currentxidmah = xidmah_bahac_o_xaali(x.waseelah);
	if (currentxidmah == -1) {
		XATAA && printf("all slots are taken\n");
		return NULL;
	}
	xidmah *cx = &xidmaat[currentxidmah];
	cx->isti3aad = x.isti3aad;
	cx->waseelah = x.waseelah;
	cx->caxsiyyah = x.caxsiyyah;
	return cx;
}
void xidmah_nazzaf(int i) {
	if (i >= HADD) return;

	xidmah *cx = &xidmaat[i];
	cx->uid = i;
	cx->pid = 0;
	cx->inqilaab = 0;
	cx->caxsiyyah = 0;
	cx->waseelah = NULL;
	cx->waqtbadaa = -1;
	cx->waqtqatal = -1;
}
void xidmah_havaf(xidmah x) { // remove + close a xidmah
	int i = xidmah_bahac_o_xaali(x.waseelah);
	if (i != -1) {
		xidmah *cx = &xidmaat[i];
		xidmah_nazzaf(i);
	}
}
void execorexit(char **args) {
	int ret = execve(args[0], args, environment);
	if (ret == -1) errexit("    execv in child");
}
pid_t xidmah_fork(xidmah *cx) {
	XATAA > 1 && printf("xidmah_fork(%s)\n", cx->waseelah);
	if (strlen(cx->waseelah) <= 0) return 0;

	if (cx == NULL) return 0;

	++cx->inqilaab; // increment restart cycle
	qulbadaa(cx);

	pid_t p = fork();
	if (p == 0) {
		ppid	=	getppid();
		pid		=	getpid();
		char *args[3] = { '\0' };

		XATAA > 3 && printf("service (%s) pid(%ld) ppid(%ld)\n", cx->waseelah, pid, ppid);
		
		if (cx->waseelah[0] == '-') { // app
			args[0] = "./bin/app.dukjs";
			u_char *b;
			slice(cx->waseelah, &b, 1);
			args[1] = b;
			args[2] = NULL;
			execorexit(args);
			free(b);
		} else { // xidmah
			args[0] = cx->waseelah;
			args[1] = NULL;
			execorexit(args);
		}
	} else
	if (pid == -1) {
		errexit("    fork error");
	} else { // in parent
		quldaaim(cx, p);

		cx->waqtbadaa = daqqah();
		cx->pid = p;
		usleep(100 * 1000); // 1s
	}
	return p;
}
void xidmaatmatn() {
	/* 
	 * the goal here is to watch this file for changes
	 * and update the xidmaat with new data, even remove them (or close them?)
	 * xidmaat not marked daroorah should be closed if not found in this file
	 * */
	
	matn m;
	matn_init(&m);
	malaf mlf;
	malaf_init(&mlf, "data/xidmaat.matn");
	matn_nazaf(&m);
	malaf_ilaa_matn(&mlf, &m.barzax, &m.length);

	xidmah *cx = NULL;
	for (int i = 0; i < HADD; ++i) {
		cx = &xidmaat[i];
		cx->mowjood = 0;
	}

	XATAA > 1 && printf("\nxidmaat.matn length: %ld\n\n", m.length);
	
	/* TAFSEER
	 * doesn't support spaces in commands, only takes one argument for now
	 * 
	 * anwaa3: command arg, #comment
	 * after each \n
	 * 		check first char to determine naw3
	 * 		reset col
	 * 		reset start index
	 * each space means a new col
	 * 		first col is command
	 * 		rest of the cols are args
	 * */
	
	xidmah *tx = NULL;
	int col = 0; // column
	u_char *arg = NULL;
	u_char c;
	long fc = -1, start = 0;
	for (long i = 0; i < m.length+1; ++i) {
		if (i == m.length) c = '\n'; // to allow indexing the last one
		else c = m.barzax[i];
		if (c == '\n' || c == ' ') { // came across a space, new col
			if (col <= 1) {
				if (fc == '#') {} // ignore # comments
				else if (fc == '~') {}
				else {
					matn_iqtabas_uc(&m, &arg, start+col, i+1);
					if (arg != NULL && strlen(arg)) {
						if (col == 0) {
							// if a xidmah is complete from before, add it
							int moxid = xidmah_bahac_o_xaali(arg);
							if (moxid != -1) {
								tx = &xidmaat[moxid];
								tx->waseelah = arg;
							}
							
							tx->caxsiyyah = 1;
							tx->isti3aad = 1;
							if (tx != NULL) {
								tx->mowjood = 1;
							}
						} else if (tx != NULL) {
							tx->args = arg;
						}
					}
					// empty strings should be freed
					if (arg != NULL && strlen(arg) == 0) {
						free(arg);
					}
				}
			}
			col++;
			arg = NULL;
			start = i;
		}
		if (c == '\n') { // came across a new line
			start = i+1;
			col = 0;
			fc = -1;
		} else if (fc == -1) {
			fc = c;
		}
	}

	for (int i = 0; i < HADD; ++i) {
		xidmah *cx = &xidmaat[i];
		if (cx->caxsiyyah > 0 && cx->mowjood == 0) {
			qulmaqboor(cx);
			if (cx->pid > 0) { // terminate the process
//				kill(cx->pid, SIGKILL);
				kill(cx->pid, SIGTERM);
			}
			if (cx->waseelah != NULL) {
				free(cx->waseelah);
				waitpid(cx->pid, NULL, 0);
				xidmah_nazzaf(i);
			}
			// these get waited on later for cleanup in the main event loop
		}
	}

	matn_destroy(&m);
	malaf_destroy(&mlf);
}
void watchit() {
	inotifywd = inotify_add_watch(inotifyfd, "./data/xidmaat.matn",
		IN_ATTRIB | IN_MODIFY| IN_IGNORED | IN_DELETE | IN_ISDIR );
	if (inotifywd == -1)  { perror("inotify_add_watch"); exit(EXIT_FAILURE); }
}
void watchinit() {
//	__attribute__ ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	inotifyfd = inotify_init1(IN_NONBLOCK);	
	if ( inotifyfd < 0 ) { perror( "inotify_init" ); exit(EXIT_FAILURE); }
	watchit();
}
void iqrata3deel() {
	ssize_t numRead = read(inotifyfd, buffer, EVENT_BUF_LEN);
	if (numRead == 0) { perror("read"); exit(EXIT_FAILURE); }

    char *p;
    struct inotify_event *e;

	if (numRead > 0)
	for (p = buffer; p < buffer + numRead; ) {
		e = (struct inotify_event *) p;
		if ( e->mask & IN_ISDIR ) {
			// it should really be a file
		}
		else {
			if ( e->mask & IN_IGNORED ) {
				watchit();
			}
			// changes
			xidmaatmatn();
		}

		p += EVENT_SIZE + e->len;
	}
}
void watchdestroy() {
	inotify_rm_watch( inotifyfd, inotifywd );
	close( inotifyfd );
}
int main(int argc, char *argv[], char *envp[]) {
	// make sure only one xidmaat instance runs
	int pid_file = open("./daaim/xidmaat.pid", O_CREAT | O_RDWR, 0666);
	int rc = flock(pid_file, LOCK_EX | LOCK_NB);
	if (rc) {
		if (EWOULDBLOCK == errno) {
			printf("already running\n"); // already running
			return EXIT_FAILURE; // another instance is running
		}
	}

	environment = envp;
	
	if (XATAA > 2)
	for (int i = 0; i < 2048; ++i) {
		if (envp[i] == NULL) break;
		printf("%d %s\n", i, envp[i]);
	}
	for (int i = 0; i < HADD; ++i) {
		xidmah_nazzaf(i);
	}

	struct sigaction sa;
	if (CHILD_SIG != 0) {
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		sa.sa_handler = grimReaper;
		if (sigaction(CHILD_SIG, &sa, NULL) == -1) errexit("sigaction");
	}

	ppid	=	getppid();
	pid		=	getpid();
	XATAA && printf("services pid(%ld) ppid(%ld)\n", pid, ppid);
	
//	xidmah
//	waajihah	= { .waseelah = "./bin/waajihah",	.isti3aad = 1, .caxsiyyah = 0 },
//	madxal		= { .waseelah = "./bin/madxal",		.isti3aad = 1, .caxsiyyah = 0 },
//	mudeer		= { .waseelah = "./bin/mudeer",		.isti3aad = 1, .caxsiyyah = 0 },
//	x11			= { .waseelah = "./bin/x11",		.isti3aad = 1, .caxsiyyah = 0 }
//	;
//	
//	xidmah_adaaf(waajihah);
//	xidmah_adaaf(madxal);
//	xidmah_adaaf(mudeer);
//	xidmah_adaaf(x11);

	xidmaatmatn();
	
	watchinit();

	XATAA && printf("%s... services listening%s\n", DIM, RST); // xidaamtu yasma3oon
	while (1) {
		iqrata3deel();

		for (int i = 0; i < HADD; ++i) {
			xidmah *x = &xidmaat[i];
			if (x->pid > 0) { // running
				waitpid(x->pid, NULL, WNOHANG);
				int r = kill(x->pid, 0);
				if (r == -1) {
					qulmaqtool(x);
					x->pid = 0;
					x->waqtqatal = daqqah();
					if (x->isti3aad) {
						if (x->inqilaab < 3) xidmah_fork(x);
						else {
							qulmaqboor(x);
						}
					}
				} else if (x->waqtqatal != -1) { // was killed, now found running
					qulmaHshoor(x);
					x->inqilaab = 0; // reset restart count
					x->waqtqatal = -1; // forget murder
				}
			} else if (x->waseelah && x->isti3aad) { // not running
				// try to start enabled xidmaat
				// if a xidmah has waseelah set, then we assume it's enabled
				// be sure not to start xidmaat that were killed <20s ago
				if (x->waqtbadaa == -1) { // never started
					// if waqtqatal is zero, we assume this is a new xidmaat start
					x->inqilaab = 0; // reset restart count
					xidmah_fork(x);
				} else {
					long musammaa = ms2sec(daqqah() - x->waqtqatal); // time since killed
					if (musammaa > 20) { // seconds
						qulwaqtqatal(x, musammaa);
						x->inqilaab = 0; // reset restart count
						xidmah_fork(x);
					}
				}
			}
		}

		sleep(4);
	}

	watchdestroy();
	
	exit(EXIT_SUCCESS);
}


