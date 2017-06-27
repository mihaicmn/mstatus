#include "format.h"

typedef void (*routine_t)(cfg_t *config, struct text_t *text);
typedef void (*group_pre_routine_t)(cfg_t *config, void **context);
typedef void (*group_post_routine_t)(void **context);
typedef void (*subroutine_t)(cfg_t *config, void *context, struct text_t *text);

void battery_routine(cfg_t *config, struct text_t *text);
void cpu_usage_routine(cfg_t *config, struct text_t *text);
void cpu_load_routine(cfg_t *config, struct text_t *text);
void cpu_temp_routine(cfg_t *config, struct text_t *text);
void disk_routine(cfg_t *config, struct text_t *text);
void process_routine(cfg_t *config, struct text_t *text);
void time_routine(cfg_t *config, struct text_t *text);
void alsa_routine(cfg_t *config, struct text_t *text);
void brightness_routine(cfg_t *config, struct text_t *text);

void network_pre_routine(cfg_t *config, void **context);
void network_post_routine(void **context);

void link_subroutine(cfg_t *config, void *context, struct text_t *text);
void wifi_subroutine(cfg_t *config, void *context, struct text_t *text);
