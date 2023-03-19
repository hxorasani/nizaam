/* array: provides array structs with convenience functions
 * 
 * */

#include "array.h"

int array_integer_includes(array_integer * arr, int integer) {
	for (int i = 0; i < arr->length; ++i) {
		if (arr->barzax[i] == integer)
			return i;
	}
	return -1;
}
int array_integer_pop_member(array_integer * arr, int integer) {
	int index = array_integer_includes(arr, integer);
	if (index > -1) {
		array_integer_pop_at(arr, index);
	}
	return index;
}
int array_integer_pop_at(array_integer * arr, int index) {
	if (index > -1 && index < arr->length) {
		for (int i = index; i < arr->length-1; ++i) {
			arr->barzax[i] = arr->barzax[i+1];
		}
		array_integer_pop(arr);
		return arr->length;
	}
	return -1;
}
int array_integer_pop(array_integer * arr) {
	if (arr->length) {
		arr->length--;
		arr->barzax = realloc(arr->barzax, arr->length * sizeof(int));
		if (arr->barzax == NULL && arr->length) {
			perror("array_integer_pop realloc"); exit(EXIT_FAILURE);
		}
	}
	return arr->length-1;
}
int array_integer_set(array_integer * arr, int integer) {
	if (array_integer_includes(arr, integer) == -1)
		return array_integer_add(arr, integer);

	return -1;
}
int array_integer_add(array_integer * arr, int integer) {
	arr->length++;
	// realloc auto-frees old ptr if memory area is moved
	arr->barzax = realloc(arr->barzax, arr->length * sizeof(int));
	if (arr->barzax == NULL) {
		perror("array_integer_add realloc"); exit(EXIT_FAILURE);
	}
	arr->barzax[arr->length-1] = integer;
	return arr->length;
}
void array_integer_empty(array_integer * arr) {
	arr->length = 0;
	if (arr->length && arr->barzax) {
		free(arr->barzax);
		arr->barzax = NULL;
	}
}
void array_integer_init(array_integer * arr) {
	arr->length = 0;
	arr->barzax = NULL;
}
