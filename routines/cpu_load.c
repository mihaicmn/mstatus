#include <stdlib.h>

#include "routine.h"

void cpu_load_routine(cfg_t *config, struct text_t *text) {
	double loadavg[3];
	const char *format;

	if (getloadavg(loadavg, 3) != 3)
		die("could not get loadavg\n");

	decide(config, loadavg[0], ABOVE, &format, &text->color);

	FORMAT_WALK(format) {
		FORMAT_CONSUME;
		FORMAT_RESOLVE("1min", 4, "%.2f", loadavg[0]);
		FORMAT_RESOLVE("5min", 4, "%.2f", loadavg[1]);
		FORMAT_RESOLVE("15min", 5, "%.2f", loadavg[2]);
	}
}