#include <string.h>
#include <unistd.h>

#include "config.h"
#include "util.h"

#define CHECK_AND_RETURN(path) if (access(path, R_OK) != -1) return path


static cfg_opt_t general_opts[] = {
	CFG_INT("interval", 1, CFGF_NONE),
	CFG_BOOL("colors", cfg_true, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t cpu_usage_opts[] = {
	CFG_STR("format", "%usage", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opts[] = {
	CFG_SEC("general", general_opts, CFGF_NONE),
	CFG_STR_LIST("items", "{}", CFGF_NONE),
	CFG_SEC("cpu_usage", cpu_usage_opts, CFGF_NONE),
	CFG_END()
};

static cfg_t *config;


void config_load() {
	config = cfg_init(opts, CFGF_NONE);

	char *path = "mstatus.conf";//FIXME 
	
	switch (cfg_parse(config, path)) {
	case CFG_SUCCESS:
		break;
	default:
		die("cannot parse configuration file\n");	
	}
}

void config_unload() {
	cfg_free(config);
	config = NULL;
}

const int config_get_item_count() {
	return cfg_size(config, "items");
}

cfg_t *config_get_item(const int index) {
	const char *selector = cfg_getnstr(config, "items", index);	
	char *name = NULL;
	char *title = NULL;

	int i;
	for (i = 0; i < strlen(selector); i++) {
		if (selector[i] == ' ') {
			name = strndup(selector, i-1);
			title = strdup(selector + i);
			break;
		}
	}
	
	cfg_t *section;

	if (title)
		section = cfg_gettsec(config, name, title);
	else
		section = cfg_getsec(config, selector);
	
	sfree(name);
	sfree(title);

	return section;
}
