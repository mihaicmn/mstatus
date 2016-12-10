#include <stdlib.h>

void die(const char *message, ...);

void* smalloc(size_t size);
void sfree(void *ptr);

int file_expand(const char *glob, char *path);
int file_scanf(const char *file, const char *fmt, ...);
