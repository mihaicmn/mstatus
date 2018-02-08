#include <stdio.h>

#include "target.h"

#define PRINT_JSON_SEPARATOR printf(",")


void i3bar_init() {
	printf("{\"version\": 1}\n"); //print header
	printf("[\n"); 	//begin an infinite array
	printf("[]"); 	//print an empty array to avoid later separator checks
}

void i3bar_dismiss() {
	printf("]\n"); //end the infinite array
}

void i3bar_begin() {
	printf(",[");
}

void i3bar_end() {
	printf("]");
	fflush(stdout);
}

void i3bar_print(const char *text, const char *color, const bool separator) {
	if (separator)
		PRINT_JSON_SEPARATOR;

	printf("{\"full_text\":\"%s\"", text);

	if (color)
		printf(",\"color\":\"%s\"", color);

	printf("}");
}
