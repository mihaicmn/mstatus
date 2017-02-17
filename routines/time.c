#include <time.h>

#include "routine.h"

static time_t current;
static struct tm tm;

void time_routine(cfg_t *config, struct text_t *text) {
	const char *format;
	time(&current);
	localtime_r(&current, &tm);

	CHOOSE_FMTCOL("format", COLOR_NORMAL);

	text->cursor += strftime(text->content, text->capacity, format, &tm);
}
