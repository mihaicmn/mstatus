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
	int charge_real, charge_now, rate, voltage;

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
//		} else if (STARTS_WITH("CHARGE_FULL_DESIGN=", cursor)) {
//			cursor += 19;
//			charge_design = atoi(cursor);
		} else if (STARTS_WITH("CHARGE_FULL=", cursor)) {
			cursor += 12;
			charge_real = atoi(cursor);
		} else if (STARTS_WITH("CHARGE_NOW=", cursor)) {
			cursor += 11;
			charge_now = atoi(cursor);
		} else if (STARTS_WITH("CURRENT_NOW=", cursor)) {
			cursor += 12;
			rate = atoi(cursor);
		} else if (STARTS_WITH("VOLTAGE_NOW=", cursor)) {
			cursor += 12;
			voltage = atoi(cursor);
		}
	}

        battery->percentage = 100 * charge_now / charge_real;
        battery->consumption = (float)rate / 1e12 * voltage;
	switch (battery->status) {
	case CHARGING:
		battery->remaining = (float)(charge_real - charge_now) / rate;
		break;
	case DISCHARGING:
		battery->remaining = (float)charge_now / rate;
		break;
	default:
		battery->remaining = 0;
		break;
	}

	close(fd);
	return 0;

close_file:
	close(fd);
	return -2;
}

void battery_routine(cfg_t *config, struct text_t *text) {
	struct battery_t battery;
	const char *format;
	const char *threshold_type;

	if (get_battery_info(cfg_getstr(config, "path"), &battery) < 0)
		die("could not get battery info\n");

	switch (battery.status) {
	case FULL:
		SET_FMTCOL("format_full", COLOR_NORMAL);
		break;
	case CHARGING:
		SET_FMTCOL("format_charging", COLOR_NORMAL);
		break;
	case DISCHARGING:
		threshold_type = cfg_getstr(config, "threshold_type");
		if (EQUALS(threshold_type, "percentage")) {
			SET_FMTCOL_BYTHRESHOLD_FALLBACK(battery.percentage, BELOW, "format_discharging");
		} else if (EQUALS(threshold_type, "minutes")) {
			SET_FMTCOL_BYTHRESHOLD_FALLBACK(battery.remaining / 60, BELOW, "format_discharging");
		} else {
			SET_FMTCOL("format_discharging", COLOR_NORMAL);
		}
		break;
	case UNKNOWN:
		SET_FMTCOL("format_unknown", COLOR_NORMAL);
		break;
	}

        FORMAT_WALK(format) {
                FORMAT_PRE_RESOLVE;
                FORMAT_RESOLVE("status", "%d", battery.status);
                FORMAT_RESOLVE("percentage", "%.00f", battery.percentage);
                FORMAT_RESOLVE("remaining", "%02d:%02d", (int)battery.remaining, (int)(float)(battery.remaining * 60) / 60);
                FORMAT_RESOLVE("consumption", "%0.02fW", battery.consumption);
		FORMAT_POST_RESOLVE;
        }
}
