#include <abi-bits/errno.h>
#include <mlibc/all-sysdeps.hpp>

namespace {

using dword = unsigned long;

constexpr dword infinite_timeout = 0xFFFFFFFFul;
constexpr dword maximum_finite_timeout = infinite_timeout - 1;
constexpr dword error_invalid_parameter = 87;
constexpr dword error_timeout = 1460;

constexpr uint64_t ns_per_ms = 1000000;
constexpr uint64_t ms_per_second = 1000;
constexpr uint64_t ns_per_second = 1000000000;

extern "C" dword GetCurrentThreadId();
extern "C" dword GetLastError();
extern "C" int WaitOnAddress(volatile void *, void *, size_t, dword);
extern "C" void WakeByAddressSingle(void *);
extern "C" void WakeByAddressAll(void *);

int ts_to_ms(const timespec &timeout, uint64_t *ms) {
	if (timeout.tv_sec < 0 || timeout.tv_nsec < 0 || static_cast<uint64_t>(timeout.tv_nsec) >= ns_per_second)
		return EINVAL;

	auto seconds = static_cast<uint64_t>(timeout.tv_sec);
	auto partial_ms = (static_cast<uint64_t>(timeout.tv_nsec) + ns_per_ms - 1) / ns_per_ms;

	if (seconds > (UINT64_MAX - partial_ms) / ms_per_second)
		*ms = UINT64_MAX;
	else
		*ms = seconds * ms_per_second + partial_ms;
	return 0;
}

int wait_on_address_once(int *pointer, int expected, dword timeout) {
	if (WaitOnAddress(pointer, &expected, sizeof(expected), timeout))
		return 0;

	switch (GetLastError()) {
		case error_timeout:
			return ETIMEDOUT;
		case error_invalid_parameter:
			return EINVAL;
		default:
			return EIO;
	}
}

int wait_on_address_for(int *pointer, int expected, uint64_t timeout) {
	while (timeout > maximum_finite_timeout) {
		auto error = wait_on_address_once(pointer, expected, maximum_finite_timeout);
		if (error != ETIMEDOUT)
			return error;
		timeout -= maximum_finite_timeout;
	}

	return wait_on_address_once(pointer, expected, static_cast<dword>(timeout));
}

} // namespace

namespace mlibc {

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const timespec *timeout) {
	if (!timeout)
		return wait_on_address_once(pointer, expected, infinite_timeout);

	uint64_t ms;
	if (auto error = ts_to_ms(*timeout, &ms); error)
		return error;
	return wait_on_address_for(pointer, expected, ms);
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
	if (all)
		WakeByAddressAll(pointer);
	else
		WakeByAddressSingle(pointer);
	return 0;
}

pid_t Sysdeps<FutexTid>::operator()() {
	return static_cast<pid_t>(GetCurrentThreadId());
}

} // namespace mlibc
