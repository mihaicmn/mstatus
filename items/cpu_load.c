#include <stdlib.h>

#include "routine.h"
#include "strings.h"

void cpu_load_routine(cfg_t *config, struct text_t *text) {
	double loadavg[3];

	if (getloadavg(loadavg, 3) != 3)
		die("could not get loadavg\n");

	decide_color(config, loadavg[0], ABOVE, &text->color);

	FORMAT_WALK(cfg_getstr(config, "format")) {

		FORMAT_CONSUME;

		if (FORMAT_MATCHES("1min", 4))
			FORMAT_REPLACE(4, "%.2f", loadavg[0]);
		else if (FORMAT_MATCHES("5min", 4))
			FORMAT_REPLACE(4, "%.2f", loadavg[1]);
		else if (FORMAT_MATCHES("15min", 5))
			FORMAT_REPLACE(5, "%.2f", loadavg[2]);
	}
}
