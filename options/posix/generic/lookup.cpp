#include <mlibc/lookup.hpp>
#include <mlibc/resolv_conf.hpp>
#include <mlibc/debug.hpp>
#include <bits/ensure.h>

#include <frg/string.hpp>
#include <mlibc/allocator.hpp>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

namespace {

enum class DnsClass : uint16_t {
	IN = 1,
};

enum class ResourceRecord : uint16_t {
	A = 1,
	NS = 2,
	CNAME = 5,
	PTR = 12,
	TXT = 16,
	AAAA = 28,
	OPT = 41,
};

frg::optional<frg::string<MemoryAllocator>> read_dns_name(frg::span<uint8_t> buf, size_t &offset) {
	frg::string<MemoryAllocator> res{getAllocator()};
	while(offset < buf.size()) {
		uint8_t code = buf[offset++];

		if((code & 0xC0) == 0xC0) {
			// pointer
			if(offset + 1 > buf.size()) {
				return frg::null_opt;
			}

			uint8_t pointer_offset = ((code & 0x3F) << 8) | buf[offset++];
			if(pointer_offset >= buf.size()) {
				return frg::null_opt;
			}

			size_t sub_offset = pointer_offset;
			auto sub_name = read_dns_name(buf, sub_offset);
			if(!sub_name) {
				return frg::null_opt;
			}

			return res + *sub_name;
		} else if(!(code & 0xC0)) {
			if (!code) {
				return res;
			} else if(offset + code > buf.size()) {
				return frg::null_opt;
			}

			for (int i = 0; i < code; i++) {
				res += buf[offset++];
			}

			if (offset < buf.size() && buf[offset]) {
				res += '.';
			}
		} else {
			return frg::null_opt;
		}
	}

	return frg::null_opt;
}

void writeUint16(frg::string<MemoryAllocator> &request, uint16_t val) {
	request += static_cast<char>(val >> 8);
	request += static_cast<char>(val & 0xFF);
};

uint16_t readUint16(frg::span<uint8_t> buf, size_t offset) {
	uint16_t val = 0;
	memcpy(&val, buf.data() + offset, sizeof(val));
	return ntohs(val);
}

} // namespace

namespace mlibc {

int lookup_name_dns(struct lookup_result &buf, const char *name,
		frg::string<MemoryAllocator> &canon_name) {
	frg::string<MemoryAllocator> request{getAllocator()};

	int num_q = 1;
	struct dns_header header;
	header.identification = htons(123);
	header.flags = htons(0x100);
	header.no_q = htons(num_q);
	header.no_ans = htons(0);
	header.no_auths = htons(0);
	header.no_additional = htons(0);

	request.resize(sizeof(header));
	memcpy(request.data(), &header, sizeof(header));

	const char *end = name;
	while (*end != '\0') {
		end = strchrnul(name, '.');
		size_t length = end - name;
		frg::string_view substring{name, length};
		name += length + 1;
		request += char(length);
		request += substring;
	}

	request += char(0);
	writeUint16(request, static_cast<uint16_t>(ResourceRecord::A));
	writeUint16(request, static_cast<uint16_t>(DnsClass::IN));

	struct sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	// TODO(geert): we could probably make this use the service lookup
	// for dns
	sin.sin_port = htons(53);

	auto nameserver = get_nameserver();
	if (!inet_aton(nameserver ? nameserver->name.data() : "127.0.0.1", &sin.sin_addr)) {
		mlibc::infoLogger() << "lookup_name_dns(): inet_aton() failed!" << frg::endlog;
		return -EAI_SYSTEM;
	}

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		mlibc::infoLogger() << "lookup_name_dns(): socket() failed" << frg::endlog;
		return -EAI_SYSTEM;
	}

	size_t sent = sendto(fd, request.data(), request.size(), 0,
			(struct sockaddr*)&sin, sizeof(sin));
	if (sent != request.size()) {
		mlibc::infoLogger() << "lookup_name_dns(): sendto() failed to send everything" << frg::endlog;
		return -EAI_SYSTEM;
	}

