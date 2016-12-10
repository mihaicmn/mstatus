#include <stdlib.h>

#include "routine.h"

void cpu_load_routine(cfg_t *config, struct text_t *text) {
	double loadavg[3];
	const char *format;

	if (getloadavg(loadavg, 3) != 3)
		die("could not get loadavg\n");

	CHOOSE_FMTCOL_BYTHRESHOLD(loadavg[0], ABOVE);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("1min", 4, "%.2f", loadavg[0]);
		FORMAT_RESOLVE("5min", 4, "%.2f", loadavg[1]);
		FORMAT_RESOLVE("15min", 5, "%.2f", loadavg[2]);
		FORMAT_POST_RESOLVE;
	}
}
