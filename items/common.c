#include "routine.h"

#define COMPARE(x, y, comp) ((comp == ABOVE) ? (x > y) : (x < y))

void decide_color(cfg_t *config, const double value, enum comparison_t comp, enum color_t *color) {
	double threshold;

	threshold = cfg_getfloat(config, "threshold_bad");
	if (threshold > 0 && COMPARE(value, threshold, comp)) {
		*color = COLOR_BAD;
		return;
	}

	threshold = cfg_getfloat(config, "threshold_degraded");
	if (threshold > 0 && COMPARE(value, threshold, comp)) {
		*color = COLOR_DEGRADED;
		return;
	}

	*color = COLOR_DEFAULT;
}
