
#include <stdlib.h>
#include <stdio.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>

#include <frigg/string.hpp>
#include <frigg/vector.hpp>

static char *emptyEnvironment[] = { nullptr };

char **environ = emptyEnvironment;

namespace {

// Environment vector that is mutated by putenv() and setenv().
static frigg::Vector<char *, MemoryAllocator> env_vector{getAllocator()};

void update_env_copy() {
	if(environ == env_vector.data())
		return;
	
	// If the environ variable was changed, we copy the environment.
	// Note that we must only copy the pointers but not the strings themselves!
	__ensure(!*environ); // TODO: Actually copy the entries.
	env_vector.push(nullptr);
	
	environ = env_vector.data();
}

void fix_env_pointer() {
	environ = env_vector.data();
}

size_t find_env_index(frigg::StringView name) {
	__ensure(environ == env_vector.data());
	__ensure(!env_vector.empty());

	for(size_t i = 0; env_vector[i]; i++) {
		frigg::StringView view{env_vector[i]};
		size_t s = view.findFirst('=');
		__ensure(s != size_t(-1));
		if(view.subString(0, s) == name)
			return i;
	}

	return -1;
}

} // anonymous namespace

char *getenv(const char *name) {
	auto k = find_env_index(name);
	if(k == size_t(-1))
		return nullptr;
	
	frigg::StringView view{env_vector[k]};
	size_t s = view.findFirst('=');
	__ensure(s != size_t(-1));
	return const_cast<char *>(view.data() + s + 1);
}

int putenv(const char *string) {
	update_env_copy();

	frigg::StringView view{string};
	size_t s = view.findFirst('=');
	if(s == size_t(-1))
		__ensure(!"Environment strings need to contain an equals sign");
	
	auto k = find_env_index(view.subString(0, s));
	if(k != size_t(-1)) {
		__ensure(!"Implement enviornment variable replacement");
	}else{
		__ensure(!env_vector.back()); // Last pointer must always be a null delimiter.
		env_vector.back() = const_cast<char *>(string);
		env_vector.push(nullptr);
		fix_env_pointer();
	}

	return 0;
}

int setenv(const char *name, const char *value, int overwrite) {
	// We never free strings here.
	// TODO: Reuse them?
	__ensure(overwrite);
	char *string;
	__ensure(asprintf(&string, "%s=%s", name, value) > 0);
	__ensure(string);

	return putenv(string);
}

int unsetenv(const char *name) {
	update_env_copy();

	auto k = find_env_index(name);
	assert(k != size_t(-1));

	__ensure(env_vector.size() >= 2 && !env_vector.back()); // Last pointer is always null.
	frigg::swap(env_vector[k], env_vector[env_vector.size() - 2]);
	env_vector.pop();
	env_vector.back() = nullptr;

	return 0;
}

