#include <stdio.h>

#include "item.h"


void item_init(struct item_t *item, cfg_t *config) {
	item->config = config;
	item->routine = &cpu_usage_routine;
}

void item_clear(struct item_t *item) {
	//text_clean(&item->text);
}


void item_refresh(struct item_t *item) {
	item->routine(item->config, &item->text);
}

void item_print(struct item_t *item) {
	printf("(%s)%s ", item->text.color, item->text.content);
}
