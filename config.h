#include <confuse.h>

void config_load();
void config_unload();

const int config_get_item_count();
cfg_t *config_get_item(const int index);