	uint8_t response[512];
	ssize_t rlen;
	int num_ans = 0;
	while ((rlen = recvfrom(fd, response, sizeof(response), 0, NULL, NULL)) >= 0) {
		if ((size_t)rlen < sizeof(struct dns_header))
			continue;
		auto response_header = reinterpret_cast<struct dns_header *>(response);
		if (response_header->identification != header.identification)
			return -EAI_FAIL;

		auto view = frg::span<uint8_t>{response, static_cast<size_t>(rlen)};
		size_t offset = sizeof(struct dns_header);

		for (int i = 0; i < ntohs(response_header->no_q); i++) {
			auto dns_name = read_dns_name(view, offset);
			if(!dns_name)
				return -EAI_FAIL;

			// skip type and class
			offset += 4;
		}

		for (int i = 0; i < ntohs(response_header->no_ans); i++) {
			auto dns_name = read_dns_name(view, offset);
			if(!dns_name) {
				return -EAI_FAIL;
			} else if(offset + 10 > view.size()) {
				return -EAI_FAIL;
			}

			ResourceRecord rrType{readUint16(view, offset + 0)};
			DnsClass rrClass{readUint16(view, offset + 2)};
			// ignore TTL
			uint16_t rrLength = readUint16(view, offset + 8);
			offset += 10;

			if(rrClass != DnsClass::IN || offset + rrLength > view.size()) {
				return -EAI_FAIL;
			}

			struct dns_addr_buf buffer;
			switch (rrType) {
				case ResourceRecord::A:
					memcpy(buffer.addr, &view[offset], rrLength);
					offset += rrLength;
					buffer.family = AF_INET;
					buffer.name = std::move(*dns_name);
					buf.buf.push(std::move(buffer));
					break;
				case ResourceRecord::CNAME: {
					size_t suboffset = offset;
					auto cname = read_dns_name(view, suboffset);
					if(!cname) {
						return -EAI_FAIL;
					}
					canon_name = std::move(*cname);
					buf.aliases.push(std::move(*dns_name));
					offset = suboffset;
					break;
				}
				default:
					mlibc::infoLogger() << "lookup_name_dns: unknown rr type "
						<< static_cast<uint16_t>(rrType) << frg::endlog;
					break;
			}
		}
		num_ans += ntohs(response_header->no_ans);

		if (num_ans >= num_q)
			break;
	}

	close(fd);
	return buf.buf.size();
}

