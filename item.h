#include "routine.h"


struct item_t {
	int interval;
	routine_t routine;
	cfg_t *config;
	struct text_t text;
};

void item_init(struct item_t *item, cfg_t *config);
void item_clear(struct item_t *item);

void item_refresh(struct item_t *item);
void item_print(struct item_t *item);

