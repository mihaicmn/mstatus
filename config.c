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

static cfg_opt_t cpu_load_opts[] = {
	CFG_STR("format", "%1min %5min %15min", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t cpu_temp_opts[] = {
	CFG_STR("format", "%temp", CFGF_NONE),
	CFG_STR("path", NULL, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t disk_opts[] = {
	CFG_STR("format", "%free %used %total", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t time_opts[] = { 
	CFG_STR("format", "%F - %T", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t volume_opts[] = {
	CFG_STR("format", "%volume", CFGF_NONE),
	CFG_STR("device", "default", CFGF_NONE),
	CFG_STR("mixer", "Master", CFGF_NONE),
	CFG_INT("index", 0, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opts[] = {
	CFG_SEC("general",	general_opts,	CFGF_NONE),
	CFG_STR_LIST("items",	"{}",		CFGF_NONE),
	CFG_SEC("cpu_usage",	cpu_usage_opts,	CFGF_NONE),
	CFG_SEC("cpu_load",	cpu_load_opts,	CFGF_NONE),
	CFG_SEC("cpu_temp",	cpu_temp_opts,	CFGF_NONE),
	CFG_SEC("disk",		disk_opts,	CFGF_MULTI | CFGF_TITLE),
	CFG_SEC("time",		time_opts,	CFGF_NONE),
	CFG_SEC("volume",	volume_opts,	CFGF_MULTI | CFGF_TITLE),
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
	char *selector = cfg_getnstr(config, "items", index);
	char *name = NULL;
	char *title = NULL;

	int i;
	for (i = 0; i < strlen(selector); i++) {
		if (selector[i] == ' ') {
			name = strndup(selector, i);
			title = selector + i + 1; /* don't duplicate title sice it already contains trailing '\0' */
			break;
		}
	}
	
	cfg_t *section;

	if (title)
		section = cfg_gettsec(config, name, title);
	else
		section = cfg_getsec(config, selector);
	
	sfree(name);

	return section;
}
