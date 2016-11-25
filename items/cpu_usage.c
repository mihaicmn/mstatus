#include "routine.h"

static int counter = 0;

void cpu_usage_routine(cfg_t *config, struct text_t *text) {
	if (counter++ % 2 == 0)
		text->content = "cpu0";
	else
		text->content = cfg_getstr(config, "format");
}
