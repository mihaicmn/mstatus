#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


#include "routine.h"
#include "format.h"

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
#define BUFF_SIZE 1024
	int fd, count;
	char buff[BUFF_SIZE], *cursor;
	int charge_design, charge_real, charge_now, rate, voltage;

	if ((fd = open(path, O_RDONLY)) < 0)
		return -1;

	if ((count = read(fd, buff, BUFF_SIZE - 1)) < 0)
		goto close_file;

	buff[count] = '\0';

	for (cursor = buff; cursor - buff < count; cursor++) {
		if (*cursor == '\n')
			continue;

		if (!STARTS_WITH("POWER_SUPPLY_", cursor, 13))
			continue;

		cursor += 13; //consume 'POWER_SUPPLY_' sequence

		if (STARTS_WITH("STATUS=", cursor, 7)) {
			cursor += 7;
			if (STARTS_WITH("Full", cursor, 4)) {
				cursor += 4;
				battery->status = FULL;
			} else if (STARTS_WITH("Charging", cursor, 8)) {
				cursor += 8;
				battery->status = CHARGING;
			} else if (STARTS_WITH("Discharging", cursor, 11)) {
				cursor += 11;
				battery->status = DISCHARGING;
			} else {
				cursor += 7; //FIXME
				battery->status = UNKNOWN;
			}
//		} else if (STARTS_WITH("CHARGE_FULL_DESIGN=", cursor, 19)) {
//			cursor += 19;
//			charge_design = atoi(cursor);
		} else if (STARTS_WITH("CHARGE_FULL=", cursor, 12)) {
			cursor += 12;
			charge_real = atoi(cursor);
		} else if (STARTS_WITH("CHARGE_NOW=", cursor, 11)) {
			cursor += 11;
			charge_now = atoi(cursor);
		} else if (STARTS_WITH("CURRENT_NOW=", cursor, 12)) {
			cursor += 12;
			rate = atoi(cursor);
		} else if (STARTS_WITH("VOLTAGE_NOW=", cursor, 12)) {
			cursor += 12;
			voltage = atoi(cursor);
		}
	}

        battery->percentage = 100 * charge_now / charge_real;
        battery->consumption = (float)rate / 1e12 * voltage;
	switch (battery->status) {
	case CHARGING:
		battery->remaining = 3600 * (charge_real - charge_now) / rate;
		break;
	case DISCHARGING:
		battery->remaining = 3600 * charge_now / rate;
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

	if (get_battery_info(cfg_getstr(config, "path"), &battery) < 0)
		die("could not get battery info\n");

	if (battery.status == DISCHARGING) {
		const char *threshold_type = cfg_getstr(config, "threshold_type");
		if (EQUALS(threshold_type, "percentage"))
			decide(config, battery.percentage, BELOW, &format, &text->color);
		else if (EQUALS(threshold_type, "minutes"))
			decide(config, battery.remaining / 60, BELOW, &format, &text->color);
		else
			die("invalid threshold_type: %s\n", threshold_type);
	} else {
		text->color = COLOR_DEFAULT;
		format = FORMAT_LOAD_DEFAULT;
	}

        FORMAT_WALK(format) {
                FORMAT_CONSUME;
                FORMAT_RESOLVE("status", 6, "%d", battery.status);
                FORMAT_RESOLVE("percentage", 10, "%.00f", battery.percentage);
                FORMAT_RESOLVE("remaining", 9, "%f", battery.remaining);
                FORMAT_RESOLVE("consumption", 11, "%0.02fW", battery.consumption);
        }
}
