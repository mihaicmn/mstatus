#include "routine.h"
#include "util.h"


static int get_cpu_temp(const char *path, int *result) {
	if (file_scanf(path, "%ld", result) != 1)
		return -1;
	*result /= 1000;
	return 0;
}

void cpu_temp_routine(cfg_t *config, struct text_t *text) {
	const char *glob = cfg_getstr(config, "path");

	char path[512];
	if (file_expand(glob, path) != 0)
		die("could not expand glob %s\n", glob);

	int temp;
	if (get_cpu_temp(path, &temp) < 0)
		die("could not get cpu temp\n");

	const char *format;
	SET_FMTCOL_BYTHRESHOLD(temp, ABOVE);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("temp", "%02d", temp);
		FORMAT_POST_RESOLVE;
	}
}
