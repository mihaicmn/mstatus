#include <netlink/netlink.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <linux/nl80211.h>
#include <linux/if.h>
#include <string.h>

#include "routine.h"
#include "converter.h"
#include "util.h"

#define MAX_IP4 20
#define MAX_IP6 48
#define MAX_ESSID 32
#define MAX_BSSID 6
#define STRONGEST_SIGNAL -20
#define WEAKEST_SIGNAL -90

struct network_t {
	enum msystem_t msystem;
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
	char ip4[MAX_IP4];
	char ip6[MAX_IP6];
};

struct wifi_t {
	enum msystem_t msystem;

	int index;
	char essid[MAX_ESSID];
	float strength;
	int32_t signal;

	struct usize_t rx_rate;
	struct usize_t tx_rate;
};

/*---	COMMON HELPERS	---*/
static inline const char *state_valueof(const enum state_t state) {
	switch (state) {
	case OPERATIONAL:
		return "operational";
	case UP:
		return "up";
	default /*DOWN*/:
		return "down";
	}
}

static inline struct nl_sock *snl_connect(int protocol) {
	struct nl_sock *socket;

	if ((socket = nl_socket_alloc()) == NULL)
		die("cannot alloc nl_socket\n");

	if (nl_connect(socket, protocol) < 0)
		die("cannot connect socket\n");

	return socket;
}

static inline struct nl_msg *sgenl_message(const int family, const uint8_t cmd, const int ifindex) {
	struct nl_msg *msg;

	if ((msg = nlmsg_alloc()) == NULL)
		die("cannot allocate new message\n");

	if (!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family, 0, NLM_F_DUMP, cmd, 0))
		die("cannot set cmd within message\n");

	if (nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifindex) < 0)
		die("cannot set NL80211_ATTR_IFINDEX\n");

	return msg;

}

static inline void sgenl_send(struct nl_sock *socket, struct nl_msg *msg, nl_recvmsg_msg_cb_t cb, void *arg) {
	if (nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM, cb, arg) < 0)
		die("cannot set callback\n");

	if (nl_send_auto(socket, msg) < 0)
		die("cannot to send message\n");

	if (nl_wait_for_ack(socket) < 0)
		die("cannot wait for acknowledge\n");
}

/*---	NETWORK GROUP	---*/
void network_pre_routine(cfg_t *config, void **context) {
	struct network_t *network = smalloc(sizeof(struct network_t));
	struct nl_sock *socket = snl_connect(NETLINK_ROUTE);

	if (rtnl_link_alloc_cache(socket, AF_UNSPEC, &network->links) < 0)
		die("cannot alloc link_cache\n");

	if (rtnl_addr_alloc_cache(socket, &network->addrs) < 0)
		die("cannot alloc addr_cache\n");

	nl_socket_free(socket);

	network->msystem = system_valueof(cfg_getstr(config, "measurement_system"));

	*context = network;
}

void network_post_routine(void **context) {
	struct network_t *network = *context;
	nl_cache_free(network->links);
	nl_cache_free(network->addrs);
	sfree(network);
}

/*---	LINK SUBITEM	---*/
static inline int link_fetch(struct network_t *network, const char *name, struct link_t *link) {
	struct rtnl_link *rtlink = rtnl_link_get_by_name(network->links, name);

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

	struct nl_object *object = nl_cache_get_first(network->addrs);
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
			nl_addr2str(addr, link->ip4, MAX_IP4);
		else if (family == AF_INET6)
			nl_addr2str(addr, link->ip6, MAX_IP6);
	}

	return 0;
}

