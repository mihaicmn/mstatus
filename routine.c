#include "routine.h"

#define COMPARE(x, y, comp) ((comp == ABOVE) ? (x > y) : (x < y))

#define FORMAT_LOAD_FALLBACK(key) *format = FORMAT_LOAD(key); if (*format == NULL) *format = FORMAT_LOAD_DEFAULT


void decide(cfg_t *config, const double value, enum comparison_t comp, const char **format, enum color_t *color) {
	double threshold;

	threshold = cfg_getfloat(config, "threshold_bad");
	if (threshold > 0 && COMPARE(value, threshold, comp)) {
		FORMAT_LOAD_FALLBACK("format_bad");
		*color = COLOR_BAD;
		return;
	}

	threshold = cfg_getfloat(config, "threshold_degraded");
	if (threshold > 0 && COMPARE(value, threshold, comp)) {
		FORMAT_LOAD_FALLBACK("format_degraded");
		*color = COLOR_DEGRADED;
		return;
	}

	*format = FORMAT_LOAD_DEFAULT;
	*color = COLOR_DEFAULT;
}
