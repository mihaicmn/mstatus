#define MAX_UNITS 4

enum msystem_t {
	METRIC,
	IEC,
	JEDEC
};

struct usize_t {
	double value;
	const char *unit;
};


inline enum msystem_t system_valueof(const char *name);

inline void convert_auto(const enum msystem_t msystem, const long value, struct usize_t *result);
inline void convert_to(const enum msystem_t msystem, const long value, const int unit_index, double *result);
