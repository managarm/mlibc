
#include <grp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

namespace {
	FILE *global_file;

	bool open_global_file() {
		if(!global_file) {
			global_file = fopen("/etc/group", "r");
			if(!global_file) {
				errno = EIO;
				return false;
			}
		}

		return true;
	}

	void close_global_file() {
		if(global_file) {
			fclose(global_file);
			global_file = nullptr;
		}
	}

	template<typename F>
	void walk_segments(frg::string_view line, char delimiter, F fn) {
		size_t s = 0;
		while(true) {
			size_t d = line.find_first(delimiter, s);
			if(d == size_t(-1))
				break;
			auto chunk = line.sub_string(s, d - s);
			fn(chunk);
			s = d + 1;
		}
		if(line[s]) {
			auto chunk = line.sub_string(s, line.size() - s);

			if (chunk.size() > 0) {
				// Remove trailing newline
				if (chunk[chunk.size() - 1] == '\n')
					chunk = chunk.sub_string(0, chunk.size() - 1);

				fn(chunk);
			}
		}
	}

	bool extract_entry(frg::string_view line, group *entry) {
		frg::string_view segments[5];

		// Parse the line into 3 or 4 segments (depending if the group has members or not)
		int n = 0;
		walk_segments(line, ':', [&] (frg::string_view s) {
			__ensure(n < 4);
			segments[n++] = s;
		});

		if(n < 3) // n can be 3 when there are no members in the group
			return false;

		// TODO: Handle strndup() and malloc() failure.
		auto name = strndup(segments[0].data(), segments[0].size());
		__ensure(name);

		auto passwd = strndup(segments[1].data(), segments[1].size());

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
		entry->gr_passwd = passwd;
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
	int walk_file(struct group *entry, C cond) {
		auto file = fopen("/etc/group", "r");
		if(!file) {
			return EIO;
		}

		char line[512];
		while(fgets(line, 512, file)) {
			if(!extract_entry(line, entry))
				continue;
			if(cond(entry)) {
				fclose(file);
				return 0;
			}
		}

		int err = ESRCH;
		if(ferror(file)) {
			err = EIO;
		}

		fclose(file);
		return err;
	}

	int copy_to_buffer(struct group *grp, char *buffer, size_t size) {
		// Adjust to correct alignment so that we can put gr_mem first in buffer
		uintptr_t mask = sizeof(char *) - 1;
		size_t offset = (reinterpret_cast<uintptr_t>(buffer) % sizeof(char *) + mask) & ~mask;
		if (size < offset)
			return ERANGE;

		buffer += offset;
		size -= offset;

		// Calculate the amount of space we need
		size_t nmemb, required_size = 0;
		for (nmemb = 0; grp->gr_mem[nmemb] != nullptr; nmemb++) {
			// One for the string's null terminator and one for the pointer in gr_mem
			required_size += strlen(grp->gr_mem[nmemb]) + 1 + sizeof(char *);
		}

		// One for null terminator of gr_name, plus sizeof(char *) for nullptr terminator of gr_mem
		required_size += strlen(grp->gr_name) + 1 + sizeof(char *);
		if (size < required_size)
			return ERANGE;

		// Put the gr_mem array first in the buffer as we are guaranteed
		// that the pointer is aligned correctly
		char *string_data = buffer + (nmemb + 1) * sizeof(char *);

		for (size_t i = 0; i < nmemb; i++) {
			reinterpret_cast<char **>(buffer)[i] = string_data;
			string_data = stpcpy(string_data, grp->gr_mem[i]) + 1;
			free(grp->gr_mem[i]);
		}

		reinterpret_cast<char **>(buffer)[nmemb] = nullptr;
		free(grp->gr_mem);
		grp->gr_mem = reinterpret_cast<char **>(buffer);

		char *gr_name = stpcpy(string_data, grp->gr_name) + 1;
		free(grp->gr_name);
		grp->gr_name = string_data;

		__ensure(gr_name <= buffer + size);
		return 0;
	}
}

void endgrent(void) {
	close_global_file();
}

struct group *getgrent(void) {
	static group entry;
	char line[512];

	if(!open_global_file()) {
		return nullptr;
	}

