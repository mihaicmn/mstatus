#include <stdlib.h>

#include "routine.h"

void cpu_load_routine(cfg_t *config, struct text_t *text) {
	double loadavg[3];

	if (getloadavg(loadavg, 3) != 3)
		die("could not get loadavg\n");

	decide_color(config, loadavg[0], ABOVE, &text->color);
	text_printf(text, "%.2f %.2f %.2f", loadavg[0], loadavg[1], loadavg[2]);
}
