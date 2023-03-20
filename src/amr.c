/* amr: d-ipc+shm with waajihah
 * 
 * TODO logging method should prepend appname
 * 
 * 
 * runs as a client of waajihah
 * speaks its protocol with high-level functions:
 *   init		connect to waajihah
 *   name		set descriptive name of amr on waajihah
 *   send		send a message to waajihah
 *   copy		advertise that you've copied data to waajihah
 *   paste		request copied data to be pasted
 *   primary	request primary space on a maxraj
 * 
 * amr handles these things automatically:
 * maintains a connection with mudeer, handles all basic waaqi3aat
 * 
 * each amr must have a musheer & a raees
 * nizaam does not hide any umoor, when it runs independently
 * except where it might depend on kaafir daemons like pulseaudio, x11...
 * even those are mentioned by mudeer
 * 
 * an amr has a unique ism and it maintains ahwaal (states)
 * a HAAL waaqi3ah is sent to mudeer whenever a haal becomes faa3il (active)
 * 
 * amr://saa3ah.awqaat			(clock.times)
 * amr://saa3ah.munabbah		(clock.alarm)
 * 
 * this ism+ahwaal combination can be used to launch a app in a specific way
 * directly and share it with other nizaam users
 * it can also help preserve a majlis (session) between restarts
 * */

#include "amr.h"

#define MAX_EVENTS 10
#define XATAA 0
int MUDEERUID = -1;
int WIJHAHHAAJA = 0;
wijhah WJHH;

struct epoll_event ev, events[MAX_EVENTS];
int listen_sock, init = 0, epollfd, ret, curpid = 0;
char barzax[1024];
struct sockaddr_un sa = { AF_UNIX, SOCKNAME };

u_char * mafateeh2str(mafateeh m) {
	return helper_stringify("%d%c%d%c%d%c%d%c%d%c%d%c%f%c%f%c%f%c%s%c%s",
				m.ctrl, 130, m.shift, 130, m.meta, 130,
				m.alt, 130, m.key, 130, m.state, 130,
				m.x, 130, m.y, 130, m.w, 130,
				m.huroof, 130, m.ism);
}
void str2mafateeh(unsigned char *barzax, mafateeh *m) {
//	printf("str2mafateeh %s\n", barzax);
	int strlimit = 256;
	unsigned char str[strlimit];
	int coli = 0, col = 0; // key column index
	int stri = 0; // key string index
	float keyf = 0;
	long key = 0, length = strlen(barzax); // key string as long
	unsigned char c;
	
	for (long i = 0; i < length+1; ++i) {
		if (i == length) c = 130; // to allow indexing the last column
		else c = barzax[i];

		if (c == 130 || i == length) { // came across a sep or new line
			// either way
			str[stri] = '\0'; // null terminate string
			// wcstol( const wchar_t *nptr, wchar_t **endptr, int base )
			if (col < 9) {
				if (col >= 6)
				keyf = strtof(str, (char **) &str[stri]);
				else
				key = strtol(str, (char **) &str[stri], 10);

				if (col == 0) m->ctrl	= key;
				if (col == 1) m->shift	= key;
				if (col == 2) m->meta	= key;
				if (col == 3) m->alt	= key;
				if (col == 4) m->key	= key;
				if (col == 5) m->state	= key;
				if (col == 6) m->x		= keyf;
				if (col == 7) m->y		= keyf;
				if (col == 8) m->w		= keyf;
			} else
			if (col == 9) {
				if (str[0] > 0 && str[0] <= 255) {
					int j = 0; unsigned char cc = str[j];
					while (j < 4 && cc != '\0') {
						m->huroof[j] = str[j];
						j++;
					}
				} else {
					m->huroof[0] = '\0';
				}
			} else
			if (col == 7) {
				if (str[0] > 0 && str[0] <= 255) {
					int j = 0; unsigned char cc = str[j];
					while (j < 4 && cc != '\0') {
						m->ism[j] = str[j];
						j++;
					}
				} else {
					m->ism[0] = '\0';
				}
			}

			matn_nazaf_str(str, strlimit); // reset string
			stri = 0; // reset key string index
			coli = 0; // reset key column index

			if (c == 130) { // only when it's a new sep
				col++; // going to the next column
			}
		} else {
			if (stri < strlimit) {
				str[stri] = c;
				stri++;
			}
			coli++;
		}
	}
}

