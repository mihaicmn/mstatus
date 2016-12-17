#include "routine.h"
#include "nlwrap.h"

void link_routine(cfg_t *config, struct text_t * text) {
	const char *name = cfg_title(config);
	struct link_t link;

	if (link_fetch(name, &link) < 0)
		die("could not fetch link details\n");

	const char *format = FORMAT_LOAD_DEFAULT;
	text->color = COLOR_GOOD;

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("title", 5, "%s", name);
		FORMAT_RESOLVE("ip4", 3, "%s", link.ip4);
		FORMAT_RESOLVE("ip6", 3, "%s", link.ip6);
		FORMAT_RESOLVE("state", 5, "%d", link.state);
		FORMAT_POST_RESOLVE;
	}
}
