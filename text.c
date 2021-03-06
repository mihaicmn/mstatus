#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "text.h"
#include "util.h"

void text_init(struct text_t *text, size_t capacity) {
	text->content = smalloc(capacity * sizeof(char));
	text->content[0] = '\0';
	text->cursor = text->content;
	text->capacity = capacity;
	text->color = NULL;
}

void text_dismiss(struct text_t *text) {
	sfree(text->content);
}

inline void text_printf(struct text_t *text, const char *format, ...) {
	va_list args;
	va_start(args, format);
	text->cursor += vsnprintf(text->cursor, text->capacity, format, args);
	va_end(args);
}

inline void text_putc(struct text_t *text, const char c) {
	*(text->cursor++) = c;
}

inline void text_putnull(struct text_t *text) {
	*(text->cursor) = '\0';
}

inline void text_reset(struct text_t *text) {
	text->cursor = text->content;
}

inline void text_error(struct text_t *text, const char *msg) {
	strncpy(text->cursor, msg, text->capacity);
	text->cursor += strlen(msg); // TODO use min(capacity, strlen(msg))
	text->color = "#FF0000";
}

inline void text_errorf(struct text_t *text, const char *format, ...) {
	va_list args;
	va_start(args, format);
	text->cursor += vsnprintf(text->cursor, text->capacity, format, args);
	va_end(args);
	text->color = "#FF0000";
}
