#include <stdio.h>
#include <unistd.h>
#include <wordexp.h>

#include "config.h"
#include "bar.h"


static void select_path(const char **paths, const int len, char *result) {
	wordexp_t p;
	const char *path;
	int i, j;

	for (i = 0; i < len; i++) {
		path = paths[i];

		if (path == NULL)
			continue;

		wordexp(path, &p, 0);

		for (j = 0; j < p.we_wordc; j++) {
			if (access(p.we_wordv[j], R_OK) == -1)
				continue;

			strcpy(result, p.we_wordv[j]);
			i = len; //break the second outer loop
			break;
		}

		wordfree(&p);
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