void link_subroutine(cfg_t *config, void *context, struct text_t *text) {
	const char *name = cfg_title(config);
	struct network_t *network = context;
	struct link_t link;

	const char *format;
	if (link_fetch(network, name, &link) < 0) {
		CHOOSE_FMTCOL("format_down", "color_bad");
	} else {
		if (link.ip4[0] && link.ip6[0]) {
			CHOOSE_FMTCOL("format", "color_normal");
		} else if (link.state == OPERATIONAL || link.state == UP) {
			CHOOSE_FMTCOL("format_up", "color_degraded");
		} else /*if (link.state == DOWN)*/ {
			CHOOSE_FMTCOL("format_down", "color_bad");
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

/*---	WIFI SUBITEM	---*/

// see:
//	http://www.infradead.org/~tgr/libnl/doc/core.html
//	https://git.launchpad.net/network-manager/tree/src/platform/wifi/wifi-utils-nl80211.c
//	https://github.com/i3/i3status/blob/master/src/print_wireless_info.c

static struct nla_policy bss_policy[NL80211_BSS_MAX + 1] = {
	[NL80211_BSS_INFORMATION_ELEMENTS] = { .type = NLA_UNSPEC },
	[NL80211_BSS_SIGNAL_MBM] = { .type = NLA_U32 },
	[NL80211_BSS_SIGNAL_UNSPEC] = { .type = NLA_U8 },
	[NL80211_BSS_STATUS] = { .type = NLA_U32 },
};
static struct nla_policy sta_policy[NL80211_STA_INFO_MAX + 1] = {
	[NL80211_STA_INFO_RX_BITRATE] = { .type = NLA_NESTED },
	[NL80211_STA_INFO_TX_BITRATE] = { .type = NLA_NESTED },
};
static struct nla_policy rate_policy[NL80211_RATE_INFO_MAX + 1] = {
	[NL80211_RATE_INFO_BITRATE] = { .type = NLA_U16 }
};


static int handler_get_scan(struct nl_msg *msg, void *arg) {
	struct wifi_t *wifi = arg;

	struct nlmsghdr *header = nlmsg_hdr(msg);
	struct nlattr *tb_attr[NL80211_ATTR_MAX + 1];
	struct nlattr *tb_bss[NL80211_BSS_MAX + 1];

	if (genlmsg_parse(header, 0, tb_attr, NL80211_ATTR_MAX, NULL) < 0)
		return NL_SKIP;

	if (tb_attr[NL80211_ATTR_BSS] == NULL)
		return NL_SKIP;

	if (nla_parse_nested(tb_bss, NL80211_BSS_MAX, tb_attr[NL80211_ATTR_BSS], bss_policy) < 0)
		return NL_SKIP;

	if (tb_bss[NL80211_BSS_STATUS] == NULL)
		return NL_SKIP;

	uint32_t status = nla_get_u32(tb_bss[NL80211_BSS_STATUS]);
	if (status != NL80211_BSS_STATUS_ASSOCIATED && status != NL80211_BSS_STATUS_IBSS_JOINED)
		return NL_SKIP;

	if (tb_bss[NL80211_BSS_SIGNAL_MBM]) {
		wifi->signal = (int32_t)nla_get_u32(tb_bss[NL80211_BSS_SIGNAL_MBM]) / 100; /* convert mBm to dBm */

		// idea from ipw2x00.c
		const int delta1 = STRONGEST_SIGNAL - WEAKEST_SIGNAL;
		const int delta2 = STRONGEST_SIGNAL - wifi->signal;

		wifi->strength = (100 * delta1 * delta1 - delta2 * ( 15 * delta1 + 62 * delta2)) / (delta1 * delta1); 
	}

	if (tb_bss[NL80211_BSS_SIGNAL_UNSPEC])
		wifi->strength = nla_get_u8(tb_bss[NL80211_BSS_SIGNAL_UNSPEC]);

	if (tb_bss[NL80211_BSS_INFORMATION_ELEMENTS]) {
		uint8_t *ies = nla_data(tb_bss[NL80211_BSS_INFORMATION_ELEMENTS]);
		uint32_t ies_len = nla_len(tb_bss[NL80211_BSS_INFORMATION_ELEMENTS]);

		while (ies_len > 2 && ies[0] != 0) {
			ies_len -= ies[1] + 2;
			ies += ies[1] + 2;
		}

		if (ies_len > 2 + ies[1])
			snprintf(wifi->essid, sizeof(wifi->essid), "%.*s", ies[1], ies + 2);
	}

	return NL_SKIP;
}

static int handler_get_station(struct nl_msg *msg, void *arg) {
	struct wifi_t *wifi = arg;

	struct nlmsghdr *header = nlmsg_hdr(msg);
	struct nlattr *tb_attr[NL80211_ATTR_MAX + 1];
	struct nlattr *tb_sta[NL80211_STA_INFO_MAX + 1];
	struct nlattr *tb_rxrate[NL80211_RATE_INFO_MAX + 1];
	struct nlattr *tb_txrate[NL80211_RATE_INFO_MAX + 1];

	if (genlmsg_parse(header, 0, tb_attr, NL80211_ATTR_MAX, NULL) < 0)
		return NL_SKIP;

	if (tb_attr[NL80211_ATTR_STA_INFO] == NULL)
		return NL_SKIP;

	if (nla_parse_nested(tb_sta, NL80211_STA_INFO_MAX, tb_attr[NL80211_ATTR_STA_INFO], sta_policy) < 0)
		return NL_SKIP;

	if (nla_parse_nested(tb_rxrate, NL80211_RATE_INFO_MAX, tb_sta[NL80211_STA_INFO_RX_BITRATE], rate_policy) < 0)
		return NL_SKIP;

	if (tb_rxrate[NL80211_RATE_INFO_BITRATE])
		convert_auto(wifi->msystem, nla_get_u16(tb_rxrate[NL80211_RATE_INFO_BITRATE]) * 100000, &wifi->rx_rate);

	if (nla_parse_nested(tb_txrate, NL80211_RATE_INFO_MAX, tb_sta[NL80211_STA_INFO_TX_BITRATE], rate_policy) < 0)
		return NL_SKIP;

	if (tb_txrate[NL80211_RATE_INFO_BITRATE])
		convert_auto(wifi->msystem, nla_get_u16(tb_txrate[NL80211_RATE_INFO_BITRATE]) * 100000, &wifi->tx_rate);

	return NL_SKIP;
}

static inline int wifi_fetch(struct network_t *network, const char *name, struct wifi_t *wifi) {
	struct rtnl_link *rtlink = rtnl_link_get_by_name(network->links, name);

	if (rtlink == NULL)
		return -1;

	wifi->index = rtnl_link_get_ifindex(rtlink);

	rtnl_link_put(rtlink);

	struct nl_sock *socket = snl_connect(NETLINK_GENERIC);
	const int family = genl_ctrl_resolve(socket, "nl80211");

	struct nl_msg *msg = sgenl_message(family, NL80211_CMD_GET_SCAN, wifi->index);
	sgenl_send(socket, msg, &handler_get_scan, wifi);
	nlmsg_free(msg);

	msg = sgenl_message(family, NL80211_CMD_GET_STATION, wifi->index);
	sgenl_send(socket, msg, &handler_get_station, wifi);
	nlmsg_free(msg);

	nl_socket_free(socket);
	return 0;
}

void wifi_subroutine(cfg_t *config, void *context, struct text_t *text) {
	const char *name = cfg_title(config);
	struct network_t *network = context;
	struct wifi_t wifi;

	memset(&wifi, 0, sizeof(struct wifi_t));
	wifi.msystem = network->msystem;

	const char *format;
	if (wifi_fetch(network, name, &wifi) < 0 || !wifi.essid[0]) {
		CHOOSE_FMTCOL("format_disconnected", "color_bad");
	} else {
		CHOOSE_FMTCOL_BYTHRESHOLD(wifi.strength, BELOW);
	}

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("title", 5, "%s", name);
		FORMAT_RESOLVE("essid", 5, "%s", wifi.essid);
		FORMAT_RESOLVE("strength", 8, "%.0f", wifi.strength);
		FORMAT_RESOLVE("signal", 6, "%d", wifi.signal);
		FORMAT_RESOLVE("rxrate", 6, "%3.0f%s", wifi.rx_rate.value, wifi.rx_rate.unit);
		FORMAT_RESOLVE("txrate", 6, "%3.0f%s", wifi.tx_rate.value, wifi.tx_rate.unit);
		FORMAT_POST_RESOLVE;
	}
}
