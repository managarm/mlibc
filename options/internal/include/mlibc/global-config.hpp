#ifndef MLIBC_GLOBAL_CONFIG
#define MLIBC_GLOBAL_CONFIG

namespace mlibc {

struct GlobalConfig {
	GlobalConfig();
	
	bool debugMalloc;
};

inline const GlobalConfig &globalConfig() {
	static GlobalConfig cached;
	return cached;
}

}

#endif // MLIBC_GLOBAL_CONFIG
