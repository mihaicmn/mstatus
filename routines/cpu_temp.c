#include "routine.h"
#include "util.h"

static int get_cpu_temp(const char *path, int *result) {
	if (file_scanf(path, "%ld", result) != 1)
		return -1;
	*result /= 1000;
	return 0;
}

void cpu_temp_routine(cfg_t *config, struct text_t *text) {
	char path[512];

	const char *glob = cfg_getstr(config, "path");
	if (file_expand(glob, path) < 0) {
		text_errorf(text, "could not expand glob %s", glob);
		return;
	}

	int temp;
	if (get_cpu_temp(path, &temp) < 0) {
		text_error(text, "could not get cpu temp");
		return;
	}

	text->color = load_color_threshold(config, temp, ABOVE);
	const char *format = load_format_threshold(config, temp, ABOVE);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("temp", "%02d", temp);
		FORMAT_POST_RESOLVE;
	}
}
