#include "format.h"

#define GET_THRESHOLD(key) cfg_getfloat(config, key)

#define COMPARE(x, y, comp) ((comp == ABOVE) ? (x > y) : (x < y))


inline void choose_fmt(cfg_t *config, const char *key, const char **format) {
	*format = FORMAT_LOAD(key);
	if (*format == NULL)
		*format = FORMAT_LOAD("format");
}

inline void choose_fmt_fallback(cfg_t *config, const char *key, const char *fallback_key, const char **format) {
	*format = FORMAT_LOAD(key);
	if (*format == NULL)
		choose_fmt(config, fallback_key, format);
}

inline void choose_fmtcol_bool(cfg_t *config, const bool value, const char **format, enum color_t *color) {
	choose_fmt(config, value ? "format" : "format_bad", format);
	*color = value ? COLOR_GOOD : COLOR_BAD;
}

inline void choose_fmtcol_fallback_threshold(cfg_t *config, const double value, enum comp_t comp, const char *fallback_fmtkey, const char **format, enum color_t *color) {
	double threshold;
	const char *format_key;

	if ((threshold = GET_THRESHOLD("threshold_bad")) > 0 && COMPARE(value, threshold, comp)) {
		format_key = "format_bad";
		*color = COLOR_BAD;
	} else if ((threshold = GET_THRESHOLD("threshold_degraded")) > 0 && COMPARE(value, threshold, comp)) {
		format_key = "format_degraded";
		*color = COLOR_DEGRADED;
	} else {
		format_key = fallback_fmtkey;
		*color = COLOR_DEFAULT;
	}

	choose_fmt_fallback(config, format_key, fallback_fmtkey, format);
}
