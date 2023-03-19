#ifndef _array_h
#define _array_h

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

typedef struct {
	int length;
	int * barzax;
} array_integer;

int array_integer_includes(array_integer * arr, int integer);
int array_integer_pop_member(array_integer * arr, int integer);
int array_integer_pop_at(array_integer * arr, int index);
// returns 0 on success, -1 on error
int array_integer_pop(array_integer * arr);
int array_integer_set(array_integer * arr, int integer);
int array_integer_add(array_integer * arr, int integer);
void array_integer_empty(array_integer * arr);
void array_integer_init(array_integer * arr);

#endif