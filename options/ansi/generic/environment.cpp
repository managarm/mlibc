#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>

#include <frg/string.hpp>
#include <frg/vector.hpp>

namespace {
	char *empty_environment[] = { nullptr };
}

char **environ = empty_environment;

namespace {

size_t find_environ_index(frg::string_view name) {
	for(size_t i = 0; environ[i]; i++) {
		frg::string_view view{environ[i]};
		size_t s = view.find_first('=');
		if(s == size_t(-1)) {
			mlibc::infoLogger() << "mlibc: environment string \""
					<< frg::escape_fmt{view.data(), view.size()}
					<< "\" does not contain an equals sign (=)" << frg::endlog;
			continue;
		}
		if(view.sub_string(0, s) == name)
			return i;
	}

	return -1;
}

// Environment vector that is mutated by putenv() and setenv().
// Cannot be global as it is accessed during library initialization.
frg::vector<char *, MemoryAllocator> &get_vector() {
	static frg::vector<char *, MemoryAllocator> vector{getAllocator()};
	return vector;
}

void update_vector() {
	auto &vector = get_vector();
	if(environ == vector.data())
		return;

	// If the environ variable was changed, we copy the environment.
	// Note that we must only copy the pointers but not the strings themselves!
	vector.clear();
	for(size_t i = 0; environ[i]; i++)
		vector.push(environ[i]);
	vector.push(nullptr);

	environ = vector.data();
}

void assign_variable(frg::string_view name, const char *string, bool overwrite) {
	auto &vector = get_vector();
	__ensure(environ == vector.data());

	auto k = find_environ_index(name);
	if(k != size_t(-1)) {
		if(overwrite)
			vector[k] = const_cast<char *>(string);
	}else{
		// Last pointer of environ must always be a null delimiter.
		__ensure(!vector.back());
		vector.back() = const_cast<char *>(string);
		vector.push(nullptr);
	}

	// push() might have re-allocated the vector.
	environ = vector.data();
}

void unassign_variable(frg::string_view name) {
	auto &vector = get_vector();
	__ensure(environ == vector.data());

	auto k = find_environ_index(name);
	if(k == size_t(-1))
		return;

	// Last pointer of environ must always be a null delimiter.
	__ensure(vector.size() >= 2 && !vector.back());
	std::swap(vector[k], vector[vector.size() - 2]);
	vector.pop();
	vector.back() = nullptr;

	// pop() might have re-allocated the vector.
	environ = vector.data();
}

} // anonymous namespace

char *getenv(const char *name) {
	auto k = find_environ_index(name);
	if(k == size_t(-1))
		return nullptr;

	frg::string_view view{environ[k]};
	size_t s = view.find_first('=');
	__ensure(s != size_t(-1));
	return const_cast<char *>(view.data() + s + 1);
}

namespace mlibc {

int putenv(char *string) {
	frg::string_view view{string};
	size_t s = view.find_first('=');
	if(s == size_t(-1))
		__ensure(!"Environment strings need to contain an equals sign");

	update_vector();
	assign_variable(view.sub_string(0, s), string, true);
	return 0;
}

} // namespace mlibc

#if __MLIBC_POSIX_OPTION

int putenv(char *string) {
	return mlibc::putenv(string);
}

int setenv(const char *name, const char *value, int overwrite) {
	frg::string_view view{name};
	size_t s = view.find_first('=');
	if(s != size_t(-1)) {
		mlibc::infoLogger() << "mlibc: environment variable \""
				<< frg::escape_fmt{view.data(), view.size()} << "\" contains an equals sign"
				<< frg::endlog;
		errno = EINVAL;
		return -1;
	}

	// We never free strings here. TODO: Reuse them?
	char *string;
	__ensure(asprintf(&string, "%s=%s", name, value) > 0);
	__ensure(string);

	update_vector();
	assign_variable(name, string, overwrite);
	return 0;
}

int unsetenv(const char *name) {
	update_vector();
	unassign_variable(name);
	return 0;
}

int clearenv(void) {
	auto vector = get_vector();
	vector.clear();
	update_vector();
	return 0;
}

#endif /* __MLIBC_POSIX_OPTION */
