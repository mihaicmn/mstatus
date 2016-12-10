#include "routine.h"


static int get_cpu_temp(const char *path, int *result) {
	if (file_scanf(path, "%ld", result) != 1)
		return -1;
	*result /= 1000;
	return 0;
}

void cpu_temp_routine(cfg_t *config, struct text_t *text) {
	const char *path = cfg_getstr(config, "path");
	if (path == NULL)
		path = "/sys/class/thermal/thermal_zone0/temp";

	int temp;
	if (get_cpu_temp(path, &temp) < 0)
		die("could not get cpu temp\n");

	const char *format;
	CHOOSE_FMTCOL_BYTHRESHOLD(temp, ABOVE);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("temp", 4, "%02d", temp);
		FORMAT_POST_RESOLVE;
	}
}
