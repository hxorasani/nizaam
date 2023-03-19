#ifndef _linkedlist_h
#define _linkedlist_h

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

struct linkedlist_integer {
	int length;
	int * barzax;
};
typedef struct linkedlist_integer linkedlist_integer;

int linkedlist_integer_includes(linkedlist_integer * arr, int integer);
int linkedlist_integer_pop_member(linkedlist_integer * arr, int integer);
int linkedlist_integer_pop_at(linkedlist_integer * arr, int index);
// returns 0 on success, -1 on error
int linkedlist_integer_pop(linkedlist_integer * arr);
int linkedlist_integer_set(linkedlist_integer * arr, int integer);
int linkedlist_integer_add(linkedlist_integer * arr, int integer);
void linkedlist_integer_empty(linkedlist_integer * arr);
void linkedlist_integer_init(linkedlist_integer * arr);

#endif