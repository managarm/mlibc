
struct __abi_tls_entry;

extern "C" void *__rtdl_get_tls(struct __abi_tls_entry *);

extern "C" void *__tls_get_addr(struct __abi_tls_entry *entry) {
	return __rtdl_get_tls(entry);
}