int amr_dhikr(char *name, int size, int *fd) { // create in physical memory
	int res;

	// get shared memory file descriptor (NOT a file)
	*fd = shm_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (*fd == -1) { perror("open"); return 10; }

	// extend shared memory object as by default it's initialized with size 0
	res = ftruncate(*fd, size);
	if (res == -1) { perror("ftruncate"); return 20; }

	return 0;
}
int amr_dhikr_hadhaf(char *name) { // destroy physically
	int fd = shm_unlink(name); // shm_open cleanup
	if (fd == -1) { perror("unlink"); return 100; }

	return 0;
}
int amr_dhikr_axath(int fd, int size, void **addr) { // open in current process's address space
	// map shared memory to process address space
	*addr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (*addr == MAP_FAILED) { perror("mmap"); return 30; }

	return 0;
}
int amr_dhikr_ixttm(void *addr, int size) { // close from current process
	int res = munmap(addr, size); // mmap cleanup
	if (res == -1) { perror("munmap"); return 40; }
	
	return 0;
}

int amr_irsal_li_str(long li, long ism, long miftaah, long qadr, u_char * qadrstr, long min) {
	XATAA > 2 && printf("amr_irsal %ld . %ld . %ld\n", ism, miftaah, qadr);
    char * barzax2;
	// the last column when sent to waajihah means li, otherwise means min
	if (qadrstr) {
		barzax2 = helper_stringify("%ld%c%ld%c%s%c%ld\n",
				ism, 129, miftaah, 129, qadrstr, 129, min);
	} else {
		barzax2 = helper_stringify("%ld%c%ld%c%ld%c%ld\n",
				ism, 129, miftaah, 129, qadr, 129, min);
	}
	write(li, barzax2, strlen(barzax2));
	free(barzax2);
	return 0;
}
// to be used by an amr
int amr_irsal_ilaa(long li, long ism, long miftaah, long qadr) {
	return amr_irsal_li_str(listen_sock, ism, miftaah, qadr, NULL, li);
}
int amr_irsal_ilaa_str(long li, long ism, long miftaah, u_char *qadrstr) {
	return amr_irsal_li_str(listen_sock, ism, miftaah, -1, qadrstr, li);
}
int amr_irsal_li(long li, long ism, long miftaah, long qadr, long min) {
	return amr_irsal_li_str(li, ism, miftaah, qadr, NULL, min);
}
int amr_irsal_str(long ism, long miftaah, unsigned char *qadr) {
	return amr_irsal_li_str(listen_sock, ism, miftaah, -1, qadr, 0);
}
int amr_ism(unsigned char *qadr) {
	return amr_irsal_str(NAFS, ISM, qadr);
}
int amr_irsal(long ism, long miftaah, long qadr) {
	return amr_irsal_li(listen_sock, ism, miftaah, qadr, 0);
}
void amr_tb3_waaqi3ah(waaqi3ah *w) {
	if (w->qadrstr)
	printf("(%ld) %s . %s . %s", w->min, AMR_STRING[w->ism], AMR_STRING[w->miftaah], w->qadrstr);
	else
	printf("(%ld) %s . %s . %ld", w->min, AMR_STRING[w->ism], AMR_STRING[w->miftaah], w->qadr);
	printf(" . <%ld>", w->li);
	printf("\n");
}
void amr_musheer_moqoof() {
	if (WJHH.mshr.xitaab != NULL) {
		int hajm =	WJHH.mshr.mtrx.w *
						WJHH.mshr.mtrx.h * sizeof(int);

		XATAA && printf("amr_musheer_moqoof %p %s %d\n", WJHH.mshr.xitaab, WJHH.mshr.shaxsiyyah, hajm);


		amr_dhikr_ixttm(WJHH.mshr.xitaab, hajm); // close from current process
		WJHH.mshr.xitaab = NULL;

		free(WJHH.mshr.shaxsiyyah);
		WJHH.mshr.shaxsiyyah = NULL;
		
		WJHH.mshr.mtrx.barzax = NULL;
		msfoof_radd_barzax_alif(&WJHH.mshr.mtrx);

		WJHH.mshr.zaahir = 0;
	}
}
void amr_raees_moqoof() {
	if (WJHH.raees.xitaab != NULL) {
		int hajm =	WJHH.raees.mtrx.w *
						WJHH.raees.mtrx.h * sizeof(int);

		XATAA && printf("amr_raees_moqoof %p %s %d\n", WJHH.raees.xitaab, WJHH.raees.shaxsiyyah, hajm);


		amr_dhikr_ixttm(WJHH.raees.xitaab, hajm); // close from current process
		WJHH.raees.xitaab = NULL;

		free(WJHH.raees.shaxsiyyah);
		WJHH.raees.shaxsiyyah = NULL; // safe way to assume if raees is present
		
		WJHH.raees.mtrx.barzax = NULL; // not doing this results in segfault
		msfoof_radd_barzax_alif(&WJHH.raees.mtrx);

		WJHH.raees.zaahir = 0;
	}
}
void amr_musheer_badaa() {
	if (!WIJHAHHAAJA || WJHH.mshr.xitaab != NULL) return;

	int malaf;
	int hajm =	WJHH.mshr.mtrx.size * sizeof(int);

	XATAA && printf("amr_musheer_badaa %p %s %d\n", WJHH.mshr.xitaab, WJHH.mshr.shaxsiyyah, hajm);

	amr_dhikr(WJHH.mshr.shaxsiyyah, hajm, &malaf);
	amr_dhikr_axath(malaf, hajm, &WJHH.mshr.xitaab);

	WJHH.mshr.mutadarrar = 1;
	WJHH.mshr.mtrx.strokew = 1;
	WJHH.mshr.mtrx.smooth = 0;
	
	msfoof_set_barzax(&WJHH.mshr.mtrx, (int *) WJHH.mshr.xitaab);
	msfoof_clear(&WJHH.mshr.mtrx, 0);
	
	msfoof_radd_barzax_alif(&WJHH.mshr.mtrx);
	msfoof_clear_alif(&WJHH.mshr.mtrx, 0);
}
void amr_raees_badaa() {
	if (!WIJHAHHAAJA || WJHH.raees.xitaab != NULL) return;

	msfoof_resize(&WJHH.raees.mtrx, -1, -1);

	int malaf;
	int hajm =	WJHH.raees.mtrx.size * sizeof(int);

	XATAA && printf("amr_raees_badaa %p %s %d\n", WJHH.raees.xitaab, WJHH.raees.shaxsiyyah, hajm);

	amr_dhikr(WJHH.raees.shaxsiyyah, hajm, &malaf);
	amr_dhikr_axath(malaf, hajm, &WJHH.raees.xitaab);

	WJHH.raees.mutadarrar = 1;
	WJHH.raees.mtrx.strokew = 1;
	WJHH.raees.mtrx.smooth = 0;
	
	msfoof_set_barzax(&WJHH.raees.mtrx, (int *) WJHH.raees.xitaab);
	msfoof_clear(&WJHH.raees.mtrx, 0);
	
	msfoof_radd_barzax_alif(&WJHH.raees.mtrx);
	msfoof_clear_alif(&WJHH.raees.mtrx, 0);
}
void amr_mantiq_wijhah(waaqi3ah *w) {
	if (w->ism == MUDEER) {
		if (MUDEERUID > -1 && WJHH.mshr.xitaab == NULL) {
			if (w->miftaah == BADAA || w->miftaah == MST3D) {
				amr_irsal_ilaa(MUDEERUID, MUSHEER, HAAJA, 1); // request a musheer slot from mudeer
				amr_irsal_ilaa(MUDEERUID, RAEES, HAAJA, 1); // request a raees slot from mudeer
			}
		}
		if (w->miftaah == MOQOOF) {
			amr_musheer_moqoof();
			amr_raees_moqoof();
		}
	}
	if (w->ism == MUSHEER) {
		if (w->miftaah == IZHAR) WJHH.mshr.zaahir = 1;
		if (w->miftaah == IXTAF) WJHH.mshr.zaahir = 0;
		if (w->miftaah == NAFS) {
			// available musheer slot
			WJHH.mshr.shaxsiyyah = helper_stringify("musheer%d", w->qadr);
		}
		if (w->miftaah == MST3D) {
			if (w->qadr == 0) WJHH.mshr.zaahir = 0;
			if (w->qadr == 1) amr_musheer_badaa();
		}
		if (w->miftaah == HAJMW) {
			WJHH.mshr.mtrx.w = w->qadr;
			WJHH.mshr.mtrx.h = w->qadr;
			msfoof_resize(&WJHH.mshr.mtrx, -1, -1);
		}
	}
	if (w->ism == RAEES) {
		if (w->miftaah == IZHAR) WJHH.raees.zaahir = 1;
		if (w->miftaah == IXTAF) WJHH.raees.zaahir = 0;
		if (w->miftaah == NAFS) {
			// available musheer slot
			WJHH.raees.shaxsiyyah = helper_stringify("raees%d", w->qadr);
		}
		if (w->miftaah == MST3D) {
			if (w->qadr == 0) WJHH.raees.zaahir = 0;
			if (w->qadr == 1) amr_raees_badaa();
		}
		if (w->miftaah == HAJMW) {
			WJHH.raees.mtrx.w = w->qadr;
		}
		if (w->miftaah == HAJMH) {
			WJHH.raees.mtrx.h = w->qadr;
		}
	}
}
void amr_waaqi3aat(long min, unsigned char * barzax, long length, waaqi3ah **head) {
	XATAA > 3 && printf("amr_waaqi3aat \n%s-------\n", barzax);
	int c, fc = -1, col = 0, line = 0, count = 0, ignore = 1;
	int strlimit = 256, str[strlimit];
	int coli = 0; // key column index
	int stri = 0; // key string index
	long key = 0; // key string as long

	waaqi3ah *last = *head, *w2 = NULL, *prevw = NULL;
	while (last) { if (last->qaadim) last = last->qaadim; else break; }

	for (long i = 0; i < length+1; ++i) {
		if (i == length) c = 129; // to allow indexing the last column
		else c = barzax[i];

		if (c == '\n' || i == 0 || i == length) {
			if (i == 0) fc = c;
			else if (i < length && fc == -1) fc = barzax[i+1];

			if (fc <= '9' && fc >= '0') {
				XATAA > 3 && printf("new waaqi3ah\n");
				ignore = 0;
				waaqi3ah * lastw = NULL;

				if (w2) { lastw = w2; prevw = w2; }

				w2 = (waaqi3ah *) malloc( sizeof(waaqi3ah) );
				if (w2 == NULL) {
					perror("amr_waaqi3aat malloc"); exit(EXIT_FAILURE);
				}
				// without this, memory gets corrupted & app crashes
				memset(w2, 0, sizeof(waaqi3ah) );
				w2->min = min;
				w2->qadrstr[0] = '\0';
				
				if (lastw) {
					lastw->qaadim = w2;
				}
				
				if (*head == NULL) *head = (waaqi3ah *) w2;
				else if (last && w2) {
					last->qaadim = (waaqi3ah *) w2;
					last = last->qaadim;
				}

				count++;
			} else {
				ignore = 1;
			}
			fc = -1; // reset first character
		}
		
		if (ignore && i != length) {}
		else if (c == 129 || c == '\n' || i == length) { // came across a pipe or new line
			// either way
			str[stri] = '\0'; // null terminate string
			// wcstol( const wchar_t *nptr, wchar_t **endptr, int base )
			key = wcstol(str, (wchar_t **) &str[stri], 10);

			// exiting 1st and 2nd column
			if (col == 0) w2->ism = key;
			if (col == 1) w2->miftaah = key;
			if (col == 2) {
				w2->qadr = key;
				for (int j = 0; j < stri; ++j) {
					w2->qadrstr[j] = str[j];
				}
			}
			if (col == 3) { // last column
				if (prevw) { prevw->li = key; prevw = NULL; }
				if (key > 0) w2->li = key; else w2->li = 0;
			}

			utf8_emptystr(str, strlimit); // reset string
			stri = 0; // reset key string index
			coli = 0; // reset key column index

			if (c == 10) { // only when it's a new line
				XATAA > 2 && printf("\n[%d] ", line);
				col = 0; // back to the first column
				key = -1; // key not read yet
				line++;
			} else
			if (c == 129) { // only when it's a new pipe
				col++; // going to the next column
			}
		} else {
			if (stri < strlimit) {
				str[stri] = c;
				stri++;
			}
			coli++;
		}
	}

	XATAA > 3 && printf("count %d\n", count);
}
int amr_axath_fi(int (*onwaaqi3ah)(), int timeout) { // get waaqi3aat with timeout
	XATAA && printf("amr_axath %d\n", timeout);
	waaqi3ah * head = NULL;
	int nfds, running = 1;
	while (running) {
		XATAA > 2 && printf("---process waaqi3aat---\n");
		waaqi3ah *tmp = head, *followptr; int i = 0;
		while (tmp) {
			followptr = tmp;
			XATAA > 1 && printf("%d] (%ld) %s.%s.%ld <%ld>\n", i++,
								tmp->min, AMR_STRING[tmp->ism],
								AMR_STRING[tmp->miftaah], tmp->qadr, tmp->li);
			
			if (tmp->ism == MUDEER && tmp->miftaah == BADAA) MUDEERUID = tmp->qadr;
			if (tmp->ism == MUDEER && tmp->miftaah == MOQOOF) MUDEERUID = -1;
			if (tmp->ism == MUDEER && tmp->miftaah == QATAL) exit(EXIT_SUCCESS);

			// order is important here, MUDEERUID needs to be set before
			if (WIJHAHHAAJA) amr_mantiq_wijhah(tmp);

			if (onwaaqi3ah) {
				int handled = onwaaqi3ah(tmp, NULL);
				if (handled == 0) { // not handled
					if (tmp->ism == MUDEER && tmp->miftaah == QATAL)
						exit(EXIT_SUCCESS);
				}
			}
			
			tmp = tmp->qaadim;
			if (followptr != NULL) {
				free(followptr);
				followptr = NULL;
			}
		}
		tmp = NULL;
		head = NULL;

		XATAA && printf("amr_axath_fi epollfd %d %d\n", epollfd, MAX_EVENTS);
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, timeout);
		if (nfds == -1) { perror("amr_axath_fi epoll_wait"); exit(EXIT_FAILURE); }
	
		for (int n = 0; n < nfds; ++n) {
			int currentfd = events[n].data.fd;
			if (currentfd == listen_sock) {
				int reading = 1;
				while (reading) {
					memset(barzax, 0, sizeof barzax);
					ret = recv(currentfd, barzax, sizeof barzax - 1, 0);
					if (ret < 0) {
						if (errno == EAGAIN || errno == EWOULDBLOCK) {
							XATAA > 2 && printf("read later!\n");
						} else {
							close(currentfd);
						}

						reading = 0;
					} else if (ret == 0) { // client disconnected
						XATAA && printf("(%d) disconnected\n", currentfd);
						close(currentfd);
						running = 0;
						reading = 0;
					}
					else { // not finished, continue reading in a loop
						XATAA > 2 && printf("%s\n", barzax);
						amr_waaqi3aat(0, barzax, ret, &head);
					}
				}
			} else { // custom watched events, we don't assume any format
//				onwaaqi3ah(NULL, currentfd);
			}
		}
	}
}
int amr_axath(int (*onwaaqi3ah)()) { // get waaqi3aat
	amr_axath_fi(onwaaqi3ah, -1);
}
int amr_fasal(amr * a) { // disconnect
	XATAA && printf("amr_fasal\n");
	close(listen_sock);
}
int amr_ittasal() { // connect
	XATAA && printf("amr_ittasal\n");
    ret = connect(listen_sock, (struct sockaddr *)&sa, sizeof sa);
	if (ret == -1) { perror("connect"); exit(EXIT_FAILURE); }
	
	make_socket_non_blocking(listen_sock);
}
int amr_asaas() { // base (configure haajaat)
	amr_irsal(HAAJA, MUDEER, 1); // listen on mudeer haajaat
}
int amr_intazar(int fd) { // watch an fd for events (like x11)
	XATAA && printf("amr_intazar %d\n", fd);
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll_ctl: amr_intazar");
		exit(EXIT_FAILURE);
	}
}
int amr_ibtada() { // init
	if (!init) init = 1; else return EXIT_FAILURE;
	XATAA && printf("amr_ibtada\n");

	curpid = getpid();
	epollfd = epoll_create1(0);
	if (epollfd == -1) { perror("epoll_create1"); exit(EXIT_FAILURE); }
	make_socket_non_blocking(epollfd);
	ev.events = EPOLLIN | EPOLLET;

    listen_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	amr_intazar(listen_sock);

	XATAA > 1 && printf("added main socket to epoll (%d)\n", listen_sock);

    return 0;
}
void amr_wijhah(int n3m) {
	WIJHAHHAAJA = n3m;
	// TODO check status of amr and request
	WJHH.mshr.xitaab = NULL;
	WJHH.raees.xitaab = NULL;
}
void amr_towheed(u_char *ism) { // ibtada+ittasal+ism+asaas
	amr_ibtada();
	amr_ittasal();
	amr_ism(ism);
	amr_asaas();
}