#include "item.h"


struct bar_t {
	struct item_t *items;
	int count;
};

void bar_init(struct bar_t *bar);
void bar_loop(struct bar_t *bar);
void bar_destroy(struct bar_t *bar);
