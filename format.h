#include <stdbool.h>

#include "config.h"
#include "strings.h"
#include "text.h"


#define FORMAT_LOAD(key) cfg_getstr(config, key)
#define FORMAT_LOAD_DEFAULT cfg_getstr(config, "format")

#define CHOOSE_FORMAT(key) choose_fmt(config, key, &format)
#define CHOOSE_FORMAT_FALLBACK(key, fallback) choose_fmt(config, key, fallback, &format)
#define CHOOSE_FMTCOL_BYBOOL(value) choose_fmtcol_bool(config, value, &format, &text->color)
#define CHOOSE_FMTCOL_BYTHRESHOLD(value, comp) choose_fmtcol_threshold(config, value, comp, &format, &text->color)
#define CHOOSE_FMTCOL_FALLBACK_BYTHRESHOLD(value, comp, fallback) choose_fmtcol_fallback_threshold(config, value, comp, fallback, &format, &text->color)

#define CHOOSE_FORMAT_AND_COLOR(key, scolor)	\
	CHOOSE_FORMAT(key);			\
	text->color = scolor;


#define FORMAT_WALK(format) const char *c; for (c = format; *c != '\0'; c++)
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
#define FORMAT_POST_RESOLVE text_putc(text, '%')


enum comp_t { ABOVE, BELOW };

void choose_fmt(cfg_t *config, const char *fmtkey, const char **format);
void choose_fmt_fallback(cfg_t *config, const char *fmtkey, const char *fallback_fmtkey, const char **format);

void choose_fmtcol_bool(cfg_t *config, const bool value, const char **format, enum color_t *color);
#define choose_fmtcol_threshold(config, val, comp, fmt, col) choose_fmtcol_fallback_threshold(config, val, comp, "format", fmt, col)
void choose_fmtcol_fallback_threshold(cfg_t *config, const double val, enum comp_t comp, const char *fallback_fmtkey, const char **fmt, enum color_t *col);
