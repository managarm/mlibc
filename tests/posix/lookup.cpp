#include <mlibc/tcb.hpp>
#include <mlibc/lookup.hpp>
#include <stdio.h>

int main() {
	struct mlibc::lookup_result res;
	frg::string<MemoryAllocator> canon_name{getAllocator(), "google.com"};

	auto ret = mlibc::lookup_name_dns(res, "google.com", canon_name);
	if(ret > 0) {
		char buf[INET6_ADDRSTRLEN];
		inet_ntop(res.buf.front().family, res.buf.front().addr, buf, sizeof(buf));
		fprintf(stderr, "Lookup of google.com returns: '%s'\n", buf);
	} else {
		return 1;
	}

	return 0;
}
