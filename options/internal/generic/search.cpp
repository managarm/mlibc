#include <mlibc/search.hpp>
#include <frg/string.hpp>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

struct _ENTRY {
	ENTRY entry;
	bool used;
};

namespace mlibc {

int hcreate_r(size_t num_entries, struct hsearch_data *htab) {
	if(!htab) {
		errno = EINVAL;
		return 0;
	}

	htab->table = static_cast<_ENTRY*>(calloc(num_entries, sizeof(_ENTRY)));
	if(!htab->table) {
		errno = ENOMEM;
		return 0;
	}
	htab->filled = 0;
	htab->size = num_entries;
	return 1;
}

void hdestroy_r(struct hsearch_data *htab) {
	if(!htab) {
		errno = EINVAL;
		return;
	}
	free(htab->table);
	htab->table = nullptr;
	htab->size = 0;
	htab->filled = 0;
}


int hsearch_r(ENTRY item, ACTION action, ENTRY **ret, struct hsearch_data *htab) {
	auto key = frg::string_view{item.key};
	auto hash = frg::hash<frg::string_view>{}(key);

	size_t bucket_index = hash % htab->size;
	size_t start = bucket_index;
	while(true) {
		auto &bucket = htab->table[bucket_index];

		if(bucket.used) {
			if(bucket.entry.key == key) {
				*ret = &bucket.entry;
				return 1;
			}
		} else if(action == FIND) {
			errno = ESRCH;
			*ret = nullptr;
			return 0;
		}

		bucket_index = (bucket_index + 1) % htab->size;

		if(bucket_index == start) {
			if(action == FIND) {
				errno = ESRCH;
				*ret = nullptr;
				return 0;
			} else {
				break;
			}
		}
	}

	// insert a new entry.
	if(htab->size == htab->filled) {
		errno = ENOMEM;
		return 0;
	}
	++htab->filled;

	bucket_index = start;
	while(true) {
		auto &bucket = htab->table[bucket_index];
		if(!bucket.used) {
			bucket.used = true;
			bucket.entry = item;
			*ret = &bucket.entry;
			break;
		}

		bucket_index = (bucket_index + 1) % htab->size;
	}

	return 1;
}

} // namespace mlibc
