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


int file_scanf(const char *file, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	FILE *f = fopen(file, "r");
	if (f == NULL)
		return -1;
	int result = vfscanf(f, fmt, args);
	fclose(f);
	return result;
}
