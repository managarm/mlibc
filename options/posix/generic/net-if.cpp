#include <errno.h>
#include <net/if.h>

#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>

#if __MLIBC_LINUX_OPTION
#include <generic-helpers/netlink.hpp>
#endif // __MLIBC_LINUX_OPTION

void if_freenameindex(struct if_nameindex *p) {
#if __MLIBC_LINUX_OPTION
	if constexpr (mlibc::SysdepTraits::usesRtNetlink) {
		if (!p)
			return;

		for (auto c = p; c->if_index || c->if_name; c++) {
			if (c->if_name)
				getAllocator().free(c->if_name);
		}

		getAllocator().free(p);
		return;
	}
#endif

	if constexpr (mlibc::IsImplemented<IfFreeNameindex>)
		mlibc::sysdep_or_panic<IfFreeNameindex>(p);
}

char *if_indextoname(unsigned int index, char *name) {
	if(int e = mlibc::sysdep_or_enosys<IfIndextoname>(index, name); e) {
		errno = e;
		return nullptr;
	}

	return name;
}

struct if_nameindex *if_nameindex(void) {
#if __MLIBC_LINUX_OPTION
	if constexpr (mlibc::SysdepTraits::usesRtNetlink) {
		NetlinkHelper nl;
		frg::vector<struct if_nameindex, MemoryAllocator> list{getAllocator()};

		bool link_ret = nl.send_request(RTM_GETLINK) && nl.recv(&if_nameindex_callback, &list);
		__ensure(link_ret);

		list.emplace_back(0, nullptr);
		auto ret = list.data();
		list.detach();

		return ret;
	}
#endif

	struct if_nameindex *out = nullptr;
	if(int e = mlibc::sysdep_or_enosys<IfNameindex>(&out); e) {
		errno = e;
		return nullptr;
	}

	return out;
}

unsigned int if_nametoindex(const char *name) {
	unsigned int ret = 0;

	if(int e = mlibc::sysdep_or_enosys<IfNametoindex>(name, &ret); e) {
		errno = e;
		return 0;
	}

	return ret;
}
