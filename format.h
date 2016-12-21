#include <stdbool.h>

#include "config.h"
#include "strings.h"
#include "text.h"

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


#define FORMAT_LOAD(key) format_load(config, key)

#define FORMAT_LOAD_DEFAULT FORMAT_LOAD("format")

#define CHOOSE_FMTCOL_BYBOOL(value) 					\
	format = format_load(config, value ? "format" : "format_bad");	\
	text->color = value ? COLOR_GOOD : COLOR_BAD

#define CHOOSE_FMTCOL_BYTHRESHOLD(value, comp)						\
	format = format_choose_by_threshold(config, value, comp, "format");		\
	text->color = color_choose_by_threshold(config, value, comp, COLOR_DEFAULT)

#define CHOOSE_FMTCOL_DEFAULT_BYTHRESHOLD(value, comp, def_color)			\
	format = format_choose_by_threshold(config, value, comp, "format");		\
	text->color = color_choose_by_threshold(config, value, comp, def_color)

#define CHOOSE_FMTCOL_FALLBACK_BYTHRESHOLD(value, comp, fallback)			\
	format = format_choose_by_threshold(config, value, comp, fallback);		\
	text->color = color_choose_by_threshold(config, value, comp, COLOR_DEFAULT)

#define CHOOSE_FORMAT_AND_COLOR(key, scolor)	\
	format = FORMAT_LOAD(key);		\
	text->color = scolor

enum comp_t { ABOVE, BELOW };

inline const char *format_load(cfg_t *config, const char *fmtkey);
inline const char *format_load_fallback(cfg_t *config, const char *fmtkey, const char *fallback_fmtkey);

inline const char *format_choose_by_threshold(cfg_t *confg, const double value, enum comp_t comp, const char *fallback_fmtkey);
inline enum color_t color_choose_by_threshold(cfg_t *config, const double value, enum comp_t comp, const enum color_t def_color);