int lookup_addr_dns(frg::span<char> name, frg::array<uint8_t, 16> &addr, int family) {
	frg::string<MemoryAllocator> request{getAllocator()};

	int num_q = 1;
	struct dns_header header;
	header.identification = htons(123);
	header.flags = htons(0x100);
	header.no_q = htons(num_q);
	header.no_ans = htons(0);
	header.no_auths = htons(0);
	header.no_additional = htons(0);

	request.resize(sizeof(header));
	memcpy(request.data(), &header, sizeof(header));

	char addr_str[INET6_ADDRSTRLEN];
	if(!inet_ntop(family, addr.data(), addr_str, sizeof(addr_str))) {
		switch(errno) {
			case EAFNOSUPPORT:
				return -EAI_FAMILY;
			case ENOSPC:
				return -EAI_OVERFLOW;
			default:
				return -EAI_FAIL;
		}
	}
	frg::string<MemoryAllocator> req_str{getAllocator(), addr_str};
	req_str += ".in-addr.arpa";

	frg::string_view req_view{req_str.data(), req_str.size()};
	size_t ptr = 0;
	do {
		size_t next = req_view.find_first('.', ptr);
		size_t length = next != (size_t)-1 ? next - ptr : req_view.size() - ptr;
		frg::string_view substring = req_view.sub_string(ptr, length);
		request += char(length);
		request += substring;
		ptr = next + 1;
	} while(ptr != 0);

	request += char(0);
	writeUint16(request, static_cast<uint16_t>(ResourceRecord::PTR));
	writeUint16(request, static_cast<uint16_t>(DnsClass::IN));

	struct sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	// TODO(geert): we could probably make this use the service lookup
	// for dns
	sin.sin_port = htons(53);

	auto nameserver = get_nameserver();
	if (!inet_aton(nameserver ? nameserver->name.data() : "127.0.0.1", &sin.sin_addr)) {
		mlibc::infoLogger() << "lookup_name_dns(): inet_aton() failed!" << frg::endlog;
		return -EAI_SYSTEM;
	}

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		mlibc::infoLogger() << "lookup_name_dns(): socket() failed" << frg::endlog;
		return -EAI_SYSTEM;
	}

	size_t sent = sendto(fd, request.data(), request.size(), 0,
			(struct sockaddr*)&sin, sizeof(sin));
	if (sent != request.size()) {
		mlibc::infoLogger() << "lookup_name_dns(): sendto() failed to send everything" << frg::endlog;
		return -EAI_SYSTEM;
	}

	uint8_t response[512];
	ssize_t rlen;
	int num_ans = 0;
	while ((rlen = recvfrom(fd, response, sizeof(response), 0, NULL, NULL)) >= 0) {
		if ((size_t)rlen < sizeof(struct dns_header))
			continue;
		auto response_header = reinterpret_cast<struct dns_header*>(response);
		if (response_header->identification != header.identification)
			return -EAI_FAIL;

		frg::span<uint8_t> view{response, static_cast<size_t>(rlen)};
		size_t offset = sizeof(struct dns_header);

		for (int i = 0; i < ntohs(response_header->no_q); i++) {
			auto dns_name = read_dns_name(view, offset);
			if(!dns_name)
				return -EAI_FAIL;

			offset += 4;
		}

		for (int i = 0; i < ntohs(response_header->no_ans); i++) {
			auto dns_name = read_dns_name(view, offset);
			if(!dns_name) {
				return -EAI_FAIL;
			} else if(offset + 10 > view.size()) {
				return -EAI_FAIL;
			}

			ResourceRecord rrType{readUint16(view, offset + 0)};
			DnsClass rrClass{readUint16(view, offset + 2)};
			// ignore TTL
			uint16_t rrLength = readUint16(view, offset + 8);

			offset += 10;

			if(rrClass != DnsClass::IN || offset + rrLength > view.size()) {
				return -EAI_FAIL;
			}

			struct dns_addr_buf buffer;
			switch (rrType) {
				case ResourceRecord::PTR: {
					auto ptr_name = read_dns_name(view, offset);
					if(!ptr_name) {
						return -EAI_FAIL;
					} else if(ptr_name->size() >= name.size()) {
						return -EAI_OVERFLOW;
					}
					std::copy(ptr_name->begin(), ptr_name->end(), name.data());
					name.data()[ptr_name->size()] = '\0';
					return 1;
				}
				default:
					mlibc::infoLogger() << "lookup_addr_dns: unknown rr type "
						<< static_cast<uint16_t>(rrType) << frg::endlog;
					break;
			}
			num_ans += ntohs(response_header->no_ans);

			if (num_ans >= num_q)
				break;
		}
	}

	close(fd);
	return 0;
}

int lookup_name_hosts(struct lookup_result &buf, const char *name,
		frg::string<MemoryAllocator> &canon_name) {
	auto file = fopen("/etc/hosts", "r");
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
	while (fgets(line, 128, file)) {
		char *pos;
		// same way to deal with comments as in services.cpp
		if ((pos = strchr(line, '#'))) {
			*pos++ = '\n';
			*pos = '\0';
		}

		for(pos = line + 1; (pos = strstr(pos, name)) &&
				(!isspace(pos[-1]) || !isspace(pos[name_length])); pos++);
		if (!pos)
			continue;

		for (pos = line; !isspace(*pos); pos++);
		*pos = '\0';

		// TODO(geert): we assume ipv4 for now
		struct in_addr addr;
		if (!inet_aton(line, &addr))
			continue;

		pos++;
		for(; *pos && isspace(*pos); pos++);
		char *end;
		for(end = pos; *end && !isspace(*end); end++);

		struct dns_addr_buf buffer;
		memcpy(buffer.addr, &addr, 4);
		buffer.family = AF_INET;
		buffer.name = frg::string<MemoryAllocator>{pos,
			static_cast<size_t>(end - pos), getAllocator()};
		canon_name = buffer.name;

		buf.buf.push(std::move(buffer));

		pos = end;
		while (pos[1]) {
			for (; *pos && isspace(*pos); pos++);
			for (end = pos; *end && !isspace(*end); end++);
			auto name = frg::string<MemoryAllocator>{pos,
				static_cast<size_t>(end - pos), getAllocator()};
			buf.aliases.push(std::move(name));
			pos = end;
		}
	}

	fclose(file);
	return buf.buf.size();
}

