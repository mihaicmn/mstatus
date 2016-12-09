#include <stdio.h>

#include "target.h"

#define PRINT_JSON_SEPARATOR printf(",")

static inline const char *select_color(enum color_t color) {
	switch (color) {
	case COLOR_GOOD:
		return color_good;
	case COLOR_DEGRADED:
		return color_degraded;
	default:
		return color_bad;
	}
}


void i3bar_init() {
	printf("{\"version\": 1}\n"); //print header
	printf("[\n"); 	//begin an infinite array
	printf("[]"); 	//print an empty array to avoid later separator checks
}

void i3bar_dismiss() {
	printf("]\n"); //end the infinite array
}

void i3bar_begin() {
	printf(",[");
}

void i3bar_end() {
	printf("]");
	fflush(stdout);
}

void i3bar_print(const struct text_t *t) {
	if (append_separator)
		PRINT_JSON_SEPARATOR;

	printf("{\"full_text\":\"%s\"", t->content);

	if (use_colors && t->color != COLOR_DEFAULT)
		printf(",\"color\":\"%s\"", select_color(t->color));

	printf("}");
}
