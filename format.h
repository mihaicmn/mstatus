
#define FORMAT_LOAD(key) cfg_getstr(config, key)
#define FORMAT_LOAD_DEFAULT cfg_getstr(config, "format")

#define FORMAT_WALK(format) const char *c; for (c = format; *c != '\0'; c++)
#define FORMAT_CONSUME					\
	if (*c != '%') {				\
		text_putc(text, *c);			\
		continue;				\
	}
#define FORMAT_RESOLVE(variable, len, pattern, ...)	\
	if (STARTS_WITH(c + 1, variable, len)) {	\
		text_printf(text, pattern, __VA_ARGS__);\
		c += len;				\
		continue;				\
	}

enum comparison_t {
	ABOVE,
	BELOW
};

void decide_format(cfg_t *config, const double value, enum comparison_t comp, const char **format, enum color_t *color); 
