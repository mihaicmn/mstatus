#include <stdio.h>

#include "output.h"
#include "target.h"
#include "strings.h"
#include "util.h"

#define GET_COLOR(key) cfg_getstr(config, key)

bool use_colors;
bool append_separator;

const char *color_good;
const char *color_degraded;
const char *color_bad;

static target_init_t target_init = NULL;
static target_dismiss_t target_dismiss = NULL;
static target_begin_t target_begin = NULL;
static target_end_t target_end = NULL;
static target_print_t target_print;

void output_init(cfg_t *config) {
	const char *target = cfg_getstr(config, "target");
	if (EQUALS(target, "TERMINAL")) {
		target_init = &terminal_init;
		target_print = &terminal_print;
	} else if (EQUALS(target, "I3BAR")) {
		target_init = &i3bar_init;
		target_dismiss = &i3bar_dismiss;
		target_begin = &i3bar_begin;
		target_end = &i3bar_end;
		target_print = &i3bar_print;
	} else {
		die("invalid target: %s\n", target);
	}

	use_colors = cfg_getbool(config, "colors");
	color_good = GET_COLOR("color_good");
	color_degraded = GET_COLOR("color_degraded");
	color_bad = GET_COLOR("color_bad");

	if (target_init)
		target_init();
}

void output_dismiss() {
	if (target_dismiss)
		target_dismiss();
}

void output_begin() {
	if (target_begin)
		target_begin();
	append_separator = false;
}

void output_print(const struct text_t *text) {
	target_print(text);
	append_separator = true;
}

void output_end() {
	if (target_end)
		target_end();
	printf("\n");
}
