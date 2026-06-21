#include <stdlib.h>
#include <mlibc/allocator.hpp>
#include <mlibc/global-config.hpp>

namespace mlibc {

namespace {

constinit mlibc::lazy_eternal<GlobalConfig> globalConfigInstance;

} // namespace

struct GlobalConfigGuard {
	GlobalConfigGuard();
};

GlobalConfigGuard guard;

GlobalConfigGuard::GlobalConfigGuard() {
	// Force the config to be created during initialization of libc.so.
	globalConfigInstance.get();
}

static bool envEnabled(const char *env) {
	auto value = getenv(env);
	return value && *value && *value != '0';
}

GlobalConfig::GlobalConfig() {
	debugMalloc = envEnabled("MLIBC_DEBUG_MALLOC");
	debugPrintf = envEnabled("MLIBC_DEBUG_PRINTF");
	debugLocale = envEnabled("MLIBC_DEBUG_LOCALE");
	debugPthreadTrace = envEnabled("MLIBC_DEBUG_PTHREAD_TRACE");
	debugPathResolution = envEnabled("MLIBC_DEBUG_PATH_RESOLUTION");
	debugMonetaryLengths = envEnabled("MLIBC_DEBUG_MONETARY_LENGTHS");
}

const GlobalConfig &globalConfig() {
	return globalConfigInstance.get();
}

} // namespace mlibc
