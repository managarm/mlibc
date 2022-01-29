#include <mlibc/resolv_conf.hpp>
#include <mlibc/allocator.hpp>
#include <stdio.h>
#include <ctype.h>

namespace mlibc {

frg::optional<struct nameserver_data> get_nameserver() {
	auto file = fopen("/etc/resolv.conf", "r");
	if (!file)
		return frg::null_opt;

	char line[128];
	struct nameserver_data ret;
	while (fgets(line, 128, file)) {
		char *pos;
		if (!strchr(line, '\n') && !feof(file)) {
			// skip truncated lines
			for (int c = getc(file); c != '\n' && c != EOF; c = getc(file));
			continue;
		}

		// TODO(geert): resolv.conf can actually have multiple nameservers
		// but we just pick the first one for now
		if (!strncmp(line, "nameserver", 10) && isspace(line[10])) {
			char *end;
			for (pos = line + 11; isspace(*pos); pos++);
			for (end = pos; *end && !isspace(*end); end++);
			*end = '\0';
			ret.name = frg::string<MemoryAllocator>(
					pos, end - pos, getAllocator());
			break;
		}
	}

	fclose(file);
	if(ret.name.empty())
		return frg::null_opt;
	return ret;
}

} // namespace mlibc
