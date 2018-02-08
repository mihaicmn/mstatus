#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "routine.h"
#include "util.h"

#define BUFF_SIZE 1024

enum status_t {
	UNKNOWN,
	FULL,
	CHARGING,
	DISCHARGING
};

struct battery_t {
	enum status_t status;
	float percentage;
	float remaining;
	float consumption;
};

static int get_battery_info(const char *path, struct battery_t *battery) {
	int fd, count;
	char buff[BUFF_SIZE], *cursor;
	int charge_full = 0, charge_now = 0;
	int energy_full = 0, energy_now = 0;
	int current_now = 0, power_now = 0;
	int voltage_now = 0;

	if ((fd = open(path, O_RDONLY)) < 0)
		return -1;

	if ((count = read(fd, buff, BUFF_SIZE - 1)) < 0)
		goto close_file;

	buff[count] = '\0';

	for (cursor = buff; cursor - buff < count; cursor++) {
		if (*cursor == '\n')
			continue;

		if (!STARTS_WITH("POWER_SUPPLY_", cursor))
			continue;

		cursor += 13; //consume 'POWER_SUPPLY_' sequence

		if (STARTS_WITH("STATUS=", cursor)) {
			cursor += 7;
			if (STARTS_WITH("Full", cursor)) {
				cursor += 4;
				battery->status = FULL;
			} else if (STARTS_WITH("Charging", cursor)) {
				cursor += 8;
				battery->status = CHARGING;
			} else if (STARTS_WITH("Discharging", cursor)) {
				cursor += 11;
				battery->status = DISCHARGING;
			} else {
				cursor += 7; //FIXME
				battery->status = UNKNOWN;
			}
//		} else if (STARTS_WITH("ENERGY_FULL_DESIGN=", cursor)) {
//			cursor += 19;
//			energy_design = atoi(cursor);
		} else if (STARTS_WITH("ENERGY_FULL=", cursor)) {
			cursor += 12;
			energy_full = atoi(cursor);
		} else if (STARTS_WITH("ENERGY_NOW=", cursor)) {
			cursor += 11;
			energy_now = atoi(cursor);
		} else if (STARTS_WITH("POWER_NOW=", cursor)) {
			cursor += 10;
			power_now = atoi(cursor);
//		} else if (STARTS_WITH("CHARGE_FULL_DESIGN=", cursor)) {
//			cursor += 19;
//			charge_design = atoi(cursor);
		} else if (STARTS_WITH("CHARGE_FULL=", cursor)) {
			cursor += 12;
			charge_full = atoi(cursor);
		} else if (STARTS_WITH("CHARGE_NOW=", cursor)) {
			cursor += 11;
			charge_now = atoi(cursor);
		} else if (STARTS_WITH("CURRENT_NOW=", cursor)) {
			cursor += 12;
			current_now = atoi(cursor);
		} else if (STARTS_WITH("VOLTAGE_NOW=", cursor)) {
			cursor += 12;
			voltage_now = atoi(cursor);
		}
	}

	// if it applies, convert energy (mWh) to charge (mAh)
	if (charge_full == 0)
		charge_full = (float)(1.0 * energy_full / voltage_now * 1e6);

	if (charge_now == 0)
		charge_now = (float)(1.0 * energy_now / voltage_now * 1e6);

	if (current_now == 0)
		current_now = (float)(1.0 * power_now / voltage_now * 1e6);


	// avoid division by zero
	if (charge_full == 0 || current_now == 0 || voltage_now == 0)
		return -2;

        battery->percentage = 100 * charge_now / charge_full;
        battery->consumption = (float)current_now / 1e12 * voltage_now;
	switch (battery->status) {
	case CHARGING:
		battery->remaining = (float)(charge_full - charge_now) / current_now;
		break;
	case DISCHARGING:
		battery->remaining = (float)charge_now / current_now;
		break;
	default:
		battery->remaining = 0;
		break;
	}

	close(fd);
	return 0;

close_file:
	close(fd);
	return -3;
}

void battery_routine(cfg_t *config, struct text_t *text) {
	struct battery_t battery;
	const char *threshold_type;

	if (get_battery_info(cfg_getstr(config, "path"), &battery) < 0) {
		text_error(text, "could not get battery info");
		return;
	}

	enum color_t color = COLOR_NORMAL;
	const char *format = "format";
	switch (battery.status) {
	case FULL:
		format = "format_full";
		break;
	case CHARGING:
		format = "format_charging";
		break;
	case DISCHARGING:
		threshold_type = cfg_getstr(config, "threshold_type");
		if (EQUALS(threshold_type, "percentage")) {
			color = color_by_threshold(config, battery.percentage, BELOW);
			format = format_by_threshold(config, battery.percentage, BELOW);
		} else if (EQUALS(threshold_type, "minutes")) {
			color = color_by_threshold(config, battery.remaining / 60, BELOW);
			format = format_by_threshold(config, battery.remaining / 60, BELOW);
		}
		if (EQUALS(format, "format")) {
			format = "format_discharging";
		}
		break;
	case UNKNOWN:
		format = "format_unknown";
		break;
	}

	text->color = color_load(config, color);
	format = format_load(config, format);

        FORMAT_WALK(format) {
                FORMAT_PRE_RESOLVE;
                FORMAT_RESOLVE("status", "%d", battery.status);
                FORMAT_RESOLVE("percentage", "%.00f", battery.percentage);
                FORMAT_RESOLVE("remaining", "%02d:%02d", (int)battery.remaining, (int)(float)(battery.remaining * 60) / 60);
                FORMAT_RESOLVE("consumption", "%0.02fW", battery.consumption);
		FORMAT_POST_RESOLVE;
        }
}
