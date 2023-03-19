/* matn
 * 
 * */

#include "matn.h"

#define XATAA 0

void matn_nazaf_str( unsigned char *p, int len ) {
	for (int i = 0; i < len; ++i) {
		p[i] = '\0';
	}
}
void matn_nazaf( matn *m ) {
	if (m->barzax)
	for (int i = 0; i < m->length; ++i) {
		m->barzax[i] = '\0';
	}
}
void matn_destroy(matn *m) {
	if (m) {
		if (m->barzax) {
			free(m->barzax);
			m->barzax = NULL;
		}
	}
}
void matn_adaaf( matn *m, unsigned char *p, int autofree ) { // auto frees *p
	XATAA && printf("matn_adaaf");
	if (m->barzax != NULL) XATAA && printf(" %ld", strlen(m->barzax));
	if (p != NULL) XATAA && printf(" + %ld", strlen(p));

	if (p != NULL) {
//		m->length += strlen(p) + 1;
		
		unsigned char *q = NULL;
		if (m->barzax != NULL) {
			q = helper_stringify("%s%s", m->barzax, p);
		} else {
			q = helper_stringify("%s", p);
		}
		if (q != NULL) {
			if (m->barzax != NULL) free(m->barzax);
			m->barzax = q;
			m->length = strlen(q);
		}
		
//		m->barzax = realloc( m->barzax, m->length );
//		if (m->barzax == NULL) {
//			perror("matn_adaaf calloc"); return;
//		}
//		
//		strncat( m->barzax, p, m->length );
		
		XATAA && printf(" = %ld %s", m->length, p);
		
		if (autofree) free(p);
	}
	XATAA && printf("\n");
}
int matn_iqtabas(matn *s, matn *t, long start, long end) {
	XATAA > 1 && printf("matn_iqtabas\n");

	if (end <= start) {
		XATAA && printf("matn_iqtabas length <= 0\n");
		return EXIT_FAILURE;
	}
	
	t->length = end - start;

	// allocate u_char str, +1 to allow for null char '\0'
	t->barzax = calloc(t->length + 1, sizeof(unsigned char));
	if (t->barzax == NULL) {
		perror("matn_iqtabas calloc"); return EXIT_FAILURE;
	}
	
	for (long i = 0; i < t->length; ++i) {
		t->barzax[i] = s->barzax[start+i];
	}
	t->barzax[t->length-1] = '\0';

	return EXIT_SUCCESS;
}
/* EXPLAIN **
 * c copies *t into a function so it's not mod'd outside of it
 * */
int matn_iqtabas_uc(matn *s, u_char **t, long start, long end) {
	XATAA > 1 && printf("matn_iqtabas\n");

	if (end <= start) {
		XATAA && printf("matn_iqtabas length <= 0\n");
		return EXIT_FAILURE;
	}
	
	long length = end - start;

	// allocate u_char str, +1 to allow for null char '\0'
	u_char *tmp = calloc(length + 1, sizeof(u_char));
	if (tmp == NULL) {
		perror("matn_iqtabas calloc"); return EXIT_FAILURE;
	}
	
	for (long i = 0; i < length; ++i) {
		tmp[i] = s->barzax[start+i];
	}
	tmp[length-1] = '\0';
	
	*t = tmp;

	return EXIT_SUCCESS;
}
void matn_init( matn *m ) {
	m->length = 0;
	m->barzax = NULL;
}
