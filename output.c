#include <stdio.h>

#include "output.h"
#include "target.h"
#include "strings.h"
#include "util.h"

static target_init_t target_init = NULL;
static target_dismiss_t target_dismiss = NULL;
static target_begin_t target_begin = NULL;
static target_end_t target_end = NULL;
static target_print_t target_print;

static bool use_colors;
static bool append_separator;

void output_init(cfg_t *config) {
	const char *target = cfg_getstr(config, "target");
	if (EQUALS(target, "TERMINAL")) {
		target_print = &terminal_print;
		target_end = &terminal_end;
	} else if (EQUALS(target, "I3BAR")) {
		target_init    = &i3bar_init;
		target_dismiss = &i3bar_dismiss;
		target_begin   = &i3bar_begin;
		target_end     = &i3bar_end;
		target_print   = &i3bar_print;
	} else {
		die("invalid target: %s\n", target);
	}

	use_colors     = cfg_getbool(config, "colors");
	color_normal   = cfg_getstr(config, "color_normal");
	color_degraded = cfg_getstr(config, "color_degraded");
	color_bad      = cfg_getstr(config, "color_bad");

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
	target_print(text->content, text->color, append_separator);
	append_separator = true;
}

void output_end() {
	if (target_end)
		target_end();
	append_separator = false;
}
