#include "format.h"

#define GET_STRING(key) cfg_getstr(config, key)
#define GET_THRESHOLD(key) cfg_getfloat(config, key)

#define COMPARE(x, y, comp) ((comp == ABOVE) ? (x > y) : (x < y))


inline const char *format_load(cfg_t *config, const char *fmtkey) {
	const char *format = GET_STRING(fmtkey);
	if (format == NULL)
		format = GET_STRING("format");
	return format;
}

inline const char *format_load_fallback(cfg_t *config, const char *fmtkey, const char *fallback_fmtkey) {
	const char *format = GET_STRING(fmtkey);
	if (format == NULL)
		format = GET_STRING(fallback_fmtkey);
	if (format == NULL)
		format = GET_STRING("format");
	return format;
}


inline const char *format_choose_by_threshold(cfg_t *config, const double value, enum comp_t comp, const char *fallback_fmtkey) {
	double threshold;
	const char *format_key;

	if ((threshold = GET_THRESHOLD("threshold_bad")) > 0 && COMPARE(value, threshold, comp))
		format_key = "format_bad";
	else if ((threshold = GET_THRESHOLD("threshold_degraded")) > 0 && COMPARE(value, threshold, comp))
		format_key = "format_degraded";
	else
		format_key = fallback_fmtkey;

	return format_load_fallback(config, format_key, fallback_fmtkey);
}

inline enum color_t color_choose_by_threshold(cfg_t *config, const double value, enum comp_t comp, const enum color_t default_color) {
        double threshold;

        if ((threshold = GET_THRESHOLD("threshold_bad")) > 0 && COMPARE(value, threshold, comp))
                return COLOR_BAD;
        else if ((threshold = GET_THRESHOLD("threshold_degraded")) > 0 && COMPARE(value, threshold, comp))
		return COLOR_DEGRADED;
        else
		return default_color;
}
