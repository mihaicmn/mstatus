#include <string.h>

#include "config.h"
#include "util.h"

#define CFG_INTERVAL 						\
	CFG_INT("interval", 2, CFGF_NONE)

#define CFG_COLOR(name, value) 					\
	CFG_STR(name, value, CFGF_NONE)

#define CFG_THRESHOLD(bad, degraded, common_format)		\
	CFG_FLOAT("threshold_bad", bad, CFGF_NONE),		\
	CFG_FLOAT("threshold_degraded", degraded, CFGF_NONE),	\
	CFG_STR("format", common_format, CFGF_NONE),		\
	CFG_STR("format_bad", common_format, CFGF_NONE),	\
	CFG_STR("format_degraded", common_format, CFGF_NONE)


static cfg_opt_t general_opts[] = {
	CFG_STR("separator", "|", CFGF_NONE),
	CFG_BOOL("colors", cfg_true, CFGF_NONE),
	CFG_STR("target", "TERMINAL", CFGF_NONE),
	CFG_INTERVAL,
	CFG_COLOR("color_good", "#00FF00"),
	CFG_COLOR("color_degraded", "#FFFF00"),
	CFG_COLOR("color_bad", "#FF0000"),
	CFG_END()
};

static cfg_opt_t battery_opts[] = {
	CFG_STR("path", "/sys/class/power_supply/BAT0/uevent", CFGF_NONE),
	CFG_THRESHOLD(5, 15, "status %percentage %remaining %consumption"),
	CFG_STR("threshold_type", "percentage" , CFGF_NONE), /* percentage|minutes */
	CFG_INTERVAL,
	CFG_END()
};

static cfg_opt_t cpu_usage_opts[] = {
	CFG_THRESHOLD(95, 90, "%usage"),
	CFG_INTERVAL,
	CFG_END()
};

static cfg_opt_t cpu_load_opts[] = {
	CFG_THRESHOLD(5, 3, "%1min %5min %15min"),
	CFG_INTERVAL,
	CFG_END()
};

static cfg_opt_t cpu_temp_opts[] = {
	CFG_STR("path", NULL, CFGF_NONE),
	CFG_THRESHOLD(75, 60, "%temp"),
	CFG_INTERVAL,
	CFG_END()
};

static cfg_opt_t disk_opts[] = {
	CFG_THRESHOLD(5, 10, "%free %used %total"),
	CFG_STR("threshold_type", "free" , CFGF_NONE), /* free|avail|used */
	CFG_STR("threshold_unit", "%", CFGF_NONE), /* k|M|G|T|% */
	CFG_STR("measurement_system", "jedec", CFGF_NONE), /* metric|iec|jedec  */
	CFG_INTERVAL,
	CFG_END()
};

static cfg_opt_t time_opts[] = { 
	CFG_STR("format", "%F - %T", CFGF_NONE),
	CFG_INTERVAL,
	CFG_END()
};

static cfg_opt_t volume_opts[] = {
	CFG_STR("format", "%volume", CFGF_NONE),
	CFG_STR("format_muted", "%volume", CFGF_NONE),
	CFG_STR("device", "default", CFGF_NONE),
	CFG_STR("mixer", "Master", CFGF_NONE),
	CFG_INT("index", 0, CFGF_NONE),
	CFG_INTERVAL,
	CFG_END()
};

static cfg_opt_t opts[] = {
	CFG_SEC("general",	general_opts,	CFGF_NONE),
	CFG_STR_LIST("items",	"{}",		CFGF_NONE),
	CFG_SEC("battery",	battery_opts,	CFGF_NONE),
	CFG_SEC("cpu_usage",	cpu_usage_opts,	CFGF_NONE),
	CFG_SEC("cpu_load",	cpu_load_opts,	CFGF_NONE),
	CFG_SEC("cpu_temp",	cpu_temp_opts,	CFGF_NONE),
	CFG_SEC("disk",		disk_opts,	CFGF_MULTI | CFGF_TITLE),
	CFG_SEC("time",		time_opts,	CFGF_NONE),
	CFG_SEC("volume",	volume_opts,	CFGF_MULTI | CFGF_TITLE),
	CFG_END()
};

static cfg_t *config;


void config_load(const char *path) {
	config = cfg_init(opts, CFGF_NONE);
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


cfg_t *config_get_general() {
	return cfg_getsec(config, "general");
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
