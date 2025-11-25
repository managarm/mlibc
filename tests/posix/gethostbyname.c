#include <netdb.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    struct hostent *ent;
    char ip[INET_ADDRSTRLEN];
    
    ent = gethostbyname("dns.google");
    assert(ent);
    assert(ent->h_addrtype == AF_INET);
    assert(inet_ntop(AF_INET, ent->h_addr, ip, sizeof(ip)));
    assert(strcmp(ip, "8.8.8.8") == 0 || strcmp(ip, "8.8.4.4") == 0);
    
    ent = gethostbyname("8.8.8.8");
    assert(ent);
    assert(ent->h_addrtype == AF_INET);
    assert(inet_ntop(AF_INET, ent->h_addr, ip, sizeof(ip)));
    assert(strcmp(ip, "8.8.8.8") == 0);

    return 0;
}
