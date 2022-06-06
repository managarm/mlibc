#include <mlibc/services.hpp>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mlibc/debug.hpp>

namespace mlibc {

static int parse_rest(service_buf &buf, char *end, int proto) {
	if (!strncmp(end, "/udp", 4)) {
		if (proto == IPPROTO_TCP && proto != -1)
			return 0;
		buf.protocol = IPPROTO_UDP;
		buf.socktype = SOCK_DGRAM;
	} else if (!strncmp(end, "/tcp", 4)) {
		if (proto == IPPROTO_UDP && proto != -1)
			return 0;
		buf.protocol = IPPROTO_TCP;
		buf.socktype = SOCK_STREAM;
	} else {
		return 0;
	}

	//TODO(geert): also parse aliases.

	return 1;
}

static int lookup_serv_file_port(service_result &buf, int proto, int port) {
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

	char line_buf[129] = {0};
	char *line = line_buf + 1;
	while(fgets(line, 128, file)) {
		int name_length = 0;
		char *pos;
		// easy way to handle comments, just move the end of the line
		// to the beginning of the comment
		if ((pos = strchr(line, '#'))) {
			*pos++ = '\n';
			*pos = '\0';
		}

		char *end = NULL;
		for (pos = line; *pos; pos++) {
			for (; isalpha(*pos); pos++);
			int rport = strtoul(pos, &end, 10);
			if (rport != port || rport > 65535) {
				pos = end;
				continue;
			}

			// We have found the port, time to rewind to the start
			// of the line.
			for (; pos[-1]; pos--)
				if(!isspace(pos[-1]))
					name_length++;
			break;
		}

		if (!pos)
			continue;

		if (!name_length)
			continue;

		auto name = frg::string<MemoryAllocator>(pos, name_length,
				getAllocator());

		struct service_buf sbuf = {};
		sbuf.port = port;
		sbuf.name = std::move(name);
		if (!parse_rest(sbuf, end, proto))
			continue;
		buf.push_back(std::move(sbuf));
	}

	fclose(file);
	return buf.size();
}

static int lookup_serv_file_name(service_result &buf, const char *name,
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
	int name_length = strlen(name);
	while(fgets(line, 128, file)) {
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

		// Skip the name at the beginning of the line.
		for(pos = line; *pos && !isspace(*pos); pos++)
			;

		char *end = NULL;
		int port = strtoul(pos, &end, 10);
		if (port > 65535 || end == pos)
			continue;

		struct service_buf sbuf;
		sbuf.port = port;
		sbuf.name = frg::string<MemoryAllocator>(name, getAllocator());
		if (!parse_rest(sbuf, end, proto))
			continue;

		buf.push_back(sbuf);

	}

	fclose(file);
	return buf.size();
}


// This function returns a negative error code, since a positive
// return code means success.
int lookup_serv_by_name(service_result &buf, const char *name, int proto,
		int socktype, int flags) {
	switch(socktype) {
		case SOCK_STREAM:
			if (!proto)
				proto = IPPROTO_TCP;
			else if (proto != IPPROTO_TCP)
				return -EAI_SERVICE;
			break;
		case SOCK_DGRAM:
			if (!proto)
				proto = IPPROTO_UDP;
			else if (proto != IPPROTO_UDP)
				return -EAI_SERVICE;
			break;
		case 0:
			break;
		default:
			if (name)
				return -EAI_SERVICE;
			buf[0].port = 0;
			buf[0].socktype = socktype;
			buf[0].protocol = proto;
			return 1;
	}

	char *end = nullptr;
	unsigned int port = 0;
	int count = 0;

	if (name) {
		if (!*name)
			return -EAI_SERVICE;
		port = strtoul(name, &end, 10);
	}
	// The end pointer is a null pointer so the name was a port
	// or the name was not specified.
	if (!end || !*end) {
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

	if (flags & AI_NUMERICSERV)
		return -EAI_NONAME;

	return lookup_serv_file_name(buf, name, proto);
}

int lookup_serv_by_port(service_result &buf, int proto, int port) {
	return lookup_serv_file_port(buf, proto, port);
}

} // namespace mlibc
