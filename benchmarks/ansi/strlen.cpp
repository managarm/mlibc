#include <benchmark/benchmark.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN (16384+8)

static char buf[MAX_LEN];

typedef size_t (*strlen_func)(const char*);

extern "C" {
#if !USE_HOST_LIBC && !USE_CROSS_LIBC
size_t __mlibc_strlen_default(const char *s);

#if defined(__x86_64__)
size_t __mlibc_strlen_sse2(const char *s);
size_t __mlibc_strlen_avx2(const char *s);
#endif // defined(__x86_64__)
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC
}

static void run_strlen_bench(benchmark::State& state, strlen_func func) {
	size_t len = state.range(0);

	memset(buf, 'a', MAX_LEN - 1);
	buf[MAX_LEN - 1] = '\0';

	buf[len] = '\0';

	benchmark::DoNotOptimize(buf);
	benchmark::ClobberMemory();

	const char *s = buf + 7;

	for (auto _ : state) {
		size_t ret = func(s);
		benchmark::DoNotOptimize(ret);
		benchmark::ClobberMemory();
	}

	buf[len] = 'a';
}

static void custom_args(benchmark::internal::Benchmark* b) {
	for (size_t len : {8, 16, 64, 128, 256, 1024, 4096, 16384}) {
		b->Arg(len);
	}
}

template <strlen_func Func>
static void bench_strlen(benchmark::State& state) {
	volatile strlen_func f = Func;
	run_strlen_bench(state, f);
}

#if USE_HOST_LIBC || USE_CROSS_LIBC
BENCHMARK_TEMPLATE(bench_strlen, strlen)->Apply(custom_args);
#else
BENCHMARK_TEMPLATE(bench_strlen, __mlibc_strlen_default)->Apply(custom_args);

#if defined(__x86_64__)
BENCHMARK_TEMPLATE(bench_strlen, __mlibc_strlen_sse2)->Apply(custom_args);
BENCHMARK_TEMPLATE(bench_strlen, __mlibc_strlen_avx2)->Apply(custom_args);
#endif // defined(__x86_64__)
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC
