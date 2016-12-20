#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wordexp.h>
#include <unistd.h>

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


int file_expand(const char *glob, char *path) {
	wordexp_t p;
	int i, result = -1;

	wordexp(glob, &p, 0);
	for (i = 0; i < p.we_wordc; i++) {
		if (access(p.we_wordv[i], R_OK) != -1) {
			strcpy(path, p.we_wordv[i]);
			result = 0;
			break;
		}
	}
	wordfree(&p);
	return result;
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

float interpolate(const float value, const float min, const float max) {
	return (100.0 * (value - min)) / (max - min);
}
