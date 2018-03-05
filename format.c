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

inline const char *load_color(cfg_t *config, enum color_t colkey) {
	const struct color_mapping_t color_mapping = color_mappings[colkey];
	const char *color = GET_STRING(color_mapping.config_key);
	return color ? color : *color_mapping.global_color;
}

inline const char *load_format(cfg_t *config, const char *fmtkey) {
	const char *format = GET_STRING(fmtkey);
	if (format == NULL)
		format = GET_STRING("format");
	return format;
}

inline const char *load_color_threshold(cfg_t *config, const double value, enum comp_t comp) {
	double threshold;
	enum color_t color;
	if ((threshold = GET_THRESHOLD("threshold_bad")) > 0 && COMPARE(value, threshold, comp))
		color = COLOR_BAD;
	else if ((threshold = GET_THRESHOLD("threshold_degraded")) > 0 && COMPARE(value, threshold, comp))
		color = COLOR_DEGRADED;
	else
		color = COLOR_NORMAL;

	return load_color(config, color);
}

inline const char *load_format_threshold(cfg_t *config, const double value, enum comp_t comp) {
	return load_format_threshold_fallback(config, value, comp, "format");
}

inline const char *load_format_threshold_fallback(cfg_t *config, const double value, enum comp_t comp, const char *fallback) {
	double threshold;
	const char *format;
	if ((threshold = GET_THRESHOLD("threshold_bad")) > 0 && COMPARE(value, threshold, comp)) {
		format = GET_STRING("format_bad");
		if (format != NULL)
			return format;
	} else if ((threshold = GET_THRESHOLD("threshold_degraded")) > 0 && COMPARE(value, threshold, comp)) {
		format = GET_STRING("format_degraded");
		if (format != NULL)
			return format;
	}

	format = GET_STRING(fallback);
	if (format != NULL)
		return format;

	return GET_STRING("format");
}
