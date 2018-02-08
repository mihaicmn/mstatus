#include <stdlib.h>

#include "routine.h"
#include "util.h"

void cpu_load_routine(cfg_t *config, struct text_t *text) {
	double loadavg[3];

	if (getloadavg(loadavg, 3) != 3) {
		text_error(text, "could not get loadavg");
		return;
	}

	text->color = color_load_threshold(config, loadavg[0], ABOVE);
	const char *format = format_load_threshold(config, loadavg[0], ABOVE);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("1min", "%.2f", loadavg[0]);
		FORMAT_RESOLVE("5min", "%.2f", loadavg[1]);
		FORMAT_RESOLVE("15min", "%.2f", loadavg[2]);
		FORMAT_POST_RESOLVE;
	}
}
