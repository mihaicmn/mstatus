#include "routine.h"

static unsigned long long int prev_idle = 0;
static unsigned long long int prev_total = 0;


static int get_cpu_usage(double *result) {
	FILE *f = fopen("/proc/stat", "r");

	if (f == NULL)
		return -1;

	/* see htop - ProcessList.c */
	unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal;
	unsigned long long int current_idle, current_total, delta_idle, delta_total;

	if (fscanf(f, "%*s %llu %llu %llu %llu %llu %llu %llu %llu",
		&user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) != 8)
		return -2;

	current_idle = idle + iowait;
	current_total = user + nice + steal + system + irq + softirq + current_idle;

	delta_idle = current_idle - prev_idle;
	delta_total = current_total - prev_total;

	*result = 100 * (double)(delta_total - delta_idle) / delta_total;

	prev_idle = current_idle;
	prev_total = current_total;

	return 0;
}

void cpu_usage_routine(cfg_t *config, struct text_t *text) {
	double usage;
	if (get_cpu_usage(&usage) != 0)
		die("could not read cpu usage\n");
	text_printf(text, "%02.0f", usage);
}
