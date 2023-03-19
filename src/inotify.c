/* demo_inotify.c

   Demonstrate the use of the inotify API.

   Usage: demo_inotify pathname...

   The program monitors each of the files specified on the command line for all
   possible file events.

   This program is Linux-specific. The inotify API is available in Linux 2.6.13
   and later.
*/
#include <sys/inotify.h>
#include <limits.h>
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
#include <sys/wait.h>
#include <time.h>
#include <errno.h>


int inotifyFd, wd;

void errExit(char *str) {
	puts(str);
	exit(1);
}
void watchit() {
	wd = inotify_add_watch(inotifyFd, "./data/xidmaat.matn",
		IN_ATTRIB | IN_MODIFY| IN_IGNORED | IN_DELETE | IN_ISDIR );
	if (wd == -1) errExit("inotify_add_watch");

	printf("Watching ./data/xidmaat.matn using wd %d\n", wd);
}

static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    if (i->mask & IN_ISDIR) {
		// ugh
	} else {
		if (i->mask & IN_IGNORED) watchit();
		// changes
	}
}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))


int
main(int argc, char *argv[])
{
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    inotifyFd = inotify_init1(IN_NONBLOCK);                 /* Create inotify instance */
    if (inotifyFd == -1)
        errExit("inotify_init");

    /* For each command-line argument, add a watch for all events */

	watchit();

    for (;;) {                                  /* Read events forever */
		sleep(2);

        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)
            errExit("read() from inotify fd returned 0!");

        /* Process all of the events in buffer returned by read() */

		if (numRead > 0)
        for (p = buf; p < buf + numRead; ) {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}

