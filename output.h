#include "config.h"
#include "text.h"

void output_init(cfg_t *config);
void output_dismiss();

void output_begin();
void output_print(const struct text_t *text);
void output_end();
