#include "routine.h"

#define GET_THRESHOLD(key) cfg_getfloat(config, key)
#define COMPARE(x, y, comp) ((comp == ABOVE) ? (x > y) : (x < y))

void choose_fmtcol(cfg_t *config, const double value, enum comp_t comp, const char *fallback_fmtkey, const char **format, enum color_t *color) {
	double threshold;

	if ((threshold = GET_THRESHOLD("threshold_bad")) > 0 && COMPARE(value, threshold, comp)) {
		*format = FORMAT_LOAD("format_bad");
		*color = COLOR_BAD;
	} else if ((threshold = GET_THRESHOLD("threshold_degraded")) > 0 && COMPARE(value, threshold, comp)) {
		*format = FORMAT_LOAD("format_degraded");
		*color = COLOR_DEGRADED;
	} else {
		*format = NULL;
		*color = COLOR_DEFAULT;
	}

	if (*format == NULL)
		*format = FORMAT_LOAD(fallback_fmtkey);

	if (*format == NULL)
		*format = FORMAT_LOAD_DEFAULT;
}
