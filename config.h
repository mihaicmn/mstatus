#include <confuse.h>


void config_load(const char *path);
void config_unload();

cfg_t *config_get_general();

int config_get_block_count();
cfg_t *config_get_block(const int index);

int config_get_subcount(cfg_t *config);
cfg_t *config_get_sub(cfg_t *config, const int index);
