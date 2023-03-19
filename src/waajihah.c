/* waajihah: a bridge between umoor
 * listens to madxal for input, maxraj for output
 * 
 * duplex inter process communication, passing shared memory barzaxs.
 * waajihah acts as a bridge between umoor.
 * designed to keep running even when certain key umoor crash.
 * 
 * each amr can send & receive waaqi3aat, some are sent by default
 * extra waaqi3aat can be optionally subscribed
 * 
 * some default waaqi3aat: IXTAF, IZHAR, NABAD, XARAJ, NAFS, ...
 * these waaqi3aat are needed for an amr to complete its basic lifecycle
 * 
 * some optional waaqi3aat: MAQTOOL, MAZHOOR, MAXTOOF, MAXROOJ...
 * passive waaqi3aat about an amr are used by mudeer to manage umoor
 * 
 * an amr can set itself as the mudeer, there can only be one mudeer at a time
 * if one is already running, subsequent requests by others are rejected
 * only a mudeer can send events like QATAL, XARAJ...
 * also a mudeer is notified about umoor dying or connecting so it can keep
 * track of umoor, it can also request a list of all running umoor
 * 
 * TODO
 * * whenever an amr connects tell it who the mudeer & maxraj are (uids)
 * */

#include "waajihah.h"		// header file

#define MAX_EVENTS 10
int XATAA = 0;
#define MAX_UMOOR 1024

int mudeeruid = -1;
struct timespec waqt;
waaqi3ah * head = NULL;
amr umoor[MAX_UMOOR] = { 0 }; // index by amr uid (i.e connection fd)
array_integer umoor_index;

int strequals(char *arg, char *arg2) {
	if ( strcmp(arg, arg2) == 0 ) {
		return 1;
	}
	
	return 0;
}

