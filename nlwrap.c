#include <netlink/netlink.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <linux/if.h>

#include "nlwrap.h"


static int query_state(struct nl_sock *socket, const char *name, struct link_t *link) {
	struct rtnl_link *nl_link;

	if (rtnl_link_get_kernel(socket, 0, name, &nl_link) < 0)
		return -1;

	link->index = rtnl_link_get_ifindex(nl_link);

	unsigned int flags = rtnl_link_get_flags(nl_link);
	uint8_t operstate = rtnl_link_get_operstate(nl_link);

	if (operstate & IF_OPER_UP)
		link->state = OPERATIONAL;
	else if (flags & IFF_RUNNING)
		link->state = UP;
	else
		link->state = DOWN;

	rtnl_link_put(nl_link);

	return 0;
}

static int query_address(struct nl_sock *socket, const char *name, struct link_t *link) {
	struct nl_cache *cache;
	struct nl_object *object;
	struct nl_addr *addr;
	struct rtnl_addr *raddr;
	int family;

	if (rtnl_addr_alloc_cache(socket, &cache) < 0)
		return -1;

	link->ip4[0] = '\0';
	link->ip6[0] = '\0';

	object = nl_cache_get_first(cache);
	while (object) {
		raddr = (struct rtnl_addr*)object;
		object = nl_cache_get_next(object);

		if (link->index != rtnl_addr_get_ifindex(raddr))
			continue;

		addr = rtnl_addr_get_local(raddr);
		family = nl_addr_get_family(addr);

		if (family == AF_INET) {
			nl_addr2str(addr, link->ip4, INET_ADDRSTRLEN);
		} else if (family == AF_INET6) {
			nl_addr2str(addr, link->ip6, INET6_ADDRSTRLEN);
		}
	}

	nl_cache_free(cache);
	return 0;
}

int link_fetch(const char *name, struct link_t *link) {
	struct nl_sock *socket;
	if ((socket = nl_socket_alloc()) == NULL)
		return -1;

	if (nl_connect(socket, NETLINK_ROUTE) < 0)
		goto release_socket;

	if (query_state(socket, name, link) < 0)
		goto release_socket;

	if (query_address(socket, name, link) < 0)
		goto release_socket;

	nl_socket_free(socket);
	return 0;

release_socket:
	nl_socket_free(socket);
	return -1;
}
