#include <arpa/inet.h>

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

int link_fetch(const char *name, struct link_t *link);
