#include "config.h"
#include "text.h"

typedef void (*routine_t)(cfg_t *config, struct text_t *text);

void cpu_usage_routine(cfg_t *config, struct text_t *text);
