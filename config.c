#include <string.h>

#include "config.h"
#include "util.h"

#define CFG_INTERVAL 						\
	CFG_INT("interval", 0, CFGF_NONE)

#define CFG_COLOR(name, value)					\
	CFG_STR(name, value, CFGF_NONE)

#define CFG_THRESHOLD(bad, degraded, cformat)			\
	CFG_FLOAT("threshold_bad", bad, CFGF_NONE),		\
	CFG_FLOAT("threshold_degraded", degraded, CFGF_NONE),	\
	CFG_STR("format", cformat, CFGF_NONE),			\
	CFG_STR("format_bad", NULL, CFGF_NONE),			\
	CFG_STR("format_degraded", NULL, CFGF_NONE)

#define CFG_UPDOWN(cformat)					\
	CFG_STR("format", cformat, CFGF_NONE),			\
	CFG_STR("format_bad", NULL, CFGF_NONE)

#define CFG_MEASUREMENT_SYSTEM CFG_STR("measurement_system", "jedec", CFGF_NONE) /* metric|iec|jedec  */

static cfg_opt_t general_opts[] = {
	CFG_STR("separator", "|", CFGF_NONE),
	CFG_BOOL("colors", cfg_true, CFGF_NONE),
	CFG_STR("target", "TERMINAL", CFGF_NONE),
	CFG_INT("interval", 2, CFGF_NONE),
	CFG_COLOR("color_good", "#00FF00"),
	CFG_COLOR("color_degraded", "#FFFF00"),
	CFG_COLOR("color_bad", "#FF0000"),
	CFG_END()
};

static cfg_opt_t battery_opts[] = {
	CFG_STR("path", "/sys/class/power_supply/BAT0/uevent", CFGF_NONE),
	CFG_STR("format_charging", NULL, CFGF_NONE),
	CFG_STR("format_discharging", NULL, CFGF_NONE),
	CFG_STR("format_full", NULL, CFGF_NONE),
	CFG_STR("format_unknown", NULL, CFGF_NONE),
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
	CFG_MEASUREMENT_SYSTEM,
	CFG_INTERVAL,
	CFG_END()
};

static cfg_opt_t network_link_opts[] = {
        CFG_STR("format", "%title: %ip4 %ip6 %up", CFGF_NONE), /* link is operational */
        CFG_STR("format_degraded", NULL, CFGF_NONE), /* link is up and not operational */
        CFG_STR("format_bad", NULL, CFGF_NONE), /* link is down */
        CFG_END()
};

static cfg_opt_t network_wifi_opts[] = {
	CFG_STR("format", "%title: %essid %strength%", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t network_opts[] = {
        CFG_STR_LIST("items", "{}", CFGF_NONE),
        CFG_SEC("link", network_link_opts, CFGF_MULTI | CFGF_TITLE),
        CFG_SEC("wifi", network_wifi_opts, CFGF_MULTI | CFGF_TITLE),
	CFG_MEASUREMENT_SYSTEM,
        CFG_INTERVAL,
        CFG_END()
};

static cfg_opt_t process_opts[] = {
	CFG_STR("pidfile", NULL, CFGF_NONE),
	CFG_UPDOWN("%title: %good"),
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
	CFG_SEC("network",	network_opts,	CFGF_NONE),
	CFG_SEC("process",	process_opts,	CFGF_MULTI | CFGF_TITLE),
	CFG_SEC("time",		time_opts,	CFGF_NONE),
	CFG_SEC("volume",	volume_opts,	CFGF_MULTI | CFGF_TITLE),
	CFG_END()
};

static cfg_t *main_config;


void config_load(const char *path) {
	main_config = cfg_init(opts, CFGF_NONE);
	switch (cfg_parse(main_config, path)) {
	case CFG_SUCCESS:
		break;
	default:
		die("cannot parse configuration file\n");	
	}
}

void config_unload() {
	cfg_free(main_config);
	main_config = NULL;
}


cfg_t *config_get_general() {
	return cfg_getsec(main_config, "general");
}

const int config_get_block_count() {
	return config_get_subcount(main_config);
}

cfg_t *config_get_block(const int index) {
	return config_get_sub(main_config, index);
}

const int config_get_subcount(cfg_t *config) {
	return cfg_size(config, "items");
}

cfg_t *config_get_sub(cfg_t *config, const int index) {
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

	if (section == NULL)
		die("section: %s not found\n", name);

	return section;
}
