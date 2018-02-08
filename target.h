#include <stdbool.h>

typedef void (*target_init_t)();
typedef void (*target_dismiss_t)();
typedef void (*target_begin_t)();
typedef void (*target_print_t)(const char *text, const char *color, const bool separator);
typedef void (*target_end_t)();


void terminal_print(const char *text, const char *color, const bool separator);
void terminal_end();

void i3bar_init();
void i3bar_dismiss();
void i3bar_begin();
void i3bar_end();
void i3bar_print(const char *text, const char *color, const bool separator);
