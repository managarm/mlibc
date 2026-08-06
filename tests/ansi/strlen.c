#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef size_t (*strlen_func)(const char *);

struct strlen_impl {
	const char *name;
	strlen_func func;
};

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
size_t __mlibc_strlen_default(const char *s);

#if defined(__x86_64__)
size_t __mlibc_strlen_sse2(const char *s);
size_t __mlibc_strlen_avx2(const char *s);
#endif
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC

static struct strlen_impl impls[] = {
	{"libc-provided strlen", strlen},

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
	{"mlibc default fallback", __mlibc_strlen_default},

#if defined(__x86_64__)
	{"x86_64 sse2", __mlibc_strlen_sse2},
	{"x86_64 avx2", __mlibc_strlen_avx2},
#endif
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC
};

const size_t num_impls = sizeof(impls) / sizeof(impls[0]);

int main() {
	char short_buffer[128];
	for (int i = 0; i < 128; i++) {
		short_buffer[i] = 'A';
	}

	for (int align = 0; align < 64; align++) {
		for (size_t len = 0; len < 64; len++) {
			char *ptr = short_buffer + align;
			char old_val = ptr[len];
			ptr[len] = '\0';

			for (size_t i = 0; i < num_impls; i++) {
				size_t ret = impls[i].func(ptr);
				if (ret != len)
					fprintf(stderr, "FAIL: '%s' returned %zu instead of %zu\n", impls[i].name, ret, len);
				assert(ret == len);
			}
			ptr[len] = old_val;
		}
	}

	size_t base_size = 1048576;
	size_t alloc_size = base_size + 128;
	char *large_buffer = malloc(alloc_size);
	if (!large_buffer) {
		perror("malloc failed");
		return 1;
	}

	unsigned int seed = 0x1337ACED;
	for (size_t i = 0; i < alloc_size; i++) {
		seed = seed * 1103515245 + 12345;
		// Map to printable character range [0x21, 0x7E] to guarantee no '\0'
		large_buffer[i] = (char)((seed % (0x7E - 0x21 + 1)) + 0x21);
	}

	size_t large_lengths[] = {
	    4096, 4097, 8192, 8191, 16384, 65536, base_size - 128, base_size - 65, base_size - 1
	};
	size_t num_lengths = sizeof(large_lengths) / sizeof(large_lengths[0]);

	for (int align = 0; align < 64; align++) {
		for (size_t l_idx = 0; l_idx < num_lengths; l_idx++) {
			size_t target_len = large_lengths[l_idx];
			char *ptr = large_buffer + align;

			char old_val = ptr[target_len];
			ptr[target_len] = '\0';

			for (size_t i = 0; i < num_impls; i++) {
				size_t ret = impls[i].func(ptr);
				if (ret != target_len)
					fprintf(stderr, "FAIL: '%s' at alignment %d, multi-page length %zu returned %zu\n",
							impls[i].name, align, target_len, ret);
				assert(ret == target_len);
			}

			ptr[target_len] = old_val;
		}
	}

	free(large_buffer);
	return 0;
}
