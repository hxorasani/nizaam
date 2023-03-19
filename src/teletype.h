#ifndef _teletype_h
#define _teletype_h

struct teletype {
	int console;	// tty0 file descriptor
	int available;	// number of non-opened vt
	int current;	// fd of currently open vt
	int is_vt;		// is tty a vt?
	void (*onacquire)();
	void (*onrelease)();
	int destroyed;
};
void teletype_acq_rel();
void teletype_destroy();
int teletype_init(int argc, char *argv[]);
struct teletype tty;

#endif