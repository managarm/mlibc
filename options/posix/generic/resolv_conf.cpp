#include <mlibc/resolv_conf.hpp>
#include <mlibc/allocator.hpp>
#include <stdio.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace mlibc {

frg::optional<struct nameserver_data> get_nameserver(int idx) {
	auto file = fopen("/etc/resolv.conf", "r");
	if (!file)
		return frg::null_opt;

	char line[128];
	struct nameserver_data ret;
	int cur = 0;
	while (fgets(line, 128, file)) {
		char *pos;
		if (!strchr(line, '\n') && !feof(file)) {
			// skip truncated lines
			for (int c = getc(file); c != '\n' && c != EOF; c = getc(file));
			continue;
		}

		if (!strncmp(line, "nameserver", 10) && isspace(line[10])) {
			if (cur != idx) {
				cur++;
				continue;
			}

			char *end;
			for (pos = line + 11; isspace(*pos); pos++);
			for (end = pos; *end && !isspace(*end); end++);
			*end = '\0';
			auto str = frg::string<MemoryAllocator>(
					pos, end - pos, getAllocator());


			struct in_addr ipv4;
			struct in6_addr ipv6;
			if (inet_pton(AF_INET, str.data(), &ipv4))
				ret.af = AF_INET;
			else if (inet_pton(AF_INET6, str.data(), &ipv6))
				ret.af = AF_INET6;
			else
				break;

			ret.name = str;
		}
	}

	fclose(file);
	if(ret.name.empty())
		return frg::null_opt;
	return ret;
}

} // namespace mlibc
