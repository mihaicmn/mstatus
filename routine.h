#include "format.h"

typedef void (*routine_t)(cfg_t *config, struct text_t *text);
typedef void (*group_routine_t)(cfg_t *config, void **context);
typedef void (*subroutine_t)(cfg_t *config, void *context, struct text_t *text);

void battery_routine(cfg_t *config, struct text_t *text);
void cpu_usage_routine(cfg_t *config, struct text_t *text);
void cpu_load_routine(cfg_t *config, struct text_t *text);
void cpu_temp_routine(cfg_t *config, struct text_t *text);
void disk_routine(cfg_t *config, struct text_t *text);
void process_routine(cfg_t *config, struct text_t *text);
void time_routine(cfg_t *config, struct text_t *text);
void volume_routine(cfg_t *config, struct text_t *text);

void network_preroutine(cfg_t *config, void **context);
void network_postroutine(cfg_t *config, void **context);

void link_subroutine(cfg_t *config, void *context, struct text_t *text);
void wifi_subroutine(cfg_t *config, void *context, struct text_t *text);
