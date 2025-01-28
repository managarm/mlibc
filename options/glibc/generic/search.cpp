#include <bits/glibc/glibc_search.h>
#include <mlibc/search.hpp>

int hcreate_r(size_t num_entries, hsearch_data *htab) {
	return mlibc::hcreate_r(num_entries, htab);
}

void hdestroy_r(hsearch_data *htab) {
	mlibc::hdestroy_r(htab);
}

int hsearch_r(ENTRY item, ACTION action, ENTRY **ret, hsearch_data *htab) {
	return mlibc::hsearch_r(item, action, ret, htab);
}
