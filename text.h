#ifndef __TEXT_GUARD__
#define __TEXT_GUARD__

#include <stdlib.h>

struct text_t {
	char *content;
	char *cursor;
	size_t capacity;
	const char *color;
};

void text_init(struct text_t *text, size_t capacity);
void text_dismiss(struct text_t *text);

void text_printf(struct text_t *text, const char *format, ...);
void text_putc(struct text_t *text, const char c);
void text_putnull(struct text_t *text);
void text_reset(struct text_t *text);

#endif
