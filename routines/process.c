#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

#include "routine.h"
#include "util.h"

static bool process_runs(const char *pidfile) {
	static char path[512];

	if (file_expand(pidfile, path) != 0) {
		return false;
	}

	if (access(path, R_OK) < 0) {
		return false;
	}

	long pid;
	file_scanf(path, "%ld", &pid);

	return kill(pid, 0) == 0 || errno == EPERM;
}

void process_routine(cfg_t *config, struct text_t *text) {
	const bool runs = process_runs(cfg_getstr(config, "pidfile"));

	text->color = color_load(config, runs ? COLOR_NORMAL : COLOR_BAD);
	const char *format = format_load(config, runs ? "format" : "format_bad");

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("title", "%s", cfg_title(config));
		FORMAT_RESOLVE("running", "%s", runs ? "yes" : "no");
		FORMAT_POST_RESOLVE;
	}
}
