
#include <stdlib.h>

#include <frigg/string.hpp>
#include <frigg/vector.hpp>
#include <mlibc/ensure.h>
#include <mlibc/frigg-alloc.hpp>

static char *emptyEnvironment[] = { nullptr };

char **environ = emptyEnvironment;

char *getenv(const char *name) {
	for(auto it = environ; *it; it++) {
		frigg::StringView view{*it};

		size_t s = view.findFirst('=');
		if(s == size_t(-1)) {
			frigg::infoLogger() << "\e[35mmlibc: getenv() environment string '"
					<< view << "' does not contain the '=' sign\e[39m" << frigg::endLog;
			continue;
		}
		if(view.subString(0, s) != name)
			continue;

		return const_cast<char *>(view.data() + s + 1);
	}
	return nullptr;
}

// Environment vector that is mutated by putenv() and setenv().
static frigg::Vector<char *, MemoryAllocator> mutEnvironment{getAllocator()};

int putenv(const char *string) {
	// If the environ variable was changed, we copy the environment.
	if(environ != mutEnvironment.data()) {
		// TODO: Actually copy the entries.
		__ensure(!*environ);
		mutEnvironment.push(nullptr);
	}

	// TODO: Replace the entry instead of adding it.
	__ensure(!mutEnvironment.back());
	mutEnvironment.back() = const_cast<char *>(string);
	mutEnvironment.push(nullptr);
	environ = mutEnvironment.data();
	return 0;
}

