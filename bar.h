#include "output.h"


struct bar_t {
	struct item_t *items;
	int count;
};

struct bar_t *bar_create();
void bar_loop(struct bar_t *bar);
void bar_destroy(struct bar_t *bar);
