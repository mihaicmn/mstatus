#include <stdbool.h>

#include "config.h"
#include "strings.h"
#include "text.h"


#define FORMAT_WALK(format)				\
	const char *c; for (c = format; *c != '\0'; c++)

#define FORMAT_PRE_RESOLVE				\
	if (*c != '%') {				\
		text_putc(text, *c);			\
		continue;				\
	}

#define FORMAT_RESOLVE(variable, pattern, ...)	\
	if (STARTS_WITH(variable, c + 1)) {	\
		text_printf(text, pattern, __VA_ARGS__);\
		c += strlen(variable);			\
		continue;				\
	}

#define FORMAT_POST_RESOLVE				\
	text_putc(text, '%')

enum color_t {
	COLOR_NORMAL,
	COLOR_DEGRADED,
	COLOR_BAD
};

enum comp_t {
	ABOVE,
	BELOW
};

const char *color_load(cfg_t *config, enum color_t color);
const char *format_load(cfg_t *config, const char *format);
const char *color_load_threshold(cfg_t *config, const double value, enum comp_t comp);
const char *format_load_threshold(cfg_t *config, const double value, enum comp_t comp);

enum color_t color_by_threshold(cfg_t *config, const double value, enum comp_t comp);
const char *format_by_threshold(cfg_t *config, const double value, enum comp_t comp);
