#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "bar.h"
#include "config.h"
#include "util.h"

static void signal_handler(int signal) {
	switch(signal) {
	case SIGUSR1:
		bar_refresh();
		break;
	default:
		bar_kill();
		break;
	}
}

static void register_signals() {
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = &signal_handler;

	if (sigaction(SIGTERM, &action, NULL) < 0)
		die("cannot register SIGTERM handler\n");
	if (sigaction(SIGUSR1, &action, NULL) < 0)
		die("cannot register SIGUSR1 handler\n");
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

	for (int i = 0; i < 9; i++) {
		if (paths[i] == NULL) {
			continue;
		}
		if (file_expand(paths[i], path) != 0) {
			continue;
		}
		if (access(path, R_OK) != -1) {
			break;
		}
	}

	config_load(path);
	register_signals();
	bar_loop();
	config_unload();

	return 0;
}
