#include "config.h"
#include "text.h"
#include "strings.h"

typedef void (*routine_t)(cfg_t *config, struct text_t *text);

void battery_routine(cfg_t *config, struct text_t *text);
void cpu_usage_routine(cfg_t *config, struct text_t *text);
void cpu_load_routine(cfg_t *config, struct text_t *text);
void cpu_temp_routine(cfg_t *config, struct text_t *text);
void disk_routine(cfg_t *config, struct text_t *text);
void time_routine(cfg_t *config, struct text_t *text);
void volume_routine(cfg_t *config, struct text_t *text);


/* common helpers */

#define FORMAT_WALK(format) char *c; for (c = format; *c != '\0'; c++)
#define FORMAT_CONSUME if (*c != '%') {	text_putc(text, *c); continue; }
#define FORMAT_MATCHES(variable, len) STARTS_WITH(c + 1, variable, len)
#define FORMAT_REPLACE(len, pattern, ...) do { c += len; text_printf(text, pattern, __VA_ARGS__); } while(0)

enum comparison_t {
	ABOVE,
	BELOW
};

void decide_color(cfg_t *config, const double value, enum comparison_t comp, enum color_t *color); 
