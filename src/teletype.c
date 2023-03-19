/* teletype outline
 * find a non-opened VT, if not root or not a vt, exit
 * open it and 
 * */

#include "teletype.h"	// header file
#include <linux/vt.h>	// VT_GETMODE ...
#include <linux/major.h>// major
#include <linux/kd.h>	// KD_GRAPHICS KD_TEXT ...
#include <stdlib.h>		// exit
#include <string.h>		// strerror
#include <errno.h>		// errno
//#include <sys/types.h>	// ??
#include <sys/ioctl.h>	// control device
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>	// file stats
#include <stdio.h>		// perror printf
#include <unistd.h>		// close sleep
#include <fcntl.h>		// open O_RDWR
#include <signal.h>		// examine and change a signal action, sigaction

#define XATAA 1
#define SIGREL SIGRTMIN+1
#define SIGACQ SIGRTMIN+2

#ifndef KDSKBMUTE
#define KDSKBMUTE	0x4B51
#endif

#ifndef EVIOCREVOKE
#define EVIOCREVOKE _IOW('E', 0x91, int)
#endif

#define MAX_ARGV_SIZE 256

#include <sys/sysmacros.h>	// major()/minor()

int teletype_setup();
void teletype_acq_rel();

struct vt_mode vtm, ovtm;
int open_tty_by_number(int ttynr) {
	int ret;
	char filename[16];

	ret = snprintf(filename, sizeof filename, "/dev/tty%d", ttynr);
	if (ret < 0)
		return -1;

	return open(filename, O_RDWR | O_NOCTTY);
}
int open_tty_by_name(char ttyname[]) {
	int ret;
	char filename[16];

	ret = snprintf(filename, sizeof filename, "/dev/%s", ttyname);
	if (ret < 0)
		return -1;

	return open(filename, O_RDWR | O_NOCTTY);
}
void teletype_loud() {
	XATAA && printf("loud teletype\n");
	if (ioctl(tty.console, KDSKBMUTE, 0) &&
	    ioctl(tty.console, KDSKBMODE, K_UNICODE)) {
		printf("failed to set K_UNICODE keyboard mode: %s\n", strerror(errno));
	}
}
void teletype_silence() {
	// DANGEROUS gets stuck in rawmode or offmode and then u cant switch at all
	XATAA && printf("silence teletype\n");
	if (ioctl(tty.console, KDSKBMUTE, 1) &&
	    ioctl(tty.console, KDSKBMODE, K_OFF)) {
		printf("failed to set K_OFF keyboard mode: %s\n", strerror(errno));
	}
}
void teletype_txtmode() {
	XATAA && printf("set text mode\n");
	int c = ioctl(tty.console, KDSETMODE, KD_TEXT);
	if (c < 0) printf("could not set text mode, %s\n", strerror(errno));
}
void teletype_gfxmode() {
	XATAA && printf("set graphics mode\n");
	int c = ioctl(tty.console, KDSETMODE, KD_GRAPHICS);
	if (c < 0) printf("could not set graphics mode, %s\n", strerror(errno));
}
void teletype_release(int signo) {
	XATAA && printf("teletype_release(%d): ", signo);
	if (tty.onrelease != NULL) {
		printf(": onrelease...\n");
		tty.onrelease();
	} else printf("\n");
//	teletype_txtmode();
	ioctl(tty.console, VT_RELDISP, 1);
	
	teletype_acq_rel();
}
void teletype_acquire(int signo) {
	XATAA && printf("teletype_acquire(%d): ", signo);
	if (tty.onacquire != NULL) {
		printf(": onacquire...\n");
		tty.onacquire();
	} else printf("\n");
//	teletype_gfxmode();
//	teletype_silence();
	/* ACK to kernel that switch to console is successful */
	ioctl(tty.console, VT_RELDISP, VT_ACKACQ);
	
	teletype_acq_rel();
}
void teletype_acq_rel() {
	/* 
	 * first we get the current settings, then we change the local
	 * copy, then we set the kernel's copy to look like the changed local copy.
	 *
	 * struct sigaction {
	 * 	void     (*sa_handler)(int);
	 * 	void     (*sa_sigaction)(int, siginfo_t *, void *);
	 * 	sigset_t   sa_mask;
	 * 	int        sa_flags;
	 * 	void     (*sa_restorer)(void);
	 * };
	 * */
	struct sigaction sa;
	/* we set SIGUSR{1,2} to point to *_vt() */
	sigemptyset(&(sa.sa_mask));
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = teletype_release;
	sigaction(SIGREL, &sa, NULL);
	sa.sa_handler = teletype_acquire;
	sigaction(SIGACQ, &sa, NULL);
}
int teletype_switch(int to) {
	int c = ioctl(tty.console, VT_ACTIVATE, to);
	if (c < 0) {
		printf("could not switch to vt %d %s\n", to, strerror(errno));
		return EXIT_FAILURE;
	} else {
		XATAA && printf("switching to vt %d\n", to);
	}

	c = ioctl(tty.console, VT_WAITACTIVE, to);
	if (c < 0) {
		printf("failed to wait for vt %d %s\n", to, strerror(errno));
		return EXIT_FAILURE;
	} else {
		XATAA && printf("waiting for vt %d\n", to);
	}
	return EXIT_SUCCESS;
}
int teletype_verify(int to) {
	// returns -1 on error or the tty number
	struct stat buf;
	if (fstat(to, &buf) < 0) {
		printf("stat on %s failed: %s\n", ttyname(to), strerror(errno));
		return -1;
	}

	if (major(buf.st_rdev) != TTY_MAJOR) {
		printf("%s is an invalid tty device\n", ttyname(to));
		return -1;
	}

	return minor(buf.st_rdev);
}
int teletype_find() {
	int c = ioctl(tty.console, VT_OPENQRY, &tty.available);
	if (c < 0 || tty.available == -1) {
		printf("could not find available vt, %s\n", strerror(errno));
		return EXIT_FAILURE;
	} else {
		XATAA && printf("available vt %d\n", tty.available);
	}
	return EXIT_SUCCESS;
}
void teletype_destroy() {
	if (!tty.destroyed) {
//		teletype_loud();
		teletype_txtmode();
		teletype_switch(2);
		tty.destroyed = 1;
		if (tty.is_vt) {
			ioctl(tty.console, VT_SETMODE, &ovtm);
			XATAA && printf("restored old VT settings.\n");
		}
		close(tty.console);
	}
}
int teletype_setup() {
	XATAA && printf("teletype_setup %d, %d\n", SIGREL, SIGACQ);
	teletype_acq_rel();

	/*
	 * Now that we have set up these signal handlers, we will tell the virtual
	 * console manager about them.
	 * We did not want to tell the virtual console manager to route requests to
	 * change virtual consoles through these signals until the signals' handlers
	 * had been installed, because to do otherwise could cause a small
	 * possibility of a bug on very slow machines which are running too many
	 * processes at once.
	 * Keep a copy around to restore at appropriate times
	 * ovtm is another vt_mode structure, like vtm.
	 * */
	ovtm = vtm;
	/* Setting vtm.mode to VT_PROCESS causes the kernel to ask permission to
	 * change virtual consoles.
	 * */
	vtm.mode = VT_PROCESS;
	/*
	 * Setting vtm.relsig to SIGREL and vtm.acqsig to SIGACQ tells the
	 * kernel how to ask permission.
	 * */
	/*int kb_mode;
	if (ioctl(tty.console, KDGKBMODE, &kb_mode)) {
		printf("failed to get current keyboard mode: %s\n", strerror(errno));
	}
	printf("kb_mode %d\n", kb_mode);*/

//	teletype_silence();

	/* handled by teletype_release(): */
	vtm.relsig = SIGREL;
	/* handled by teletype_acquire(): */
	vtm.acqsig = SIGACQ;
	ioctl(tty.console, VT_SETMODE, &vtm);
}
int teletype_init(int argc, char *argv[]) {
	int c;
	char console[] = "/dev/console"; // same as /dev/tty0
	tty.is_vt = 0;

	/*
	 * if theres a requested ttyN, then open that or fail
	 * else check if the current console is a vt
	 * else find first non-opened ttyN and switch to it
	 * */
	if (argv[1] != NULL) {
		tty.console = open_tty_by_name(argv[1]);
		if (tty.console < 0) {
			printf("%s could not be opened, %s\n", argv[1], strerror(errno));
			if (errno == EACCES) printf("need root\n");
			return EXIT_FAILURE;
		} else {
			XATAA && printf("opened requested vt %s\n", ttyname(tty.console));
			tty.is_vt = 1;
		}
	} else {
		tty.console = STDIN_FILENO;
		int failure = teletype_verify(tty.console);
		if (failure > 0) {
			printf("%s is a vt\n", ttyname(tty.console));
			tty.is_vt = 1;
		} else {
			// /dev/console stands for the current screen, might not be free
			tty.console = open(console, O_WRONLY | O_CLOEXEC);
			if (tty.console < 0) {
				printf("could not open %s %s %s\n", console, ttyname(tty.console),
						strerror(errno));
				return EXIT_FAILURE;
			} else {
				failure = teletype_find();
				if (failure > 0)
					return EXIT_FAILURE;

				tty.console = open_tty_by_number(tty.available);
				if (tty.console < 0) {
					printf("could not open vt %d %s\n", tty.available, strerror(errno));
					return EXIT_FAILURE;
				}

				if (teletype_switch(tty.available))
					return EXIT_FAILURE;
			}
		}
	}

	/*
	 * VT_GETMODE fills a vt_mode structure with the current VT settings. If it
	 * returns an error, the program must not be running on a virtual console.
	 * we do not exit on this error, but do set the is_vt variable to 0, and
	 * do not try to use any more VT ioctl()'s if the is_vt variable is set to 0
	 * 
	 * from linux/vt.h
	 * struct vt_mode {
	 * 	char mode;              // vt mode
	 * 	char waitv;             // if set, hang on writes if not active
	 * 	short relsig;           // signal to raise on release req
	 * 	short acqsig;           // signal to raise on acquisition
	 * 	short frsig;            // unused (set to 0) 
	 * };
	 * 
	 * */
	c = ioctl(tty.console, VT_GETMODE, &vtm);
	if (c < 0) {
		printf("failed to get mode info, %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	teletype_setup();
	XATAA && printf("started listening for signals...\n");

	return EXIT_SUCCESS;
}
