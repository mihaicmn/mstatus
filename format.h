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

const char *load_color(cfg_t *config, enum color_t color);
const char *load_format(cfg_t *config, const char *format);

const char *load_color_threshold(cfg_t *config, const double value, enum comp_t comp);
const char *load_format_threshold(cfg_t *config, const double value, enum comp_t comp);
const char *load_format_threshold_fallback(cfg_t *config, const double value, enum comp_t comp, const char *fallback);
