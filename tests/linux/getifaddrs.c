#include <arpa/inet.h>
#include <assert.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <net/if.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	(void) argc;
	(void) argv;

	struct ifaddrs *ifaddr = 0;
	char host[NI_MAXHOST];
	bool loopback_found = false;

	if(getifaddrs(&ifaddr)) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	for(struct ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		if(ifa->ifa_addr == NULL)
			continue;

		int family = ifa->ifa_addr->sa_family;

		if(family == AF_INET && !strcmp("lo", ifa->ifa_name)) {
			loopback_found = true;

			printf("%-10s %s (%d)\n", ifa->ifa_name,
				(family == AF_INET)  ? "AF_INET" : (family == AF_INET6) ? "AF_INET6" : "???",
				family
			);

			int s = getnameinfo(ifa->ifa_addr,
				(family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
				host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			assert(!s);

			printf("\t\taddress: <%s>\n", host);
			if(ifa->ifa_netmask)
				printf("\t\tnetmask: <%s>\n", inet_ntoa(((struct sockaddr_in *) ifa->ifa_netmask)->sin_addr));

			if(ifa->ifa_flags & IFF_BROADCAST) {
				assert(ifa->ifa_broadaddr);
				printf("\t\tbroadcast: <%s>\n", inet_ntoa(((struct sockaddr_in *) ifa->ifa_broadaddr)->sin_addr));
			}

			if(ifa->ifa_flags & IFF_POINTOPOINT) {
				assert(ifa->ifa_dstaddr);
				printf("\t\tdest addr: <%s>\n", inet_ntoa(((struct sockaddr_in *) ifa->ifa_dstaddr)->sin_addr));
			}
		}
	}

	freeifaddrs(ifaddr);

	exit(loopback_found ? EXIT_SUCCESS : EXIT_FAILURE);
}
