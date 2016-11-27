#include <stdarg.h>

#include "text.h"
#include "util.h"

void text_init(struct text_t *text, size_t capacity) {
	text->content = smalloc(capacity * sizeof(char));
	text->capacity = capacity;
	text->color = NULL;
}

void text_clear(struct text_t *text) {
	sfree(text->content);
	text->capacity = 0;
}

void text_printf(struct text_t *text, const char *format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(text->content, text->capacity, format, args);
	va_end(args);
}
