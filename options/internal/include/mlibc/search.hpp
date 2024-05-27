#ifndef MLIBC_SEARCH
#define MLIBC_SEARCH

#include <bits/search.h>

namespace mlibc {

int hcreate_r(size_t num_entries, struct hsearch_data *htab);
void hdestroy_r(struct hsearch_data *htab);
int hsearch_r(ENTRY item, ACTION action, ENTRY **ret, struct hsearch_data *htab);

} // namespace mlibc

#endif // MLIBC_SEARCH