int lookup_addr_hosts(frg::span<char> name, frg::array<uint8_t, 16> &addr, int family) {
	auto file = fopen("/etc/hosts", "r");
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

	// Buffer to hold ASCII version of address
	char addr_str[64];
	if(!inet_ntop(family, addr.data(), addr_str, sizeof(addr_str))) {
		switch(errno) {
			case EAFNOSUPPORT:
				return -EAI_FAMILY;
			case ENOSPC:
				return -EAI_OVERFLOW;
			default:
				return -EAI_FAIL;
		}
	}
	int addr_str_len = strlen(addr_str);

	char line[128];
	while (fgets(line, 128, file)) {
		char *pos;
		// same way to deal with comments as in services.cpp
		if ((pos = strchr(line, '#'))) {
			*pos++ = '\n';
			*pos = '\0';
		}
		if (strncmp(line, addr_str, addr_str_len))
			continue;

		for (pos = line + addr_str_len + 1; isspace(*pos); pos++);
		char *begin = pos;
		for (; !isspace(*pos); pos++);
		char *end = pos;

		size_t size = end - begin;
		if (size >= name.size())
			return -EAI_OVERFLOW;
		std::copy(begin, end, name.data());
		name.data()[size] = '\0';
		return 1;
	}
	return 0;
}

int lookup_name_null(struct lookup_result &buf, int flags, int family) {
	if (flags & AI_PASSIVE) {
		if (family != AF_INET6) {
			struct dns_addr_buf addr_buf;
			addr_buf.family = AF_INET;

			in_addr_t addr = INADDR_ANY;
			memcpy(&addr_buf.addr, &addr, 4);

			buf.buf.push_back(addr_buf);
		}
		if (family != AF_INET) {
			struct dns_addr_buf addr_buf;
			addr_buf.family = AF_INET6;

			struct in6_addr addr = IN6ADDR_ANY_INIT;
			memcpy(&addr_buf.addr, &addr, 16);

			buf.buf.push_back(addr_buf);
		}
	} else {
		if (family != AF_INET6) {
			struct dns_addr_buf addr_buf;
			addr_buf.family = AF_INET;

			in_addr_t addr = INADDR_LOOPBACK;
			memcpy(&addr_buf.addr, &addr, 4);

			buf.buf.push_back(addr_buf);
		}
		if (family != AF_INET) {
			struct dns_addr_buf addr_buf;
			addr_buf.family = AF_INET6;

			struct in6_addr addr = IN6ADDR_LOOPBACK_INIT;
			memcpy(&addr_buf.addr, &addr, 16);

			buf.buf.push_back(addr_buf);
		}
	}
	return buf.buf.size();
}

int lookup_name_ip(struct lookup_result &buf, const char *name, int family) {
	if (family == AF_INET) {
		in_addr_t addr = 0;
		int res = inet_pton(AF_INET, name, &addr);

		if (res <= 0)
			return -EAI_NONAME;

		struct dns_addr_buf addr_buf;
		addr_buf.family = AF_INET;
		memcpy(&addr_buf.addr, &addr, 4);

		buf.buf.push_back(addr_buf);
		return 1;
	}

	if (family == AF_INET6) {
		struct in6_addr addr{};
		int res = inet_pton(AF_INET6, name, &addr);

		if (res <= 0)
			return -EAI_NONAME;

		struct dns_addr_buf addr_buf;
		addr_buf.family = AF_INET6;
		memcpy(&addr_buf.addr, &addr, 16);

		buf.buf.push_back(addr_buf);
		return 1;
	}

	// If no family was specified we try ipv4 and then ipv6.
	in_addr_t addr4 = 0;
	int res = inet_pton(AF_INET, name, &addr4);

	if (res > 0) {
		struct dns_addr_buf addr_buf;
		addr_buf.family = AF_INET;
		memcpy(&addr_buf.addr, &addr4, 4);

		buf.buf.push_back(addr_buf);
		return 1;
	}

	struct in6_addr addr6{};
	res = inet_pton(AF_INET6, name, &addr6);

	if (res <= 0)
		return -EAI_NONAME;

	struct dns_addr_buf addr_buf;
	addr_buf.family = AF_INET6;
	memcpy(&addr_buf.addr, &addr6, 16);

	buf.buf.push_back(addr_buf);
	return 1;
}

} // namespace mlibc
