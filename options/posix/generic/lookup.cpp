#include <mlibc/lookup.hpp>
#include <mlibc/debug.hpp>
#include <bits/ensure.h>

#include <frg/string.hpp>
#include <mlibc/allocator.hpp>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace mlibc {

static frg::string<MemoryAllocator> read_dns_name(char *buf, char *&it) {
	frg::string<MemoryAllocator> res{getAllocator()};
	while (true) {
		char code = *it++;
		if ((code & 0xC0) == 0xC0) {
			// pointer
			uint8_t offset = ((code & 0x3F) << 8) | *it++;
			auto offset_it = buf + offset;
			return res + read_dns_name(buf, offset_it);
		} else if (!(code & 0xC0)) {
			if (!code)
				break;

			for (int i = 0; i < code; i++)
				res += (*it++);

			if (*(it + 1))
				res += '.';
		} else {
			break;
		}
	}

	return res;
}

int lookup_name_dns(frg::vector<struct dns_addr_buf, MemoryAllocator> &buf, const char *name) {
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
	// set question type to fetch A records
	request += 0;
	request += 1;
	// set CLASS to IN
	request += 0;
	request += 1;

	struct sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(53);
	if (!inet_aton("8.8.8.8", &sin.sin_addr)) {
		mlibc::infoLogger() << "__lookup_name(): inet_aton() failed!" << frg::endlog;
		return -EAI_SYSTEM;
	}

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		mlibc::infoLogger() << "_lookup_name(): socket() failed" << frg::endlog;
		return -EAI_SYSTEM;
	}

	size_t sent = sendto(fd, request.data(), request.size(), 0,
			(struct sockaddr*)&sin, sizeof(sin));
	if (sent != request.size()) {
		mlibc::infoLogger() << "__lookup_name(): sendto() failed to send everything" << frg::endlog;
		return -EAI_SYSTEM;
	}

	char response[256];
	ssize_t rlen;
	int num_ans = 0;
	while ((rlen = recvfrom(fd, response, 256, 0, NULL, NULL)) >= 0) {
		if ((size_t)rlen < sizeof(struct dns_header))
			continue;
		auto response_header = reinterpret_cast<struct dns_header*>(response);
		if (response_header->identification != header.identification)
			return -EAI_FAIL;

		auto it = response + sizeof(struct dns_header);
		for (int i = 0; i < ntohs(response_header->no_q); i++) {
			auto dns_name = read_dns_name(response, it);
			(void) dns_name;
			it += 4;
		}

		for (int i = 0; i < ntohs(response_header->no_ans); i++) {
			struct dns_addr_buf buffer;
			auto dns_name = read_dns_name(response, it);
			buffer.name = std::move(dns_name);

			uint16_t rr_type = (it[0] << 8) | it[1];
			uint16_t rr_class = (it[2] << 8) | it[3];
			uint16_t rr_length = (it[8] << 8) | it[9];
			it += 10;

			if (rr_type == 1) {
				memcpy(buffer.addr, it, rr_length);
				buffer.family = AF_INET;
				buf.push_back(std::move(buffer));
			}
			it += rr_length;
		}
		num_ans += ntohs(response_header->no_ans);

		if (num_ans >= num_q)
			break;
	}

	close(fd);
	return buf.size();
}

} // namespace mlibc
