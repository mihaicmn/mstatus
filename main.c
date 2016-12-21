#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "bar.h"
#include "config.h"
#include "util.h"

#define SIGACTION(signal) if (sigaction(signal, &action, NULL) < 0) die("cannot register signal handler\n")

static void signal_handler(int signal) {
	switch(signal) {
	case SIGUSR1:
		bar_refresh();
		break;
	default:
		bar_break();
		break;
	}
}

static void register_signals() {
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = &signal_handler;
	SIGACTION(SIGTERM);
	SIGACTION(SIGUSR1);
}

static void select_path(const char **paths, const int len, char *result) {
	int i;
	for (i = 0; i < len; i++) {
		if (paths[i] != NULL && file_expand(paths[i], result) == 0)
			break;
	}
}

int main(int argc, char *argv[]) {
	int option;
	char path[512];
	const char *paths[] = {
		NULL, /* placeholder for user specified config file */
		"$XDG_CONFIG_HOME/mstatus.conf",
		"$XDG_CONFIG_HOME/mstatus/config",
		"$XDG_CONFIG_DIRS/mstatus/config",
		"~/.config/mstatus.conf",
		"~/.config/mstatus/config",
		"~/.mstatus.conf",
		"./mstatus.conf",
		"/etc/mstatus/config"
	};

	while ((option = getopt(argc, argv, "c:v")) > 0) {
		switch (option) {
		case 'c':
			paths[0] = optarg;
			break;
		case 'v':
			printf("mstatus: VERSION\n");
			return 0;
		default:
			printf("mstatus [-c config_file] [-v]\n");
			return 1;
		}
	}

	select_path(paths, 9, path);
	config_load(path);

	bar_init();
	register_signals();
	bar_loop();
	bar_dismiss();
	config_unload();

	return 0;
}
