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

#define FORMAT_RESOLVE(variable, len, pattern, ...)	\
	if (STARTS_WITH(c + 1, variable, len)) {	\
		text_printf(text, pattern, __VA_ARGS__);\
		c += len;				\
		continue;				\
	}

#define FORMAT_POST_RESOLVE				\
	text_putc(text, '%')


#define SET_FMTCOL(fmtkey, colkey)			\
	format = format_load(config, fmtkey);		\
	text->color = color_load(config, colkey)

#define SET_FMTCOL_BYTHRESHOLD(value, comp)						\
	format = format_choose_by_threshold(config, value, comp, "format");		\
	text->color = color_choose_by_threshold(config, value, comp)

#define SET_FMTCOL_BYTHRESHOLD_FALLBACK(value, comp, fallback)				\
	format = format_choose_by_threshold(config, value, comp, fallback);		\
	text->color = color_choose_by_threshold(config, value, comp)


enum comp_t {
	ABOVE,
	BELOW
};

enum color_t {
	COLOR_NORMAL,
	COLOR_DEGRADED,
	COLOR_BAD
};

const char *color_load(cfg_t *config, enum color_t color);
const char *color_choose_by_threshold(cfg_t *config, const double value, enum comp_t comp);

const char *format_load(cfg_t *config, const char *fmtkey);
const char *format_choose_by_threshold(cfg_t *confg, const double value, enum comp_t comp, const char *fallback_fmtkey);
