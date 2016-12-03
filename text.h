#ifndef __TEXT_GUARD__
#define __TEXT_GUARD__

#include <stdlib.h>

enum color_t {
	COLOR_DEFAULT,
	COLOR_GOOD,
	COLOR_DEGRADED,
	COLOR_BAD
};

struct text_t {
	char *content;
	size_t capacity;
	enum color_t color;
};

void text_init(struct text_t *text, size_t capacity);
void text_clear(struct text_t *text);

void text_printf(struct text_t *text, const char *format, ...);

#endif
