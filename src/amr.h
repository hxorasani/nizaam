#ifndef _amr_h
#define _amr_h

#include "helper.h"
#include "array.h"
#include "utf8.h"
#include "msfoof.h"
#include "matn.h"

#include <fcntl.h>		// O_*
#include <string.h>
#include <errno.h>		// errno
#include <stdlib.h>		// exit
#include <sys/socket.h>
#include <stdio.h>		// perror printf
#include <sys/mman.h>	// memory management
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>		// close sleep

#define SOCKNAME "/tmp/waajihah"
extern int MUDEERUID;
extern int WIJHAHHAAJA;

#define FOREACH_AMR(AMR)	\
        AMR(MAXRAJ)			\
        AMR(MUDEER)			\
        AMR(DHIKR)			\
        AMR(NAFS)			\
        AMR(ISM)			\
        AMR(MKAANX)			\
        AMR(MKAANY)			\
        AMR(HAJMW)			\
        AMR(HAJMH)			\
        AMR(HAAJA)			\
        AMR(IXTAF)			\
        AMR(IZHAR)			\
        AMR(NABAD)			\
        AMR(XARAJ)			\
        AMR(QATAL)			\
        AMR(INSHA)			\
        AMR(NAQAF)			\
        AMR(RAHN)			\
        AMR(MST3D)			\
        AMR(MUSHEER)		\
        AMR(RAEES)			\
        AMR(RAKKAZ)			\
        AMR(DARAR)			\
        AMR(BADAA)			\
        AMR(MOQOOF)			\
        AMR(UMOOR)			\
        AMR(MADXAL)			\
        AMR(LOWHAH)			\
        AMR(ISHAARAH)		\
        AMR(TABA3)			\
        AMR(LAMSAH)			\

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum AMR_ENUM {
    FOREACH_AMR(GENERATE_ENUM)
};

static const char *AMR_STRING[] = {
    FOREACH_AMR(GENERATE_STRING)
};

// waaqi3aat < 100 are all auto-subscribed, first 1000 are reserved
//MUDEER		manager, register your amr as a manager
//DHIKR			memory, announce you've shared memory
//NAFS			self, anything related to basics of your amr
//ISM			name, set the descriptive name of your amr
//MKAAN			location, maxraj -> madxal, xy coords, offset on display
//HAAJA			want, (un)subscribe to waaqi3aat
//IXTAF			hide, MUSHEER or RAEES msfoof hidden
//IZHAR			show, ... is now visible
//NABAD			pulse, heartbeat to ensure amr is not frozen
//XARAJ			exit, request exit
//QATAL			kill, notify amr will be killed soon
//INSHA			draw, maxraj -> mudeer, start drawing to matrices
//NAQAF			flip, mudeer -> maxraj, flip matrices
//MUSHEER		indicator, msfoof
//RAEES			primary, msfoof
//RAKKAZ		focus
//DARAR			damage, amr -> mudeer
//ISHAARAH 		pointer events from madxal, or setpointer@ from umoor to maxraj
//TABA3 		expands ihaatah

typedef struct waaqi3ah {
	long min,		// from		5 (uid of amr that sent this event)
		li,			// to		-1 (no one)
		ism,		// name		IZHAR
		miftaah,	// key		RAEES
		qadr;		// value	0
	u_char qadrstr[256];	// value	""
	
	struct waaqi3ah * qaadim;
} waaqi3ah;
typedef struct {
	long uid; // connection id (fd)
	long pid; // process id
	u_char ism[256];
	array_integer waaqi3aat; // waaqi3aat subscribed to
} amr;
typedef struct {
	int amr;
	int mutadarrar; // damaged
	int zaahir; // visible
	int matloob; // demanded, can be pending, given later when space avails
	u_char ism[256]; // name of amr
	ihaatah ihth; // darar region
	msfoof mtrx;
	char *shaxsiyyah; // name of musheer like musheer0
	void *xitaab; // shm
} musheer;
typedef struct {
	int mutadarrar; // damaged
	int zaahir; // visible
	int matloob; // demanded
	ihaatah ihth; // darar region
	msfoof mtrx;
	char *shaxsiyyah; // name of raees like raees0
	void *xitaab; // shm
} raees;
typedef struct {
	int shaxsiyyah; // unique id
	int rabt; // shabakah port 3adad
	int amr3dd; // process id
	u_char ism[256]; // name of amr
	musheer mshr;
	raees raees;
} wijhah; // face
extern wijhah WJHH;
typedef struct {
	int type;
	int ctrl, meta, alt, shift, key, state;
	float x, y, w;
	u_char huroof[4];
	u_char ism[4];
} mafateeh;
u_char *mafateeh2str(mafateeh m);
void str2mafateeh(u_char *str, mafateeh *m);

/* 
 * loops over str, each waaqi3ah is indexed in w.qaadim -> w.qaadim -> ...
 * returns 0 on success, -1 on error
 * */ 
void amr_waaqi3aat(long min, u_char *str, long length, waaqi3ah **head);

// debug
void amr_tb3_waaqi3ah(waaqi3ah *w);

// shared memory
int amr_dhikr(char *name, int size, int *fd); // create in physical memory
int amr_dhikr_hadhaf(char *name); // destroy physically
int amr_dhikr_axath(int fd, int size, void **addr); // open in current process's address space
int amr_dhikr_ixttm(void *addr, int size); // close from current process

int amr_axath_fi(int (*onwaaqi3ah)(), int timeout); // get waaqi3aat with timeout
int amr_axath(int (*onwaaqi3ah)()); // receive waaqi3aat
int amr_irsal_li_str(long li, long ism, long miftaah, long qadr, u_char * str, long min);
int amr_irsal_ilaa_str(long li, long ism, long miftaah, u_char *str);
int amr_irsal_ilaa(long li, long ism, long miftaah, long qadr);
int amr_irsal_li(long li, long ism, long miftaah, long qadr, long min); // send waaqi3ah to amr
int amr_irsal_str(long ism, long miftaah, u_char * qadr);
int amr_irsal(long ism, long miftaah, long qadr); // send waaqi3ah
int amr_ism(u_char * qadr); // send waaqi3ah
int amr_ittasal(); // connect to waajihah
int amr_asaas(); // configure asaas haajaat
int amr_intazar(int fd); // watch an fd for events like x11
int amr_ibtada(); // initialize epoll, socket etc
void amr_wijhah(int n3m); // enable or disable wijhah (gui) capability
void amr_towheed(u_char *ism); // ibtada+ittasal+ism+asaas

#endif