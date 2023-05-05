#include "uv.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

static uv_loop_t* uv_loop;
char *buffer = NULL;

uv_fs_t open_req = {0};
uv_fs_t stat_req = {0};
uv_fs_t read_req = {0};
uv_fs_t write_req= {0};
uv_buf_t iov = {0};

void on_read(uv_fs_t *req) {
	if (req->result < 0) {
		fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
	} else if (req->result == 0) {
		uv_fs_t close_req;
		// synchronous
		uv_fs_close(uv_loop, &close_req, open_req.result, NULL);
		uv_fs_req_cleanup(&read_req);
	} else if (req->result > 0) {
		iov.len = req->result;
	}
}
void on_stat(uv_fs_t *req) {
	if (req->result < 0) {
		fprintf(stderr, "Stat error: %s\n", uv_strerror((int)req->result));
	} else {
		uv_stat_t stats = req->statbuf;
		buffer = malloc(stats.st_size);
		if (buffer == NULL) { perror("uv stat malloc"); exit(EXIT_FAILURE); }
		iov = uv_buf_init(buffer, stats.st_size);
		//int uv_fs_read(uv_loop_t* loop, uv_fs_t* req, uv_file file, const uv_buf_t bufs[], unsigned int nbufs, int64_t offset, uv_fs_cb cb);
		uv_fs_read(uv_loop, &read_req, open_req.result, &iov, 1, -1, on_read);
	}
	uv_fs_req_cleanup(&stat_req);
}
void on_write(uv_fs_t *req) {
	if (req->result < 0) {
		fprintf(stderr, "Write error: %s\n", uv_strerror((int)req->result));
	} else {
		uv_fs_read(uv_loop, &read_req, open_req.result, &iov, 1, -1, on_read);
	}
	uv_fs_req_cleanup(&write_req);
}
void on_open(uv_fs_t *req) {
	// The request passed to the callback is the same as the one the call setup
	// function was passed.
	assert(req == &open_req);
	if (req->result >= 0) {
		uv_fs_fstat(uv_loop, &stat_req, req->result, on_stat);
	} else {
		fprintf(stderr, "error opening file: %s\n", uv_strerror((int)req->result));
	}
	uv_fs_req_cleanup(&open_req);
}

void get_file(const char *path) {
	open_req = (uv_fs_t) {0};
	stat_req = (uv_fs_t) {0};
	read_req = (uv_fs_t) {0};
	write_req= (uv_fs_t) {0};
	iov = (uv_buf_t) {0};
	uv_fs_open(uv_loop, &open_req, path, O_RDONLY, 0, on_open);
}

int main(int argc, char **argv) {
	uv_loop = uv_default_loop();
	get_file("TODO.md");
//	get_file("README.md");
	uv_run(uv_loop, UV_RUN_DEFAULT);
	printf("%ld\n", strlen(buffer));
//	if (buffer) free(buffer);

	return 0;
}