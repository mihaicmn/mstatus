#include <time.h>

#include "routine.h"

static time_t current;
static struct tm tm;

void time_routine(cfg_t *config, struct text_t *text) {
	time(&current);
	localtime_r(&current, &tm);

	text->color = color_load(config, COLOR_NORMAL);
	const char *format = format_load(config, "format");

	text->cursor += strftime(text->content, text->capacity, format, &tm);
}
