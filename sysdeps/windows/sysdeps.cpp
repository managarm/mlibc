#include <abi-bits/errno.h>
#include <mlibc/all-sysdeps.hpp>

namespace {

using dword = unsigned long;

struct filetime {
	dword dwLowDateTime;
	dword dwHighDateTime;
};

constexpr dword mem_commit = 0x1000;
constexpr dword mem_reserve = 0x2000;
constexpr dword mem_release = 0x8000;
constexpr dword page_readwrite = 0x04;

constexpr dword tls_out_of_indexes = 0xFFFFFFFFul;

constexpr uint64_t windows_epoch_offset = 116444736000000000ull;
constexpr uint64_t filetime_ticks_per_second = 10000000;
constexpr uint64_t ns_per_filetime_tick = 100;
constexpr int clock_realtime = 0;

extern "C" void GetSystemTimePreciseAsFileTime(filetime *);
extern "C" [[noreturn]] void ExitProcess(dword);

extern "C" void *VirtualAlloc(void *, size_t, dword, dword);
extern "C" int VirtualFree(void *, size_t, dword);

extern "C" dword TlsAlloc();
extern "C" int TlsSetValue(dword, void *);
extern "C" void *TlsGetValue(dword);

dword tcb_tls_index = tls_out_of_indexes;

int ensure_tcb_tls_slot() {
	if (tcb_tls_index == tls_out_of_indexes)
		tcb_tls_index = TlsAlloc();
	return tcb_tls_index == tls_out_of_indexes ? ENOMEM : 0;
}

uint64_t filetime_to_ticks(const filetime &time) {
	return (static_cast<uint64_t>(time.dwHighDateTime) << 32) | time.dwLowDateTime;
}

} // namespace

namespace mlibc {

[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("!!! mlibc panic !!!\n");
	sysdep<Exit>(127);
}

[[noreturn]] void Sysdeps<Exit>::operator()(int status) {
	ExitProcess(static_cast<dword>(status));
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *seconds, long *ns) {
	if (clock != clock_realtime)
		return EINVAL;

	filetime time;
	GetSystemTimePreciseAsFileTime(&time);
	auto ticks = filetime_to_ticks(time) - windows_epoch_offset;

	*seconds = static_cast<time_t>(ticks / filetime_ticks_per_second);
	*ns = static_cast<long>((ticks % filetime_ticks_per_second) * ns_per_filetime_tick);
	return 0;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	*pointer = nullptr;
	if (!size)
		return EINVAL;

	*pointer = VirtualAlloc(nullptr, size, mem_reserve | mem_commit, page_readwrite);
	return *pointer ? 0 : ENOMEM;
}

int Sysdeps<AnonFree>::operator()(void *pointer, size_t) {
	return VirtualFree(pointer, 0, mem_release) ? 0 : EINVAL;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	if (auto error = ensure_tcb_tls_slot(); error)
		return error;
	return TlsSetValue(tcb_tls_index, pointer) ? 0 : EINVAL;
}

void *Sysdeps<TcbGet>::operator()() {
	if (tcb_tls_index == tls_out_of_indexes)
		return nullptr;
	return TlsGetValue(tcb_tls_index);
}

int Sysdeps<VmMap>::operator()(void *, size_t, int, int, int, off_t, void **window) {
	*window = nullptr;
	return ENOSYS;
}

int Sysdeps<VmUnmap>::operator()(void *, size_t) {
	return ENOSYS;
}

} // namespace mlibc
