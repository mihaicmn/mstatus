#include "format.h"

typedef void (*routine_t)(cfg_t *config, struct text_t *text);

void battery_routine(cfg_t *config, struct text_t *text);
void cpu_usage_routine(cfg_t *config, struct text_t *text);
void cpu_load_routine(cfg_t *config, struct text_t *text);
void cpu_temp_routine(cfg_t *config, struct text_t *text);
void disk_routine(cfg_t *config, struct text_t *text);
void process_routine(cfg_t *config, struct text_t *text);
void time_routine(cfg_t *config, struct text_t *text);
void volume_routine(cfg_t *config, struct text_t *text);
