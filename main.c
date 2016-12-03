#include "config.h"
#include "bar.h"


int main(void) {
	config_load();
	bar_init();
	bar_loop();
	bar_dismiss();
	config_unload();
	return 0;
}
