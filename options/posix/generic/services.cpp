#include <mlibc/services.hpp>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace mlibc {

static int lookup_serv_file(struct service_buf *buf, const char *name,
		int proto) {
	auto file = fopen(_PATH_SERVICES, "r");
	if (!file) {
		switch (errno) {
			case ENOENT:
			case ENOTDIR:
			case EACCES:
				return -EAI_SERVICE;
			default:
				return -EAI_SYSTEM;
		}
	}

	char line[128];
	int count = 0;
	int name_length = strlen(name);
	while(fgets(line, 128, file) && count < SERV_MAX) {
		char *pos;
		// easy way to handle comments, just move the end of the line
		// to the beginning of the comment
		if ((pos = strchr(line, '#'))) {
			*pos++ = '\n';
			*pos = '\0';
		}

		for (pos = line; (pos = strstr(pos, name)); pos++) {
			// the name must start and end with a space
			if (pos > line && !isspace(pos[-1]))
				continue;
			if (pos[name_length] && !isspace(pos[name_length]))
				continue;
			break;
		}
		if (!pos)
			continue;

		// now that we know the name is in this line we can parse
		// the rest

		// skip the name at the beginning of the line
		for(pos = line; *pos && !isspace(*pos); pos++)
			;

		char *end;
		int port = strtoul(pos, &end, 10);
		if (port > 65535 || end == pos)
			continue;

		if (!strncmp(end, "/udp", 4)) {
			if (proto == IPPROTO_TCP)
				continue;
			buf[count].port = port;
			buf[count].protocol = IPPROTO_UDP;
			buf[count].socktype = SOCK_DGRAM;
			count++;
		}
		if (!strncmp(end, "/tcp", 4)) {
			if (proto == IPPROTO_UDP)
				continue;
			buf[count].port = port;
			buf[count].protocol = IPPROTO_TCP;
			buf[count].socktype = SOCK_STREAM;
			count++;
		}

	}

	fclose(file);
	return count;
}

int lookup_serv(struct service_buf *buf, const char *name, int proto,
		int socktype) {
	switch(socktype) {
		case SOCK_STREAM:
			if (!proto)
				proto = IPPROTO_TCP;
			else if (proto != IPPROTO_TCP)
				return EAI_SERVICE;
			break;
		case SOCK_DGRAM:
			if (!proto)
				proto = IPPROTO_UDP;
			else if (proto != IPPROTO_UDP)
				return EAI_SERVICE;
			break;
		default:
			if (name)
				return EAI_SERVICE;
			buf[0].port = 0;
			buf[0].socktype = socktype;
			buf[0].protocol = proto;
			return 1;
	}

	// at this point we expect the user to provide a name
	if (!name)
		return EAI_NONAME;

	// handle the case where name is the port
	char *end;
	unsigned int port = strtoul(name, &end, 10);
	int count = 0;
	// the end pointer is a null pointer so the name was a port
	if (!*end) {
		if (proto != IPPROTO_UDP) {
			buf[count].port = port;
			buf[count].protocol = IPPROTO_TCP;
			buf[count].socktype = SOCK_STREAM;
			count++;
		}
		if (proto != IPPROTO_TCP) {
			buf[count].port = port;
			buf[count].protocol = IPPROTO_UDP;
			buf[count].socktype = SOCK_DGRAM;
			count++;
		}
		return count;
	}

	return lookup_serv_file(buf, name, proto);
}

} // namespace mlibc
