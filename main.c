#include <stdio.h>
#include <unistd.h>

#include "bar.h"
#include "config.h"
#include "util.h"


static void select_path(const char **paths, const int len, char *result) {
	int i;
	for (i = 0; i < len; i++) {
		if (paths[i] != NULL && file_expand(paths[i], result) == 0)
			break;
	}
}

int main(int argc, char *argv[]) {
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
	char path[512];
	int option;

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
	bar_loop();
	bar_dismiss();
	config_unload();
	return 0;
}
