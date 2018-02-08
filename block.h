#include "routine.h"

struct item_t {
	struct text_t text;
	routine_t routine;
};

struct group_t {
	void *context;
	group_pre_routine_t pre_routine;
	group_post_routine_t post_routine;

	int sub_count;
	cfg_t **sub_configs;
	struct text_t *sub_texts;
	subroutine_t *sub_routines;
};

enum block_type {
	ITEM,
	GROUP
};

struct block_t {
	cfg_t *config;
	const char *name;
	int interval;

	enum block_type type;
	union {
		struct item_t item;
		struct group_t group;
	} content;
};

typedef void (*render_t)(const struct text_t *text);

void block_init(struct block_t *block, cfg_t *config);
void block_dismiss(struct block_t *block);
void block_reload(struct block_t *block);
void block_render(struct block_t *block, render_t r);
