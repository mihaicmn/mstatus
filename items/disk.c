#include <sys/statvfs.h>

#include "routine.h"

#define MAX_UNITS 4

enum system_t { METRIC, IEC, JEDEC };

struct magnitude_t {
	const char *const units[MAX_UNITS + 1];
	short base;
};

struct dsize_t {
	double value;
	const char *unit;
};


static struct magnitude_t magnitudes[3] = {
	{ .units = { "", "k", "M", "G", "T" }, .base = 1000 },  /* METRIC */
	{ .units = { "", "ki", "Mi", "Gi", "Ti" }, .base = 1024 }, /* IEC */
	{ .units = { "", "k", "M", "G", "T" }, .base = 1024 } /* JEDED */
};

static struct statvfs fs;


static void convert_bytes(enum system_t system, unsigned long bytes, struct dsize_t *dsize) {
	int unit_index = 0;
	dsize->value = (double)bytes;
	while (dsize->value >= magnitudes[system].base && unit_index++ < MAX_UNITS)
		dsize->value /= magnitudes[system].base;
	dsize->unit = magnitudes[system].units[unit_index];
}

static int get_disk_statistics(const char *path, enum system_t system, struct dsize_t *free, struct dsize_t *avail, struct dsize_t *used, struct dsize_t *total) {
	if (statvfs(path, &fs) != 0)
		return -1;

	convert_bytes(system, fs.f_bsize * fs.f_bfree, free);
	convert_bytes(system, fs.f_bsize * fs.f_bavail, avail);
	convert_bytes(system, fs.f_bsize * (fs.f_blocks - fs.f_bfree), used);
	convert_bytes(system, fs.f_bsize * fs.f_blocks, total);

	return 0;
}

void disk_routine(cfg_t *config, struct text_t *text) {
	const char *path = cfg_title(config);
	struct dsize_t free, avail, used, total;

	if (get_disk_statistics(path, JEDEC, &free, &avail, &used, &total) != 0)
		die("could not get disk statistics\n");

	text_printf(text, "%.1f%s %.1f%s %.1f%s %1.f%s",
		free.value, free.unit,
		avail.value, avail.unit,
		used.value, used.unit,
		total.value, total.unit);
}
