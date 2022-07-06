#ifndef MLIBC_RTDL_CONFIG
#define MLIBC_RTDL_CONFIG

namespace mlibc {

struct RtdlConfig {
	bool secureRequired;
};

}

extern "C" const mlibc::RtdlConfig &__dlapi_get_config();

#ifndef MLIBC_BUILDING_RTDL
namespace mlibc {

inline const RtdlConfig &rtdlConfig() {
	return __dlapi_get_config();
}

}
#endif

#endif // MLIBC_RTDL_CONFIG
