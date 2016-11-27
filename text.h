#include <stdlib.h>


struct text_t {
	char *content;
	size_t capacity;
	char *color;
};

void text_init(struct text_t *text, size_t capacity);
void text_clear(struct text_t *text);

void text_printf(struct text_t *text, const char *format, ...);
