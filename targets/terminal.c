#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "target.h"
#include "util.h"

static void print_color(const char *color) {
	if (strlen(color) != 7/*#rrggbb*/ || color[0] != '#')
		die("invalid color: %s\n", color);

	long color_long = strtol(color + 1, NULL, 16);
	if (color_long < 0x0 || color_long > 0xFFFFFF)
		die("invalid rgb format: %s\n", color);

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

	printf("\033[3%d;1m", (r << 2) | (g << 1) | b);
}

void terminal_print(const char *text, const char *color, const bool separator) {
	if (separator)
		printf(" | ");

	if (color != NULL)
		print_color(color);

	printf("%s", text);

	if (color != NULL)
		printf("\033[0m");
}

void terminal_end() {
	printf("\n");
	fflush(stdout);
}
