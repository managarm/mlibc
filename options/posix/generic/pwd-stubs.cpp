
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>

namespace {
	thread_local passwd global_entry;

	bool extract_entry(frg::string_view line, passwd *entry) {
		__ensure(!entry->pw_name);
		__ensure(!entry->pw_dir);
		__ensure(!entry->pw_shell);

		frg::string_view segments[7];

		// Parse the line into exactly 6 segments.
		size_t s = 0;
		for(int i = 0; i < 6; i++) {
			size_t d = line.find_first(':', s);
			if(d == size_t(-1))
				return false;
			segments[i] = line.sub_string(s, d - s);
			s = d + 1;
		}
		if(line.find_first(':', s) != size_t(-1))
			return false;
		segments[6] = line.sub_string(s, line.size() - s);

		// segments[1] is the password and segments[4] is the user description; both of those
		// are not exported to struct passwd. The other segments are consumed below.

		// TODO: Handle strndup() failure.
		auto name = strndup(segments[0].data(), segments[0].size());
		__ensure(name);

		auto uid = segments[2].to_number<int>();
		if(!uid)
			return false;
		auto gid = segments[3].to_number<int>();
		if(!gid)
			return false;

		auto dir = strndup(segments[5].data(), segments[5].size());
		__ensure(dir);
		auto shell = strndup(segments[6].data(), segments[6].size());
		__ensure(shell);

		entry->pw_name = name;
		entry->pw_uid = *uid;
		entry->pw_gid = *gid;
		entry->pw_dir = dir;
		entry->pw_shell = shell;
		return true;
	}

	void clear_entry(passwd *entry) {
		free(entry->pw_name);
		free(entry->pw_dir);
		free(entry->pw_shell);
		entry->pw_name = nullptr;
		entry->pw_dir = nullptr;
		entry->pw_shell = nullptr;
	}
}

struct passwd *getpwent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct passwd *getpwnam(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct passwd *getpwuid(uid_t uid) {
	auto file = fopen("/etc/passwd", "r");
	if(!file)
		return nullptr;

	clear_entry(&global_entry);

	char line[512];
	while(fgets(line, 512, file)) {
		if(!extract_entry(line, &global_entry))
			continue;
		if(global_entry.pw_uid == uid) {
			fclose(file);
			return &global_entry;
		}
	}

	fclose(file);
	errno = ESRCH;
	return nullptr;
}

int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void setpwent(void) {
	__ensure(!"Not implemented");
}

void endpwent(void) { }

