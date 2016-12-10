#include <sys/statvfs.h>

#include "routine.h"

#define MAX_UNITS 4

struct disk_t {
	unsigned long free;
	unsigned long avail;
	unsigned long used;
	unsigned long total;
};

struct dsize_t {
	double value;
	const char *unit;
};

enum system_t { METRIC, IEC, JEDEC };

static enum system_t valueof_system(const char *system) {
	if (EQUALS(system, "metric"))
		return METRIC;
	if (EQUALS(system, "iec"))
		return IEC;
	if (EQUALS(system, "jedec"))
		return JEDEC;
	die("invalid measurement_system: %s\n", system);
}

struct magnitude_t {
	const char *const units[MAX_UNITS + 1];
	short base;
};

static struct magnitude_t magnitudes[3] = {
	{ .units = { "", "k", "M", "G", "T" },		.base = 1000 },	/* METRIC */
	{ .units = { "", "ki", "Mi", "Gi", "Ti" },	.base = 1024 },	/* IEC */
	{ .units = { "", "k", "M", "G", "T" },		.base = 1024 }	/* JEDEC */
};


static void convert_bytes_auto(enum system_t system, unsigned long bytes, struct dsize_t *result) {
	int unit_index = 0;
	result->value = (double)bytes;
	while (result->value >= magnitudes[system].base && unit_index++ < MAX_UNITS)
		result->value /= magnitudes[system].base;
	result->unit = magnitudes[system].units[unit_index];
}

static void convert_bytes_certain(enum system_t system, unsigned long bytes, double *result, short unit_index) {
	*result = (double)bytes;
	while (unit_index-- > 0)
		*result /= magnitudes[system].base;
}


static int get_disk_statistics(const char *path, struct disk_t *disk) {
	struct statvfs fs;

	if (statvfs(path, &fs) != 0)
		return -1;

	disk->free = fs.f_bsize * fs.f_bfree;
	disk->avail = fs.f_bsize * fs.f_bavail;
	disk->used = fs.f_bsize * (fs.f_blocks - fs.f_bfree);
	disk->total = fs.f_bsize * fs.f_blocks;

	return 0;
}

void disk_routine(cfg_t *config, struct text_t *text) {
	struct disk_t disk;
	struct dsize_t free, avail, used, total;

	if (get_disk_statistics(cfg_title(config), &disk) != 0)
		die("could not get disk statistics\n");

	const enum system_t system = valueof_system(cfg_getstr(config, "measurement_system"));
	const char *threshold_unit = cfg_getstr(config, "threshold_unit");
	const char *threshold_type = cfg_getstr(config, "threshold_type");

	enum comp_t comparison;
	unsigned long bytes;
	double level;

	if (EQUALS(threshold_type, "free")) {
		comparison = BELOW;
		bytes = disk.free;
	} else if (EQUALS(threshold_type, "avail")) {
		comparison = BELOW;
		bytes = disk.avail;
	} else if (EQUALS(threshold_type, "used")) {
		comparison = ABOVE;
		bytes = disk.used;
	} else {
		die("invalid threshold_type: %s\n", threshold_type);
	}

	if (EQUALS(threshold_unit, "%"))
		level = bytes * disk.total / 100;
	else if (STARTS_WITH(threshold_unit, "k", 1))
		convert_bytes_certain(system, bytes, &level, 1);
	else if (STARTS_WITH(threshold_unit, "M", 1))
		convert_bytes_certain(system, bytes, &level, 2);
	else if (STARTS_WITH(threshold_unit, "G", 1))
		convert_bytes_certain(system, bytes, &level, 3);
	else if (STARTS_WITH(threshold_unit, "T", 1))
		convert_bytes_certain(system, bytes, &level, 4);
	else
		die("invalid threshold_unit: %s\n", threshold_unit);

	const char *format;
	CHOOSE_FMTCOL_BYTHRESHOLD(level, comparison);

	convert_bytes_auto(system, disk.free, &free);
	convert_bytes_auto(system, disk.avail, &avail);
	convert_bytes_auto(system, disk.used, &used);
	convert_bytes_auto(system, disk.total, &total);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("free", 4, "%.1f%s", free.value, free.unit);
		FORMAT_RESOLVE("avail", 5, "%.1f%s", avail.value, avail.unit);
		FORMAT_RESOLVE("used", 4, "%.1f%s", used.value, used.unit);
		FORMAT_RESOLVE("total", 5, "%.1f%s", total.value, total.unit);
		FORMAT_POST_RESOLVE;
	}
}
