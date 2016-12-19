#include <arpa/inet.h>
#include <netlink/netlink.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <linux/if.h>

#include "routine.h"
#include "util.h"

struct cache_t {
	struct nl_cache *links;
	struct nl_cache *addrs;
};

enum state_t {
	OPERATIONAL,
	UP,
	DOWN
};

struct link_t {
	int index;
	enum state_t state;
	char ip4[INET_ADDRSTRLEN];
	char ip6[INET6_ADDRSTRLEN];
};

static inline const char *state_valueof(const enum state_t state) {
	switch (state) {
	case OPERATIONAL:
		return "operational";
	case UP:
		return "up";
	case DOWN:
		return "down";
	}
}

static inline int link_fetch(struct cache_t *cache, const char *name, struct link_t *link) {
	struct rtnl_link *rtlink = rtnl_link_get_by_name(cache->links, name);

	if (rtlink == NULL)
		return -1;

	link->index = rtnl_link_get_ifindex(rtlink);

	unsigned int flags = rtnl_link_get_flags(rtlink);
	uint8_t operstate = rtnl_link_get_operstate(rtlink);

	if (operstate & IF_OPER_UP)
		link->state = OPERATIONAL;
	else if (flags & IFF_RUNNING)
		link->state = UP;
	else
		link->state = DOWN;

	rtnl_link_put(rtlink);

	struct nl_object *object = nl_cache_get_first(cache->addrs);
	struct rtnl_addr *rtaddr;
	struct nl_addr *addr;
	int family;

	link->ip4[0] = '\0';
	link->ip6[0] = '\0';

	while (object) {
		rtaddr = (struct rtnl_addr*)object;
		object = nl_cache_get_next(object);

		if (link->index != rtnl_addr_get_ifindex(rtaddr))
			continue;

		addr = rtnl_addr_get_local(rtaddr);
		family = nl_addr_get_family(addr);

		if (family == AF_INET)
			nl_addr2str(addr, link->ip4, INET_ADDRSTRLEN);
		else if (family == AF_INET6)
			nl_addr2str(addr, link->ip6, INET6_ADDRSTRLEN);
	}

	return 0;
}


void network_preroutine(cfg_t *config, void **context) {
	struct cache_t *cache = smalloc(sizeof(struct cache_t));
	struct nl_sock *socket;

	if ((socket = nl_socket_alloc()) == NULL)
		die("cannot alloc nl_socket\n");

	if (nl_connect(socket, NETLINK_ROUTE) < 0)
		die("cannot connect socket\n");

	if (rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache->links) < 0)
		die("cannot alloc link_cache\n");

	if (rtnl_addr_alloc_cache(socket, &cache->addrs) < 0)
		die("cannot alloc addr_cache\n");

	struct rtnl_link *link = rtnl_link_get_by_name(cache->links, "wlp2s0");
	if (link == NULL)
		die ("sdfsdfsafas\n");

	nl_socket_free(socket);
	*context = cache;
}

void network_postroutine(cfg_t *config, void **context) {
	struct cache_t *cache = *context;
	nl_cache_free(cache->links);
	nl_cache_free(cache->addrs);
	sfree(cache);
}

void link_subroutine(cfg_t *config, void *context, struct text_t *text) {
	const char *name = cfg_title(config);
	struct cache_t *cache = context;
	struct link_t link;

	const char *format;
	if (link_fetch(cache, name, &link) < 0) {
		CHOOSE_FORMAT_AND_COLOR("format_bad", COLOR_BAD);
	} else {
		if (link.ip4[0] || link.ip6[0]) {
			CHOOSE_FORMAT_AND_COLOR("format", COLOR_GOOD);
		} else if (link.state == OPERATIONAL || link.state == UP) {
			CHOOSE_FORMAT_AND_COLOR("format_degraded", COLOR_DEGRADED);
		} else /*if (link.state == DOWN)*/ {
			CHOOSE_FORMAT_AND_COLOR("format_bad", COLOR_BAD);
		}
	}

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("title", 5, "%s", name);
		FORMAT_RESOLVE("ip4", 3, "%s", link.ip4);
		FORMAT_RESOLVE("ip6", 3, "%s", link.ip6);
		FORMAT_RESOLVE("state", 5, "%s", state_valueof(link.state));
		FORMAT_POST_RESOLVE;
	}
}
