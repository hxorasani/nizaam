#include "amr.h"

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

waaqi3ah *head = NULL;

int main() {
	int length = 77, min = 0;
	unsigned char buffer[77] = "0|18|0|5\n0|5|0|5\n0|6|0|5\n\n\n0|7|480|5\n0|8|640|5\n0|18|1|5\n\n\n19|9|1|7\n19|9|1|6\n\0";
	printf("%s", buffer);

	int c, fc = -1, col = 0, line = 0, count = 0, ignore = 1;
	int strlimit = 256, str[strlimit];
	int coli = 0; // key column index
	int stri = 0; // key string index
	long key = 0; // key string as long
	
	waaqi3ah *last = head, *w2 = NULL, *prevw = NULL;
	while (last) { if (last->qaadim) last = last->qaadim; else break; }

	for (long i = 0; i < length+1; ++i) {
		if (i == length) c = '|'; // to allow indexing the last column
		else c = buffer[i];

		if (c == '\n' || i == 0 || i == length) {
			if (i == 0) fc = c;
			else if (i < length && fc == -1) fc = buffer[i+1];

			if (fc <= '9' && fc >= '0') {
				printf("new waaqi3ah\n");
				ignore = 0;
				waaqi3ah * lastw = NULL;

				if (w2) { lastw = w2; prevw = w2; }

				w2 = (waaqi3ah *) malloc( sizeof(waaqi3ah) );
				if (w2 == NULL) {
					perror("amr_waaqi3aat malloc"); exit(EXIT_FAILURE);
				}
				// without this, memory gets corrupted & app crashes
				memset(w2, 0, sizeof(waaqi3ah) );
				w2->li = 0;
				w2->min = min;
				w2->qadrstr[0] = '\0';
				
				if (lastw) {
					lastw->qaadim = w2;
				}
				
				if (head == NULL) head = (waaqi3ah *) w2;
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
		else if (c == '|' || c == '\n' || i == length) { // came across a pipe or new line
			// either way
			str[stri] = '\0'; // null terminate string
			// wcstol( const wchar_t *nptr, wchar_t **endptr, int base )
			key = wcstol(str, (wchar_t **) &str[stri], 10);
			printf("col%d, %ld\n", col, key);

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
				col = 0; // back to the first column
				key = -1; // key not read yet
				line++;
			} else
			if (c == '|') { // only when it's a new pipe
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
	
	printf("count: %d\n", count);

	waaqi3ah *tmp = head, *followptr;
	while (tmp) {
		followptr = tmp;
		amr_tb3_waaqi3ah(tmp);
		tmp = tmp->qaadim;
		free(followptr);
	}
}