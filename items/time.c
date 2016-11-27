#include <time.h>

#include "routine.h"

static time_t current;
static struct tm tm;

void time_routine(cfg_t *config, struct text_t *text) {
	time(&current);
	localtime_r(&current, &tm);
	strftime(text->content, text->capacity, cfg_getstr(config, "format"), &tm);
}
