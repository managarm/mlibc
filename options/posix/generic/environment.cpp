
#include <stdlib.h>
#include <stdio.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>

#include <frg/string.hpp>
#include <frg/vector.hpp>

static char *emptyEnvironment[] = { nullptr };

char **environ = emptyEnvironment;

namespace {

// Environment vector that is mutated by putenv() and setenv().
static frg::vector<char *, MemoryAllocator> global_env_vector{getAllocator()};

void update_env_copy() {
	if(environ == global_env_vector.data())
		return;
	
	// If the environ variable was changed, we copy the environment.
	// Note that we must only copy the pointers but not the strings themselves!
	__ensure(!*environ); // TODO: Actually copy the entries.
	global_env_vector.push(nullptr);
	
	environ = global_env_vector.data();
}

void fix_env_pointer() {
	environ = global_env_vector.data();
}

size_t find_env_index(frg::string_view name) {
	__ensure(environ == global_env_vector.data());
	__ensure(!global_env_vector.empty());

	for(size_t i = 0; global_env_vector[i]; i++) {
		frg::string_view view{global_env_vector[i]};
		size_t s = view.find_first('=');
		__ensure(s != size_t(-1));
		if(view.sub_string(0, s) == name)
			return i;
	}

	return -1;
}

} // anonymous namespace

char *getenv(const char *name) {
	// TODO: We do not necessarily need this.
	update_env_copy();

	auto k = find_env_index(name);
	if(k == size_t(-1))
		return nullptr;
	
	frg::string_view view{global_env_vector[k]};
	size_t s = view.find_first('=');
	__ensure(s != size_t(-1));
	return const_cast<char *>(view.data() + s + 1);
}

int putenv(const char *string) {
	update_env_copy();

	frg::string_view view{string};
	size_t s = view.find_first('=');
	if(s == size_t(-1))
		__ensure(!"Environment strings need to contain an equals sign");
	
	auto k = find_env_index(view.sub_string(0, s));
	if(k != size_t(-1)) {
		__ensure(!"Implement enviornment variable replacement");
	}else{
		__ensure(!global_env_vector.back()); // Last pointer must always be a null delimiter.
		global_env_vector.back() = const_cast<char *>(string);
		global_env_vector.push(nullptr);
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
	FRG_ASSERT(k != size_t(-1));

	// Last pointer is always null.
	__ensure(global_env_vector.size() >= 2 && !global_env_vector.back());
	std::swap(global_env_vector[k], global_env_vector[global_env_vector.size() - 2]);
	global_env_vector.pop();
	global_env_vector.back() = nullptr;

	return 0;
}

