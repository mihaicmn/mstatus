#include <stdbool.h>
#include <stdio.h>

#include "output.h"


static bool use_separator;

void output_begin() {
	use_separator = false;
}

void output_print(const struct item_t *item) {
	if (use_separator)
		printf(" | ");

	if (item->text.color)
		printf("(%s)%s", item->text.color, item->text.content);
	else
		printf("%s", item->text.content);

	use_separator = true;
}

void output_end() {
	printf("\n");
}