	if(fgets(line, 512, global_file)) {
		clear_entry(&entry);
		if(!extract_entry(line, &entry)) {
			errno = EINVAL;
			return nullptr;
		}
		return &entry;
	}

	if(ferror(global_file)) {
		errno = EIO;
	}

	return nullptr;
}

struct group *getgrgid(gid_t gid) {
	static group entry;

	int err = walk_file(&entry, [&] (group *entry) {
		return entry->gr_gid == gid;
	});

	if (err) {
		errno = err;
		return nullptr;
	}

	return &entry;
}

int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t size, struct group **result) {
	*result = nullptr;
	int err = walk_file(grp, [&] (group *entry) {
		return entry->gr_gid == gid;
	});

	if (err) {
		return err;
	}

	err = copy_to_buffer(grp, buffer, size);
	if (err) {
		return err;
	}

	*result = grp;
	return 0;
}

struct group *getgrnam(const char *name) {
	static group entry;

	int err = walk_file(&entry, [&] (group *entry) {
		return !strcmp(entry->gr_name, name);
	});

	if (err) {
		errno = err;
		return nullptr;
	}

	return &entry;
}

int getgrnam_r(const char *name, struct group *grp, char *buffer, size_t size, struct group **result) {
	*result = nullptr;

	int err = walk_file(grp, [&] (group *entry) {
		return !strcmp(entry->gr_name, name);
	});

	if (err) {
		return err;
	}

	err = copy_to_buffer(grp, buffer, size);
	if (err) {
		return err;
	}

	*result = grp;
	return 0;
}

void setgrent(void) {
	if(!open_global_file()) {
		return;
	}
	rewind(global_file);
}

int setgroups(size_t size, const gid_t *list) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setgroups, -1);
	if(int e = mlibc::sys_setgroups(size, list); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int initgroups(const char *, gid_t) {
	mlibc::infoLogger() << "mlibc: initgroups is a stub" << frg::endlog;
	return 0;
}

int putgrent(const struct group *g, FILE *f) {
	auto invalid = [](const char *s) {
		return s == nullptr || strchr(s, '\n') || strchr(s, ':');
	};

	if(g == nullptr || invalid(g->gr_name) || invalid(g->gr_passwd)) {
		errno = EINVAL;
		return -1;
	}

	for(int i = 0; g->gr_mem != nullptr && g->gr_mem[i] != nullptr; ++i) {
		if (invalid(g->gr_mem[i])) {
			errno = EINVAL;
			return -1;
		}
	}

	// Taken from musl.
	flockfile(f);

	int r = fprintf(f, "%s:%s:%u:", g->gr_name, g->gr_passwd, g->gr_gid);
	if(r < 0)
		goto leave;

	for(int i = 0; g->gr_mem != nullptr && g->gr_mem[i] != nullptr; ++i) {
		r = fprintf(f, "%s%s", i ? "," : "", g->gr_mem[i]);
		if(r < 0)
			goto leave;
	}

	r = fputc('\n', f);

leave:
	funlockfile(f);
	return r < 0 ? -1 : 0;
}

struct group *fgetgrent(FILE *file) {
	static group entry;
	char line[512];

	if(fgets(line, 512, file)) {
		clear_entry(&entry);
		if(!extract_entry(line, &entry)) {
			errno = EINVAL;
			return nullptr;
		}
		return &entry;
	}

	if(ferror(file)) {
		errno = EIO;
	}

	return nullptr;
}

int getgrouplist(const char *user, gid_t gid, gid_t *groups, int *ngroups) {
	int n = 1;
	int n_limit = *ngroups;
	struct group grp;

	if(n_limit >= 1)
		*groups++ = gid;

	int err = walk_file(&grp, [&] (group *entry) {
		size_t i = 0;
		for(; entry->gr_mem[i] && strcmp(user, entry->gr_mem[i]); i++);
		if(!entry->gr_mem[i])
			return false;
		if(++n <= n_limit)
			*groups++ = entry->gr_gid;
		return false;
	});

	if(err != ESRCH) {
		errno = err;
		return -1;
	}

	*ngroups = n;

	return (n > n_limit) ? -1 : n;
}
