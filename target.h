#include <stdbool.h>

#include "text.h"

extern bool use_colors;
extern const char *color_good;
extern const char *color_degraded;
extern const char *color_bad;
extern bool append_separator;

typedef void (*target_init_t)();
typedef void (*target_dismiss_t)();
typedef void (*target_begin_t)();
typedef void (*target_print_t)(const struct text_t *t);
typedef void (*target_end_t)();


void terminal_print(const struct text_t *t);

void i3bar_init();
void i3bar_dismiss();
void i3bar_begin();
void i3bar_end();
void i3bar_print(const struct text_t *t);
