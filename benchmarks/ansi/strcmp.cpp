#include <benchmark/benchmark.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN (16384+8)

static char buf1[MAX_LEN];
static char buf2[MAX_LEN];

typedef int (*strcmp_func)(const char*, const char*);

extern "C" {
#if !USE_HOST_LIBC && !USE_CROSS_LIBC
int __mlibc_strcmp_default(const char *s1, const char *s2);

#if defined(__x86_64__)
int __mlibc_strcmp_sse2(const char *s1, const char *s2);
#endif // defined(__x86_64__)
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC
}

static void run_strcmp_bench(benchmark::State& state, strcmp_func func, bool equal) {
	size_t len = state.range(0);

	memset(buf1, 'a', MAX_LEN - 1);
	memset(buf2, 'a', MAX_LEN - 1);
	buf1[MAX_LEN - 1] = '\0';
	buf2[MAX_LEN - 1] = '\0';

	buf1[len] = '\0';
	buf2[len] = '\0';
	if (!equal && len > 0)
		buf2[len - 1] = 'c';

	const char *s1 = buf1 + 7;
	const char *s2 = buf2 + 7;

	for (auto _ : state) {
		int ret = func(s1, s2);
		benchmark::DoNotOptimize(ret);
		benchmark::ClobberMemory();
	}

	buf1[len] = 'a';
	buf2[len] = 'a';
	if (!equal && len > 0)
		buf2[len - 1] = 'a';
}

static void custom_args(benchmark::internal::Benchmark* b) {
	for (size_t len : {8, 16, 64, 128, 256, 1024, 4096, 16384}) {
		b->Arg(len);
	}
}

template <strcmp_func Func, bool Equal>
static void bench_strcmp(benchmark::State& state) {
	volatile strcmp_func f = Func;
	run_strcmp_bench(state, f, Equal);
}

BENCHMARK_TEMPLATE(bench_strcmp, strcmp, true)->Apply(custom_args);
BENCHMARK_TEMPLATE(bench_strcmp, strcmp, false)->Apply(custom_args);

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
BENCHMARK_TEMPLATE(bench_strcmp, __mlibc_strcmp_default, true)->Apply(custom_args);
BENCHMARK_TEMPLATE(bench_strcmp, __mlibc_strcmp_default, false)->Apply(custom_args);

#if defined(__x86_64__)
BENCHMARK_TEMPLATE(bench_strcmp, __mlibc_strcmp_sse2, true)->Apply(custom_args);
BENCHMARK_TEMPLATE(bench_strcmp, __mlibc_strcmp_sse2, false)->Apply(custom_args);
#endif // defined(__x86_64__)
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC
