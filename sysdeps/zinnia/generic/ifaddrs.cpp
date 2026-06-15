#include <ifaddrs.h>
#include <stdlib.h>

// TODO

int getifaddrs(struct ifaddrs **ifap) {
	*ifap = nullptr;
	return 0;
}

void freeifaddrs(struct ifaddrs *ifa) {
	while (ifa != nullptr) {
		struct ifaddrs *next = ifa->ifa_next;
		free(ifa);
		ifa = next;
	}
}
