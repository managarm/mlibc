
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>

namespace {
	FILE *global_file; // Used by setpwent/getpwent/endpwent.

	bool open_global_file() {
		if(!global_file) {
			global_file = fopen("/etc/passwd", "r");
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

	bool extract_entry(frg::string_view line, passwd *entry) {
		frg::string_view segments[8];

		// Parse the line into 7 or 8 segments.
		size_t s = 0;
		int n;
		for(n = 0; n < 7; n++) {
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

		if(n < 7)
			return false;

		// TODO: Handle strndup() failure.
		auto name = strndup(segments[0].data(), segments[0].size());
		__ensure(name);

		auto passwd = strndup(segments[1].data(), segments[1].size());
		__ensure(passwd);

		auto uid = segments[2].to_number<int>();
		if(!uid)
			return false;
		auto gid = segments[3].to_number<int>();
		if(!gid)
			return false;

		auto real_name = strndup(segments[4].data(), segments[4].size());
		__ensure(real_name);
		auto dir = strndup(segments[5].data(), segments[5].size());
		__ensure(dir);
		auto shell = strndup(segments[6].data(), segments[6].size());
		__ensure(shell);

		// Chop the newline off the end of shell
		__ensure(strlen(shell) > 0);
		shell[strlen(shell) - 1] = '\0';

		entry->pw_name = name;
		entry->pw_passwd = passwd;
		entry->pw_uid = *uid;
		entry->pw_gid = *gid;
		entry->pw_dir = dir;
		entry->pw_shell = shell;
		entry->pw_gecos = real_name;
		return true;
	}

	void copy_to_buffer(passwd *pwd, char *buffer, size_t size) {
		char *pw_dir = stpcpy(buffer, pwd->pw_name) + 1;
		free(pwd->pw_name);
		pwd->pw_name = buffer;

		char *pw_shell = stpcpy(pw_dir, pwd->pw_dir) + 1;
		free(pwd->pw_dir);
		pwd->pw_dir = pw_dir;

		char *pw_passwd = stpcpy(pw_shell, pwd->pw_shell) + 1;
		free(pwd->pw_shell);
		pwd->pw_shell = pw_shell;

		char *end = stpcpy(pw_passwd, pwd->pw_passwd);
		__ensure(end <= buffer + size);
		free(pwd->pw_passwd);
		pwd->pw_passwd = pw_passwd;
	}

	void clear_entry(passwd *entry) {
		free(entry->pw_name);
		free(entry->pw_dir);
		free(entry->pw_passwd);
		free(entry->pw_shell);
		entry->pw_name = nullptr;
		entry->pw_dir = nullptr;
		entry->pw_passwd = nullptr;
		entry->pw_shell = nullptr;
	}
}

struct passwd *getpwent(void) {
	static passwd entry;
	char line[NSS_BUFLEN_PASSWD];

	if(!open_global_file()) {
		return nullptr;
	}

	if (fgets(line, NSS_BUFLEN_PASSWD, global_file)) {
		clear_entry(&entry);
		if(!extract_entry(line, &entry)) {
			errno = EINVAL;	// I suppose this can be a valid errno?
			return nullptr;
		}
		return &entry;
	}

	if(ferror(global_file)) {
		errno = EIO;
	}

	return nullptr;
}

struct passwd *getpwnam(const char *name) {
	static passwd entry;
	auto file = fopen("/etc/passwd", "r");
	if(!file)
		return nullptr;

	char line[NSS_BUFLEN_PASSWD];
	while(fgets(line, NSS_BUFLEN_PASSWD, file)) {
		clear_entry(&entry);
		if(!extract_entry(line, &entry))
			continue;
		if(!strcmp(entry.pw_name, name)) {
			fclose(file);
			return &entry;
		}
	}

	int err = errno;
	if(ferror(file)) {
		err = EIO;
	}

	fclose(file);
	errno = err;
	return nullptr;
}

int getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t size, struct passwd **result) {
	*result = nullptr;
	auto file = fopen("/etc/passwd", "r");
	if(!file) {
		return EIO;
	}

	char line[NSS_BUFLEN_PASSWD];
	while(fgets(line, NSS_BUFLEN_PASSWD, file)) {
		if(!extract_entry(line, pwd))
			continue;
		if(!strcmp(pwd->pw_name, name)) {
			fclose(file);

			size_t required_size = strlen(pwd->pw_name) + strlen(pwd->pw_dir)
				+ strlen(pwd->pw_shell) + strlen(pwd->pw_passwd) + 4;
			if (size < required_size)
				return ERANGE;

			copy_to_buffer(pwd, buffer, size);
			*result = pwd;
			return 0;
		}
	}

	int ret = 0;
	if(ferror(file)) {
		ret = EIO;
	}

	fclose(file);
	return ret;
}

struct passwd *getpwuid(uid_t uid) {
	static passwd entry;
	auto file = fopen("/etc/passwd", "r");
	if(!file)
		return nullptr;

	char line[NSS_BUFLEN_PASSWD];
	while(fgets(line, NSS_BUFLEN_PASSWD, file)) {
		clear_entry(&entry);
		if(!extract_entry(line, &entry))
			continue;
		if(entry.pw_uid == uid) {
			fclose(file);
			return &entry;
		}
	}

	int err = ESRCH;
	if(ferror(file)) {
		err = EIO;
	}

	fclose(file);
	errno = err;
	return nullptr;
}

int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t size, struct passwd **result) {
	*result = nullptr;
	auto file = fopen("/etc/passwd", "r");
	if(!file) {
		return EIO;
	}

	char line[NSS_BUFLEN_PASSWD];
	while(fgets(line, NSS_BUFLEN_PASSWD, file)) {
		if(!extract_entry(line, pwd))
			continue;
		if(pwd->pw_uid == uid) {
			fclose(file);

			size_t required_size = strlen(pwd->pw_name) + strlen(pwd->pw_dir)
				+ strlen(pwd->pw_shell) + + strlen(pwd->pw_passwd) + 4;
			if (size < required_size)
				return ERANGE;

			copy_to_buffer(pwd, buffer, size);
			*result = pwd;
			return 0;
		}
	}

	int ret = 0;
	if(ferror(file)) {
		ret = EIO;
	}

	fclose(file);
	return ret;
}

void setpwent(void) {
	if(!open_global_file()) {
		return;
	}
	rewind(global_file);
}

void endpwent(void) {
	close_global_file();
}

int putpwent(const struct passwd *p, FILE *f) {
	auto invalid = [](const char *s) {
		return s == nullptr || strchr(s, '\n') || strchr(s, ':');
	};

	if (p == nullptr || invalid(p->pw_name) || invalid(p->pw_passwd) || invalid(p->pw_gecos) || invalid(p->pw_dir) || invalid(p->pw_shell)) {
		errno = EINVAL;
		return -1;
	}

	// Taken from musl.
	return fprintf(f, "%s:%s:%u:%u:%s:%s:%s\n", p->pw_name, p->pw_passwd, p->pw_uid, p->pw_gid, p->pw_gecos, p->pw_dir, p->pw_shell) < 0 ? -1 : 0;
}

struct passwd *fgetpwent(FILE *file) {
	static passwd entry;
	char line[NSS_BUFLEN_PASSWD];


	if (fgets(line, NSS_BUFLEN_PASSWD, file)) {
		clear_entry(&entry);
		if(!extract_entry(line, &entry)) {
			errno = EINVAL;	// I suppose this can be a valid errno?
			return nullptr;
		}
		return &entry;
	}

	if(ferror(file)) {
		errno = EIO;
	}

	return nullptr;
}
