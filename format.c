#include "format.h"

#define GET_STRING(key) cfg_getstr(config, key)
#define GET_THRESHOLD(key) cfg_getfloat(config, key)

#define COMPARE(x, y, comp) ((comp == ABOVE) ? (x > y) : (x < y))

extern const char *color_normal;
extern const char *color_degraded;
extern const char *color_bad;

static struct color_mapping_t {
	const char *config_key;
	const char **global_color;
} color_mappings[] = {
	[COLOR_NORMAL] = {
		.config_key = "color_normal",
		.global_color = &color_normal
	},
	[COLOR_DEGRADED] = {
		.config_key = "color_degraded",
		.global_color = &color_degraded
	},
	[COLOR_BAD] = {
		.config_key = "color_bad",
		.global_color = &color_bad
	}
};

inline const char *color_load(cfg_t *config, enum color_t colkey) {
	const struct color_mapping_t color_mapping = color_mappings[colkey];
	const char *color = GET_STRING(color_mapping.config_key);
	return color ? color : *color_mapping.global_color;
}

inline const char *format_load(cfg_t *config, const char *fmtkey) {
	const char *format = GET_STRING(fmtkey);
	if (format == NULL)
		format = GET_STRING("format");
	return format;
}

inline const char *color_load_threshold(cfg_t *config, const double value, enum comp_t comp) {
	return color_load(config, color_by_threshold(config, value, comp));
}

inline const char *format_load_threshold(cfg_t *config, const double value, enum comp_t comp) {
	return format_load(config, format_by_threshold(config, value, comp));
}

inline enum color_t color_by_threshold(cfg_t *config, const double value, enum comp_t comp) {
	double threshold;
	if ((threshold = GET_THRESHOLD("threshold_bad")) > 0 && COMPARE(value, threshold, comp))
		return COLOR_BAD;
	if ((threshold = GET_THRESHOLD("threshold_degraded")) > 0 && COMPARE(value, threshold, comp))
		return COLOR_DEGRADED;
	return COLOR_NORMAL;
}

inline const char *format_by_threshold(cfg_t *config, const double value, enum comp_t comp) {
	double threshold;
	if ((threshold = GET_THRESHOLD("threshold_bad")) > 0 && COMPARE(value, threshold, comp))
		return "format_bad";
	if ((threshold = GET_THRESHOLD("threshold_degraded")) > 0 && COMPARE(value, threshold, comp))
		return "format_degraded";
	return "format";
}
