#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "output.h"
#include "util.h"

static bool append_separator;

static bool use_colors;
static char *color_good;
static char *color_degraded;
static char *color_bad;

static void init_color(cfg_t *cfg, char *key, char **color) {
	char *color_str = cfg_getstr(cfg, key);

	if (strlen(color_str) != 7/*#rrggbb*/ || color_str[0] != '#')
		die("invalid color: %s\n", color_str);

	long color_long = strtol(color_str + 1, NULL, 16);
	if (color_long < 0x0 || color_long > 0xFFFFFF)
		die("invalid rgb format: %s\n", color_str);

	/*
	 * see https://github.com/i3/i3status/blob/master/src/output.c#L31
	 *
	 * The escape-sequence for color is <CSI><col>;1m (bright/bold
	 * output), where col is a 3-bit rgb-value with b in the
	 * least-significant bit. We round the given color to the
	 * nearist 3-bit-depth color and output the escape-sequence */
        int r = (color_long & (0xFF << 0)) / 0x80;
        int g = (color_long & (0xFF << 8)) / 0x8000;
        int b = (color_long & (0xFF << 16)) / 0x800000;

	*color = smalloc( 5 * sizeof(char));
	snprintf(*color, sizeof(*color), "\033[3%d;1m", (r << 2) | (g << 1) | b);
}

static const char *select_color(enum color_t color) {
	switch (color) {
	case COLOR_GOOD:
		return color_good;
	case COLOR_DEGRADED:
		return color_degraded;
	default:
		return color_bad;
	}
}

void output_init(cfg_t *config) {
	use_colors = cfg_getbool(config, "colors");
	init_color(config, "color_good", &color_good);
	init_color(config, "color_degraded", &color_degraded);
	init_color(config, "color_bad", &color_bad);
}

void output_dismiss() {
	sfree(color_good);
	sfree(color_degraded);
	sfree(color_bad);
}

void output_begin() {
	append_separator = false;
}

void output_print(const struct text_t *text) {
	if (append_separator)
		printf(" | ");

	if (use_colors && text->color != COLOR_DEFAULT)
		printf("%s", select_color(text->color));

	printf("%s", text->content);

	if (use_colors && text->color != COLOR_DEFAULT)
		printf("\033[0m");

	append_separator = true;
}

void output_end() {
	printf("\n");
}
