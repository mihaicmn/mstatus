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

	const char *path = cfg_title(config);
	if (get_disk_statistics(path, &disk) != 0) {
		text_errorf(text, "could not get disk statistics for %s", path);
		return;
	}

	const enum msystem_t msystem = system_valueof(cfg_getstr(config, "measurement_system"));
	const char *threshold_unit = cfg_getstr(config, "threshold_unit");
	const char *threshold_type = cfg_getstr(config, "threshold_type");

	convert_auto(msystem, disk.free, &free);
	convert_auto(msystem, disk.avail, &avail);
	convert_auto(msystem, disk.used, &used);
	convert_auto(msystem, disk.total, &total);

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
		text_errorf(text, "invalid threshold_type: %s", threshold_type);
		return;
	}

	if (EQUALS(threshold_unit, "%")) {
		level = bytes * disk.total / 100;
	} else if (STARTS_WITH(threshold_unit, "k")) {
		convert_to(msystem, bytes, 1, &level);
	} else if (STARTS_WITH(threshold_unit, "M")) {
		convert_to(msystem, bytes, 2, &level);
	} else if (STARTS_WITH(threshold_unit, "G")) {
		convert_to(msystem, bytes, 3, &level);
	} else if (STARTS_WITH(threshold_unit, "T")) {
		convert_to(msystem, bytes, 4, &level);
	} else {
		text_errorf(text, "invalid threshold_unit: %s", threshold_unit);
		return;
	}

	text->color = load_color_threshold(config, level, comparison);
	const char *format = load_format_threshold(config, level, comparison);

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("free", "%.1f%s", free.value, free.unit);
		FORMAT_RESOLVE("avail", "%.1f%s", avail.value, avail.unit);
		FORMAT_RESOLVE("used", "%.1f%s", used.value, used.unit);
		FORMAT_RESOLVE("total", "%.1f%s", total.value, total.unit);
		FORMAT_POST_RESOLVE;
	}
}
