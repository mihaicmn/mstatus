#include <stdio.h>

#include "block.h"
#include "output.h" //TODO shouldn't depend on output
#include "strings.h"
#include "util.h"

#define FOR_GROUP int i; for (i = 0; i < group->sub_count; i++)
#define TEXT_LENGTH 128

static inline void item_init(struct item_t *item, const char *name) {
	text_init(&item->text, TEXT_LENGTH);

	if (STARTS_WITH("battery", name))
		item->routine = &battery_routine;
	else if (STARTS_WITH("cpu_usage", name))
		item->routine = &cpu_usage_routine;
	else if (STARTS_WITH("cpu_load", name))
		item->routine = &cpu_load_routine;
	else if (STARTS_WITH("cpu_temp", name))
		item->routine = &cpu_temp_routine;
	else if (STARTS_WITH("disk", name))
		item->routine = &disk_routine;
	else if (STARTS_WITH("process", name))
		item->routine = &process_routine;
	else if (STARTS_WITH("time", name))
		item->routine = &time_routine;
	else if (STARTS_WITH("alsa", name))
		item->routine = &alsa_routine;
	else if (STARTS_WITH("brightness", name))
		item->routine = &brightness_routine;
	else
		die("item_routine \"%s\" not found\n");
}

static inline void group_init(struct group_t *group, cfg_t *config, const char *name) {
	const int sub_count = config_get_subcount(config);
	cfg_t *sub_config;

	if (STARTS_WITH(name, "network")) {
		group->pre_routine = &network_pre_routine;
		group->post_routine = &network_post_routine;
	} else {
		die("invalid group name %s\n", name);
	}

	group->sub_count = sub_count;
	group->sub_configs = smalloc(sub_count * sizeof(cfg_t*));
	group->sub_texts = smalloc(sub_count * sizeof(struct text_t));
	group->sub_routines = smalloc(sub_count * sizeof(subroutine_t));

	FOR_GROUP {
		sub_config = config_get_sub(config, i);
		group->sub_configs[i] = sub_config;
		text_init(group->sub_texts + i, TEXT_LENGTH);

		name = cfg_name(sub_config);

		if (STARTS_WITH("link", name))
			group->sub_routines[i] = &link_subroutine;
		else if (STARTS_WITH("wifi", name))
			group->sub_routines[i] = &wifi_subroutine;
		else
			die("subroutine %s not found\n", name);
	}
}

static inline void item_clear(struct item_t *item) {
	text_dismiss(&item->text);
}

static inline void group_clear(struct group_t *group) {
	FOR_GROUP {
		text_dismiss(group->sub_texts + i);
	}
}

static inline void item_refresh(struct item_t *item, cfg_t *config) {
	text_reset(&item->text);
	item->routine(config, &item->text);
	text_putnull(&item->text);
}

static inline void group_refresh(struct group_t *group, cfg_t *config) {
	group->pre_routine(config, &group->context);

	FOR_GROUP {
		text_reset(group->sub_texts + i);
		group->sub_routines[i](group->sub_configs[i], group->context, group->sub_texts + i);
		text_putnull(group->sub_texts + i);
	}

	group->post_routine(&group->context);
}

static inline void item_print(struct item_t *item) {
	output_print(&item->text);
}

static inline void group_print(struct group_t *group) {
	FOR_GROUP {
		output_print(group->sub_texts + i);
	}
}

void block_init(struct block_t *block, cfg_t *config) {
	block->config = config;
	block->interval = cfg_getint(config, "interval");

	const char *name = cfg_name(config);
	if (STARTS_WITH(name, "network")) {
		block->type = GROUP;
		group_init(&block->content.group, config, name);
	} else {
		block->type = ITEM;
		item_init(&block->content.item, name);
	}
}

void block_clear(struct block_t *block) {
	if (block->type == GROUP)
		group_clear(&block->content.group);
	else
		item_clear(&block->content.item);
}

void block_refresh(struct block_t *block) {
	if (block->type == GROUP)
		group_refresh(&block->content.group, block->config);
	else
		item_refresh(&block->content.item, block->config);
}

void block_print(struct block_t *block) {
	if (block->type == GROUP)
		group_print(&block->content.group);
	else
		item_print(&block->content.item);
}