void tb3_wq3h(waaqi3ah *tmp, amr *a, amr *a2, int fowri) {
	if (fowri)
		XATAA && printf("(%ld) %s ==> (%ld) %s %s.%s.",
			a->uid, a->ism, a2->uid, a2->ism,
			AMR_STRING[tmp->ism], AMR_STRING[tmp->miftaah]
		);
	else
		XATAA && printf("(%ld) %s --> (%ld) %s %s.%s.",
			a->uid, a->ism, a2->uid, a2->ism,
			AMR_STRING[tmp->ism], AMR_STRING[tmp->miftaah]
		);
	if (XATAA)
		if (tmp->qadrstr && strlen(tmp->qadrstr)) printf("%s\n", tmp->qadrstr);
		else printf("%ld\n", tmp->qadr);
}
void irsal_kullahum(waaqi3ah *tmp, amr *a) { // send to all umoor, irsal waaqi3ah li kullihim min amr
	long amr_uid = 0;
	if (tmp->li > 0 && a->uid != tmp->li) { // send to only one
		amr *a2 = &umoor[tmp->li];
		tb3_wq3h(tmp, a, a2, 1);
		if (tmp->qadrstr && strlen(tmp->qadrstr))
		amr_irsal_li_str(tmp->li, tmp->ism, tmp->miftaah, tmp->qadr, tmp->qadrstr, a->uid);
		else
		amr_irsal_li(tmp->li, tmp->ism, tmp->miftaah, tmp->qadr, a->uid);
	} else // send to all
	for (int i = 0; i < umoor_index.length; ++i) {
		amr_uid = umoor_index.barzax[i];
		if (amr_uid != tmp->min) { // dest and source are not the same amr
			amr *a2 = &umoor[amr_uid];
			if (a2->uid > -1) { // dest exists
				if (array_integer_includes(&a2->waaqi3aat, tmp->ism) > -1) {
					tb3_wq3h(tmp, a, a2, 0);
					if (tmp->qadrstr && strlen(tmp->qadrstr))
					amr_irsal_li_str(a2->uid, tmp->ism, tmp->miftaah, tmp->qadr, tmp->qadrstr, a->uid);
					else
					amr_irsal_li(a2->uid, tmp->ism, tmp->miftaah, tmp->qadr, a->uid);
				}
			}
		}
	}
}
void irsal_waaqi3ah_min(int ism, int miftaah, int qadr, int min) {
	amr a = umoor[min];
	waaqi3ah w = {
		.li = 0,
		.ism = ism,
		.miftaah = miftaah,
		.min = min,
		.qadr = qadr,
	};
	irsal_kullahum(&w, &a);
}
void i3aad(amr *a) { // recap waaqi3aat to a new amr
	if (mudeeruid > -1) {
		XATAA && printf("(%ld) i3aad\n", a->uid);
		XATAA && printf("(%d) is the mudeer\n", mudeeruid);
		amr_irsal_li(a->uid, MUDEER, BADAA, mudeeruid, mudeeruid);
		XATAA && printf("\n");
	}
}
void i3aadlilmudeer() { // recap waaqi3aat to the new mudeer
	if (mudeeruid > -1) {
		XATAA && printf("(%d) i3aadlilmudeer\n", mudeeruid);

		int amr_uid;
		for (int i = 0; i < umoor_index.length; ++i) {
			amr_uid = umoor_index.barzax[i];
			amr *a = &umoor[amr_uid];
			if (a->uid > -1) { // source exists
				if (mudeeruid > -1) {
					amr_irsal_li_str(mudeeruid, NAFS, ISM, -1, a->ism, a->uid);
				}
			}
		}

		XATAA && printf("\n");
	}
}
int waajihah_waaqi3aat() {
	// go through waaqi3aat, send them out and free them
	XATAA > 3 && printf("---process waaqi3aat---\n");
	waaqi3ah *tmp = head, *followptr; int i = 0, yes = 0;
	while (tmp) {
		yes = 0;
		followptr = tmp;
		if (XATAA > 1) amr_tb3_waaqi3ah(tmp);
		
		amr *a = &umoor[tmp->min];
		if (a->uid > -1 && tmp->li == 0) {
			if (tmp->ism == ISM && tmp->miftaah == MUSHEER && mudeeruid == a->uid) {
				// mudeer is asking for ism of an amr
				int amr_uid;
				for (int i = 0; i < umoor_index.length; ++i) {
					amr_uid = umoor_index.barzax[i];
					amr *tmpa = &umoor[amr_uid];
					if (amr_uid == tmp->qadr) { // is requested amr
						amr_irsal_li_str(mudeeruid, NAFS, ISM, -1, tmpa->ism, tmp->qadr);
					}
				}
			} else
			if (tmp->ism == NAFS) {
				if (tmp->miftaah == MUDEER) {
					if (mudeeruid == -1) {
						XATAA && printf("(%ld) is now the mudeer\n", a->uid);
						mudeeruid = a->uid;
						irsal_waaqi3ah_min(MUDEER, BADAA, mudeeruid, a->uid);
//						i3aadlilmudeer();
					} else {
						XATAA && printf("(%ld) failed to become mudeer\n", a->uid);
					}
				}
				if (tmp->miftaah == ISM) {
					XATAA && printf("(%ld) ism = %s\n", tmp->min, tmp->qadrstr);
					strcpy(a->ism, tmp->qadrstr);
				}
			} else
			if (tmp->ism == HAAJA) {
				if (tmp->qadr == 0) {
					XATAA && printf("(%ld) %s -%s\n",
						tmp->min, a->ism, AMR_STRING[tmp->miftaah]);
					array_integer_pop_member(&a->waaqi3aat, tmp->miftaah);
				} else if (tmp->qadr == 1) {
					XATAA && printf("(%ld) %s +%s\n",
						tmp->min, a->ism, AMR_STRING[tmp->miftaah]);
					array_integer_set(&a->waaqi3aat, tmp->miftaah);
				}
			} else yes = 1;
		} else yes = 1;
		
		if (yes) irsal_kullahum(tmp, a);
		
		tmp = tmp->qaadim;
		free(followptr);
	}
	tmp = NULL;
	head = NULL;
}
int waajihah_fasal(amr * a) { // when client disconnects
	XATAA && printf(">%ld< fasal (%s)\n", a->uid, a->ism);

	// done in ittasal as well just to be sure
	array_integer_empty(&a->waaqi3aat);

	array_integer_pop_member(&umoor_index, a->uid);

	if (a->uid > -1) {
		if (mudeeruid == a->uid) {
			XATAA && printf(">%ld< was the mudeer\n", a->uid);
			irsal_waaqi3ah_min(MUDEER, MOQOOF, mudeeruid, a->uid);
			mudeeruid = -1;
		} else {
			amr_irsal_li(mudeeruid, MOQOOF, UMOOR, a->uid, a->uid);
		}
		close(a->uid);
		a->uid = -1;
	}
}
int waajihah_ittasal(amr * a, long uid) { // when client connects
	XATAA && printf("<%ld> ittasal\n", uid);
	array_integer_set(&umoor_index, uid);

	a->uid = uid;
	strcpy(a->ism, "\0");
	
	// done in fasal as well just to be sure
	array_integer_empty(&a->waaqi3aat);
	
	i3aad(a);
}
int main(int argc, char **argv) {
	// make sure only one waajihah instance runs
	int pid_file = open("./daaim/waajihah.pid", O_CREAT | O_RDWR, 0666);
	int rc = flock(pid_file, LOCK_EX | LOCK_NB);
	if (rc) {
		if (EWOULDBLOCK == errno) {
			printf("already running\n");
			return EXIT_FAILURE; // another instance is running
		}
	}

	for (int i = 0; i < argc; ++i) {
		char *arg = argv[i];
		if ( strequals(arg, "xataa0") ) XATAA = 0;
		if ( strequals(arg, "xataa") || strequals(arg, "xataa1") ) XATAA = 1;
		if ( strequals(arg, "xataa2") ) XATAA = 2;
		if ( strequals(arg, "xataa3") ) XATAA = 3;
		if ( strequals(arg, "xataa4") ) XATAA = 4;
		if ( strequals(arg, "xataa5") ) XATAA = 5;
	}
	
	array_integer_init(&umoor_index);
	
	int listen_sock, conn_sock, epollfd, connid = 0;
	char barzax[1024];
	
	epollfd = epoll_create1(0);
	if (epollfd == -1) { perror("epoll_create1"); exit(EXIT_FAILURE); }
	make_socket_non_blocking(epollfd);
	struct epoll_event ev, events[MAX_EVENTS];
	ev.events = EPOLLIN | EPOLLET;

	struct sockaddr_un sa = { AF_UNIX, SOCKNAME };
	unlink(SOCKNAME);
	listen_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	bind(listen_sock, (struct sockaddr *)&sa, sizeof sa);
	listen(listen_sock, 5);

	ev.data.fd = listen_sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
		perror("epoll_ctl: listen_sock");
		exit(EXIT_FAILURE);
	}
	printf("%s %d %s%s waajihah listening%s\n", BHWHT, listen_sock, RST, DIM, RST);

	int nfds;
	for (;;) {
		waajihah_waaqi3aat();
		
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1) { perror("epoll_wait"); exit(EXIT_FAILURE); }
	
		for (int n = 0; n < nfds; ++n) {
			int currentfd = events[n].data.fd;
			if (currentfd == listen_sock) {
				struct sockaddr_un addr;
				socklen_t addrlen = sizeof addr;

				conn_sock = accept(listen_sock, (struct sockaddr *) &addr, &addrlen);
				if (conn_sock == -1) {
					perror("accept"); exit(EXIT_FAILURE);
				} else connid++;
				
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
					perror("epoll_ctl: conn_sock"); exit(EXIT_FAILURE);
				} else {
					waajihah_ittasal(&umoor[conn_sock], conn_sock);
				}

				make_socket_non_blocking(conn_sock);
			} else if (events[n].events & EPOLLIN) {
				amr a = umoor[currentfd];
				/* This code will not be triggered repeatedly, so we cycle through
				 * the data to make sure that all the data in the socket read cache
				 * is read out.This is how we eliminate the potential dangers of the ET model */
				
				int reading = 1;
				while (reading) {
					memset(barzax, 0, sizeof barzax);
					int ret = recv(currentfd, barzax, sizeof barzax - 1, 0);
					if (ret < 0) {
						/* For non-congested IO, the following condition is true to
						 * indicate that the data has been read completely, after
						 * which epoll can trigger the EPOLLIN event on currentfd again
						 * to drive the next read operation */
						
						if (errno == EAGAIN || errno == EWOULDBLOCK) {
							XATAA > 3 && printf("read later!\n");
						} else {
							waajihah_fasal(&a);
						}

						reading = 0;
					} else if (ret == 0) { // client disconnected
						waajihah_fasal(&a);
						reading = 0;
					}
					else { // not finished, continue reading in a loop
						XATAA == 3 && printf("%s\n", barzax);
						amr_waaqi3aat(a.uid, barzax, ret, &head);
					}
				}
			}
		}
	}

	return 0;
}
