#include <stdio.h>

#include "item.h"
#include "strings.h"
#include "util.h"

#define TEXT_LENGTH 128

static routine_t get_routine(const char *name) {
	if (STARTS_WITH("cpu_usage", name, 9))
		return &cpu_usage_routine;
	else if (STARTS_WITH("cpu_load", name, 8))
		return &cpu_load_routine;
	else if (STARTS_WITH("cpu_temp", name, 8))
		return &cpu_temp_routine;
	else if (STARTS_WITH("time", name, 8))
		return &time_routine;
	die("routine %s not found\n", name);
}

void item_init(struct item_t *item, cfg_t *config) {
	item->config = config;
	item->routine = get_routine(cfg_name(config));
	text_init(&item->text, TEXT_LENGTH);
}

void item_clear(struct item_t *item) {
	text_clear(&item->text);
}


void item_refresh(struct item_t *item) {
	item->routine(item->config, &item->text);
}

void item_print(struct item_t *item) {
	if (item->text.color)
		printf("(%s)%s ", item->text.color, item->text.content);
	else
		printf("%s ", item->text.content);
}
