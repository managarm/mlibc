#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

typedef struct {
    const char *input;
    bool should_succeed;
    uint32_t expected_addr; // host byte order
} testcase;

int main(void) {
    struct in_addr addr;

    testcase tests[] = {
        { "127.0.0.1", true, 0x7F000001 },
        { "0.0.0.0", true, 0x00000000 },
        { "255.255.255.255", true, 0xFFFFFFFF },
        { "192.168.1.1", true, 0xC0A80101 },
        { "127.1", true, 0x7F000001 },
        { "192.168.1", true, 0xC0A80001 },
        { "0", true, 0x00000000 },
        { "2130706433", true, 0x7F000001 },
        { "0x7F.0.0.1", true, 0x7F000001 },
        { "0177.0.0.1", true, 0x7F000001 },
        { "127.0.0.0x01", true, 0x7F000001 },

        { "", false, 0 },
        { "256.0.0.1", false, 0 },
        { "-1.0.0.1", false, 0 },
        { "1.2.3.4.5", false, 0 },
        { "127.0.0.1.junk", false, 0 },
        { "hello.world", false, 0 },
        { " ", false, 0 },
        { "127.0.", false, 0 },
        { NULL, false, 0 } // Sentinel
    };

	for (size_t i = 0; tests[i].input; i++) {
        memset(&addr, 0, sizeof(addr));

        int result = inet_aton(tests[i].input, &addr);

        if (tests[i].should_succeed) {
            assert(result != 0);
            assert(addr.s_addr == htonl(tests[i].expected_addr));
        } else {
            assert(result == 0);
        }
    }

    return 0;
}
