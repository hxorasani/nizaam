#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	char smth[] = "something\n";
	int fd;
	fd = write(STDOUT_FILENO, &smth, sizeof(smth));
	while (fd > 0) {
		fd = read(STDIN_FILENO, &smth, sizeof(smth));
		printf("read: %s\n", smth);
	}
    exit(EXIT_SUCCESS);
}
