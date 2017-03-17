#include <errno.h>
#include <signal.h>
#include <stdbool.h>

#include "routine.h"
#include "util.h"

static bool process_runs(const char *pidfile) {
	static char path[512];

	if (file_expand(pidfile, path) != 0)
		return false;

	long pid;
	file_scanf(path, "%ld", &pid);

	return kill(pid, 0) == 0 || errno == EPERM;
}

void process_routine(cfg_t *config, struct text_t *text) {
	bool runs = process_runs(cfg_getstr(config, "pidfile"));
	const char *format;

	SET_FMTCOL(
			runs ? "format" : "format_bad",
			runs ? COLOR_NORMAL : COLOR_BAD);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("title", "%s", cfg_title(config));
		FORMAT_RESOLVE("good", "%s", runs ? "yes" : "no");
		FORMAT_POST_RESOLVE;
	}
}
