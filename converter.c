#include "converter.h"
#include "strings.h"

inline enum msystem_t system_valueof(const char *name) {
	if (EQUALS(name, "metric"))
		return METRIC;
	if (EQUALS(name, "iec"))
		return IEC;
	if (EQUALS(name, "jedec"))
		return JEDEC;
	die("invalid measurement_system: %s\n", name);
}

static struct magnitude_t {
	short base;
	const char *units[MAX_UNITS + 1];
} magnitudes[4] = {
	[METRIC] = { .units = { "", "k", "M", "G", "T" }, .base = 1000 },
	[IEC] = { .units = { "", "ki", "Mi", "Gi", "Ti" }, .base = 1024 },
	[JEDEC] = { .units = { "", "k", "M", "G", "T" }, .base = 1024 }
};

inline void convert_auto(const enum msystem_t msystem, const long value, struct usize_t *result) {
	int unit_index = 0;
	result->value = (double)value;
	while (result->value >= magnitudes[msystem].base && unit_index++ < MAX_UNITS)
		result->value /= magnitudes[msystem].base;
	result->unit = magnitudes[msystem].units[unit_index];
}

inline void convert_to(const enum msystem_t msystem, const long value, const int unit, double *result) {
	int unit_index = 0;
	*result = (double)value;
	while (unit_index++ < unit)
		*result /= magnitudes[msystem].base;
}
