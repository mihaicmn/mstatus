#include "routine.h"
#include "util.h"

static float get_brightness(const char *actual_brightness_path, const char *max_brightness_path) {
	int actual_brightness, max_brightness;

	if (file_scanf(actual_brightness_path, "%d", &actual_brightness) < 0)
		return -1;
	if (file_scanf(max_brightness_path, "%d", &max_brightness) < 0)
		return -1;

	return ((float) actual_brightness / max_brightness) * 100; 
}

void brightness_routine(cfg_t *config, struct text_t *text) {
	const char *abp = cfg_getstr(config, "actual_brightness_path");
	const char *mbp = cfg_getstr(config, "max_brightness_path");

	const float brightness = get_brightness(abp, mbp);
	if (brightness < 0) {
		text_error(text, "could not get brightness");
		return;
	}

	text->color = load_color(config, COLOR_NORMAL);
	const char *format = load_format(config, "format");

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("brightness", "%02.0f", brightness);
		FORMAT_POST_RESOLVE;
	}
}
