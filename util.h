#include <stdlib.h>

void die(const char *message, ...);

void* smalloc(size_t size);
void sfree(void *ptr);

int file_scanf(const char *file, const char *fmt, ...);
