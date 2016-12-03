#include <stdio.h>

#include "item.h"
#include "strings.h"
#include "util.h"

#define TEXT_LENGTH 128

static routine_t get_routine(cfg_t *config) {
	const char *name = cfg_name(config);

	if (STARTS_WITH("battery", name, 7))
		return &battery_routine;
	else if (STARTS_WITH("cpu_usage", name, 9))
		return &cpu_usage_routine;
	else if (STARTS_WITH("cpu_load", name, 8))
		return &cpu_load_routine;
	else if (STARTS_WITH("cpu_temp", name, 8))
		return &cpu_temp_routine;
	else if (STARTS_WITH("disk", name, 4))
		return &disk_routine;
	else if (STARTS_WITH("time", name, 4))
		return &time_routine;
	else if (STARTS_WITH("volume", name, 6))
		return &volume_routine;

	die("routine %s not found\n", name);
}

void item_init(struct item_t *item, cfg_t *config) {
	item->config = config;
	item->routine = get_routine(config);
	item->interval = cfg_getint(config, "interval");
	text_init(&item->text, TEXT_LENGTH);
}

void item_clear(struct item_t *item) {
	text_clear(&item->text);
}

void item_refresh(struct item_t *item) {
	item->routine(item->config, &item->text);
}
