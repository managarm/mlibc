#include <stdlib.h>
#include <string.h>
#include <mlibc/global-config.hpp>

namespace mlibc {

struct GlobalConfigGuard {
	GlobalConfigGuard();
};

GlobalConfigGuard guard;

GlobalConfigGuard::GlobalConfigGuard() {
	// Force the config to be created during initialization of libc.so.
	mlibc::globalConfig();
}

static bool envEnabled(const char *env) {
	auto value = getenv(env);
	return value && *value && *value != '0';
}

GlobalConfig::GlobalConfig() {
	debugMalloc = envEnabled("MLIBC_DEBUG_MALLOC");
}

}
