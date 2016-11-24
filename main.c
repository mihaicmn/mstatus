#include <stdio.h>

#include "bar.h"
#include "util.h"


int main(void) {
	struct bar_t bar;

	bar.count = 3;
	bar.items = smalloc(bar.count * sizeof(struct item_t));
	bar.items[0].message = "item0";
	bar.items[1].message = "item1";
	bar.items[2].message = "item2";

	bar_init(&bar);
	bar_loop(&bar);
	bar_destroy(&bar);

	return 0;
}
