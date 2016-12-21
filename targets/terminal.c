#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "target.h"
#include "util.h"

#define COLOR_LENGTH 5

static char term_color_good[COLOR_LENGTH];
static char term_color_degraded[COLOR_LENGTH];
static char term_color_bad[COLOR_LENGTH];

static void convert_color(const char *in_color, char *out_color) {
	if (strlen(in_color) != 7/*#rrggbb*/ || in_color[0] != '#')
		die("invalid color: %s\n", in_color);

	long color_long = strtol(in_color + 1, NULL, 16);
	if (color_long < 0x0 || color_long > 0xFFFFFF)
		die("invalid rgb format: %s\n", in_color);

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

	snprintf(out_color, COLOR_LENGTH *sizeof(char), "\033[3%d;1m", (r << 2) | (g << 1) | b);
}

static inline const char *select_color(enum color_t color) {
	switch (color) {
	case COLOR_GOOD:
		return term_color_good;
	case COLOR_DEGRADED:
		return term_color_degraded;
	default:
		return term_color_bad;
	}
}

void terminal_init() {
	convert_color(color_good, term_color_good);
	convert_color(color_degraded, term_color_degraded);
	convert_color(color_bad, term_color_bad);
}

void terminal_print(const struct text_t *text) {
	if (append_separator)
		printf(" | ");

	if (use_colors && text->color != COLOR_DEFAULT)
		printf("%s", select_color(text->color));

	printf("%s", text->content);

	if (use_colors && text->color != COLOR_DEFAULT)
		printf("\033[0m");
}
