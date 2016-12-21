#include <sys/statvfs.h>

#include "routine.h"
#include "converter.h"
#include "util.h"

struct disk_t {
	unsigned long free;
	unsigned long avail;
	unsigned long used;
	unsigned long total;
};

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
	struct usize_t free, avail, used, total;

	if (get_disk_statistics(cfg_title(config), &disk) != 0)
		die("could not get disk statistics\n");

	const enum msystem_t msystem = system_valueof(cfg_getstr(config, "measurement_system"));
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
		convert_to(msystem, bytes, 1, &level);
	else if (STARTS_WITH(threshold_unit, "M", 1))
		convert_to(msystem, bytes, 2, &level);
	else if (STARTS_WITH(threshold_unit, "G", 1))
		convert_to(msystem, bytes, 3, &level);
	else if (STARTS_WITH(threshold_unit, "T", 1))
		convert_to(msystem, bytes, 4, &level);
	else
		die("invalid threshold_unit: %s\n", threshold_unit);

	const char *format;
	CHOOSE_FMTCOL_BYTHRESHOLD(level, comparison);

	convert_auto(msystem, disk.free, &free);
	convert_auto(msystem, disk.avail, &avail);
	convert_auto(msystem, disk.used, &used);
	convert_auto(msystem, disk.total, &total);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("free", 4, "%.1f%s", free.value, free.unit);
		FORMAT_RESOLVE("avail", 5, "%.1f%s", avail.value, avail.unit);
		FORMAT_RESOLVE("used", 4, "%.1f%s", used.value, used.unit);
		FORMAT_RESOLVE("total", 5, "%.1f%s", total.value, total.unit);
		FORMAT_POST_RESOLVE;
	}
}
