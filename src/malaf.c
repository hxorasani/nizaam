#include "malaf.h"

#define XATAA 0

u_char * assign_type(u_char t) {
	u_char *type;
	switch (t) {
		case DT_UNKNOWN		: type = "unknown"	; break;
		case DT_FIFO		: type = "fifo"		; break;
		case DT_CHR			: type = "chr"		; break;
		case DT_DIR			: type = "dir"		; break;
		case DT_BLK			: type = "blk"		; break;
		case DT_REG			: type = "reg"		; break;
		case DT_LNK			: type = "lnk"		; break;
		case DT_SOCK		: type = "sock"		; break;
		case DT_WHT			: type = "wht"		; break;
		default 			: type = "unknown"	; break;
	}
	return helper_stringify("%s", type);
}

int malaf_ilaa_matn( malaf *mlf, u_char **barzax, long *length ) { // mallocs a barzax
	XATAA && printf("malaf_ilaa_matn %ld %p\n", *length, *barzax);
	if (*barzax != NULL) free(*barzax);

	*length = (long) mlf->length + 1;
	XATAA && printf("malaf_ilaa_matn m.length %ld\n", *length);

	*barzax = (u_char *) malloc( *length * sizeof(u_char) );
	if (*barzax == NULL) {
		perror("malaf_ilaa_matn malloc"); return EXIT_FAILURE;
	}

	memcpy(*barzax, mlf->barzax, mlf->length);
	(*barzax)[ mlf->length ] = '\0';

	XATAA && printf("malaf_ilaa_matn done %ld\n", strlen(*barzax) );
	
	return EXIT_SUCCESS;
}
void malaf_destroy(malaf *m) {
	if (m) {
		if (m->fd) {
			close(m->fd); // TODO should we mark it 0 afterwards?
			m->fd = 0;
		}
		if (m->barzax) {
			free(m->barzax);
			m->barzax = NULL;
		}
	}
}
int malaf_hfiz(malaf *m, char *path) {
	if (path != NULL && m != NULL) {
		XATAA && printf("malaf_hfiz %s\n", path);
		m->fd = open(path, O_CREAT | O_WRONLY);
		if (m->fd == -1) {
			printf("malaf_hfiz open %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		if (ftruncate(m->fd, m->length) == -1) {
			printf("malaf_hfiz ftruncate %s\n", strerror(errno)); return EXIT_FAILURE;
		}

		if (write(m->fd, m->barzax, m->length) == -1) {
			printf("malaf_hfiz write (fd prolly closed earlier ?) %s\n", strerror(errno)); return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
int files_list(const char *path, files_list_t *list) {
	/*
	struct dirent {
	   ino_t          d_ino;       // Inode number
	   off_t          d_off;       // Not an offset;
	   unsigned short d_reclen;    // Length of this record
	   unsigned char  d_type;      // Type of file; not supported
									// by all filesystem types
	   char           d_name[256]; // Null-terminated filename
	};
	*/
	struct dirent **namelist;
	int n = scandir(path, &namelist, NULL, NULL);
	if (n == -1) {
		perror("files_list scandir");
		return EXIT_FAILURE;
	}
	
	int count = 0;
	list->total = n;
	list->files = (malaf *) calloc(n, sizeof(malaf));
	while (n--) {
		list->files[count].type = assign_type(namelist[n]->d_type);
		list->files[count].name = helper_stringify("%s", namelist[n]->d_name);
		count++;

		free(namelist[n]);
	}
	free(namelist);

	return EXIT_SUCCESS;
}
void files_list_free(files_list_t *list) {
	while (list->total--) {
		if (list->files[ list->total ].name) // free u_chr *
			free( list->files[ list->total ].name );
		if (list->files[ list->total ].type) // free u_chr *
			free( list->files[ list->total ].type );
	}
	if (list->files) { // free malaf *
		free(list->files);
		list->files = NULL; // hint that it is freed
	}
}
int malaf_init(malaf *m, char *path) {
	// you can omit the path and specify m->length to malloc barzax
	if (path != NULL) {
		m->fd = open(path, O_RDONLY);
		if (m->fd == -1) {
			printf("malaf_init %s %s\n", strerror(errno), path);
			return EXIT_FAILURE;
		}
		
		struct stat s;
		if (fstat(m->fd, &s) == -1) {
			printf("malaf_init %s %p\n", strerror(errno), path);
			return EXIT_FAILURE;
		}
		
		m->length = s.st_size;
		XATAA && printf("malaf_init '%s' size: %ld\n", path, m->length);
		
	}

	/* EXPLAIN
	 * +1 for the nul char, prolly needed to avoid pointer smashing
	 * mbstowcs was reporting invalid read of size 1 without this
	 * it's not counted in length, because it is not need for functions that
	 * also take a length argument
	 * */
	m->barzax = (void *) malloc(m->length * (sizeof(u_char)+1) );
	if (m->barzax == NULL) {
		printf("malaf_init malloc failed! %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (path != NULL) {
		if (read(m->fd, m->barzax, m->length) == -1) {
			printf("malaf_init %s\n", strerror(errno)); return EXIT_FAILURE;
		}
	}

	// nul terminate anyway because already malloc'd
	*(u_char *)(m->barzax + m->length) = '\0';
//	printf( "%d %d\n", *(u_char *)(m->barzax + m->length-1), *(u_char *)(m->barzax + m->length) );

	return EXIT_SUCCESS;
}