#include "config.h"
#include "format.h"
#include "text.h"

typedef void (*routine_t)(cfg_t *config, struct text_t *text);

void battery_routine(cfg_t *config, struct text_t *text);
void cpu_usage_routine(cfg_t *config, struct text_t *text);
void cpu_load_routine(cfg_t *config, struct text_t *text);
void cpu_temp_routine(cfg_t *config, struct text_t *text);
void disk_routine(cfg_t *config, struct text_t *text);
void time_routine(cfg_t *config, struct text_t *text);
void volume_routine(cfg_t *config, struct text_t *text);


enum comp_t { ABOVE, BELOW };

#define choose_fmtcol_default(config, val, comp, fmt, col) choose_fmtcol(config, val, comp, "format", fmt, col)
void choose_fmtcol(cfg_t *config, const double val, enum comp_t comp, const char *fallback_fmtkey, const char **fmt, enum color_t *col);
