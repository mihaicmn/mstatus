#include <stdio.h>
#include <stdarg.h>
#include "util.h"

void die(const char *message, ...) {
	va_list args;
	va_start(args, message);
	vfprintf(stderr, message, args);
	va_end(args);
	exit(1);
}

void* smalloc(size_t size) {
	void *result = malloc(size);
	if (result == NULL)
		die("not enough memory\n");
	return result;
}

void sfree(void *ptr) {
	free(ptr);
	ptr = NULL;
}
