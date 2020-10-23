
#include <grp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>

namespace {
	thread_local group global_entry;

	template<typename F>
	void walk_segments(frg::string_view line, char delimiter, F fn) {
		size_t s = 0;
		while(true) {
			size_t d = line.find_first(':', s);
			if(d == size_t(-1))
				break;
			auto chunk = line.sub_string(s, d - s);
			fn(chunk);
			s = d + 1;
		}
		if(line[s]) {
			auto chunk = line.sub_string(s, line.size() - s);
			fn(chunk);
		}
	}

	bool extract_entry(frg::string_view line, group *entry) {
		__ensure(!entry->gr_name);
		__ensure(!entry->gr_mem);

		frg::string_view segments[5];

		// Parse the line into exactly 4 segments.
		size_t s = 0;
		int n;
		for(n = 0; n < 4; n++) {
			size_t d = line.find_first(':', s);
			if(d == size_t(-1))
				break;
			segments[n] = line.sub_string(s, d - s);
			s = d + 1;
		}
		if(line.find_first(':', s) != size_t(-1))
			return false;
		segments[n] = line.sub_string(s, line.size() - s);
		n++;

		if(n < 4)
			return false;

		// segments[1] is the password; it is not exported to struct group.
		// The other segments are consumed below.

		// TODO: Handle strndup() and malloc() failure.
		auto name = strndup(segments[0].data(), segments[0].size());
		__ensure(name);

		auto gid = segments[2].to_number<int>();
		if(!gid)
			return false;

		size_t n_members = 0;
		walk_segments(segments[3], ',', [&] (frg::string_view) {
			n_members++;
		});

		auto members = reinterpret_cast<char **>(malloc(sizeof(char *) * (n_members + 1)));
		__ensure(members);
		size_t k = 0;
		walk_segments(segments[3], ',', [&] (frg::string_view m) {
			members[k] = strndup(m.data(), m.size());
			__ensure(members[k]);
			k++;
		});
		members[k] = nullptr;

		entry->gr_name = name;
		entry->gr_gid = *gid;
		entry->gr_mem = members;
		return true;
	}

	void clear_entry(group *entry) {
		free(entry->gr_name);
		if(entry->gr_mem) {
			for(size_t i = 0; entry->gr_mem[i]; i++)
				free(entry->gr_mem[i]);
			free(entry->gr_mem);
		}
		entry->gr_name = nullptr;
		entry->gr_mem = nullptr;
	}

	template<typename C>
	group *walk_file(C cond) {
		auto file = fopen("/etc/group", "r");
		if(!file)
			return nullptr;

		char line[512];
		while(fgets(line, 512, file)) {
			clear_entry(&global_entry);
			if(!extract_entry(line, &global_entry))
				continue;
			if(cond(&global_entry)) {
				fclose(file);
				return &global_entry;
			}
		}

		fclose(file);
		errno = ESRCH;
		return nullptr;
	}
}

void endgrent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct group *getgrent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct group *getgrgid(gid_t gid) {
	return walk_file([&] (group *entry) {
		return entry->gr_gid == gid;
	});
}
int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct group *getgrnam(const char *name) {
	return walk_file([&] (group *entry) {
		return !strcmp(entry->gr_name, name);
	});
}
int getgrnam_r(const char *, struct group *, char *, size_t, struct group **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void setgrent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setgroups(size_t size, const gid_t *list) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int initgroups(const char *user, gid_t group) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
