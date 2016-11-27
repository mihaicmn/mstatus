#include "config.h"
#include "text.h"

typedef void (*routine_t)(cfg_t *config, struct text_t *text);

void cpu_usage_routine(cfg_t *config, struct text_t *text);
void cpu_load_routine(cfg_t *config, struct text_t *text);
void cpu_temp_routine(cfg_t *config, struct text_t *text);


const char *routine_select_format(cfg_t *config, const double value);

