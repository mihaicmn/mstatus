#include "config.h"
#include "text.h"

typedef void (*routine_t)(cfg_t *config, struct text_t *text);

void battery_routine(cfg_t *config, struct text_t *text);
void cpu_usage_routine(cfg_t *config, struct text_t *text);
void cpu_load_routine(cfg_t *config, struct text_t *text);
void cpu_temp_routine(cfg_t *config, struct text_t *text);
void disk_routine(cfg_t *config, struct text_t *text);
void time_routine(cfg_t *config, struct text_t *text);
void volume_routine(cfg_t *config, struct text_t *text);


/* common helpers */
enum comparison_t {
	ABOVE,
	BELOW
};

void decide_color(cfg_t *config, const double value, enum comparison_t comp, enum color_t *color); 
