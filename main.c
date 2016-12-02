#include <stdio.h>

#include "config.h"
#include "bar.h"
#include "util.h"


int main(void) {
	config_load();

	struct bar_t *bar = bar_create();
	bar_loop(bar);
	bar_destroy(bar);

	config_unload();
	return 0;
}
