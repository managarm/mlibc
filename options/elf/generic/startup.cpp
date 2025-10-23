
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <bits/ensure.h>
#include <mlibc/elf/startup.h>
#include <mlibc/environment.hpp>
#include <mlibc/rtld-config.hpp>
#include <mlibc/debug.hpp>

#include <frg/string.hpp>

extern "C" size_t __init_array_start[];
extern "C" size_t __init_array_end[];
extern "C" size_t __preinit_array_start[];
extern "C" size_t __preinit_array_end[];

extern "C" uintptr_t *__dlapi_entrystack();

namespace {
	const char *secure_ignore[] = {
		"LD_PRELOAD",
		"LD_LIBRARY_PATH",
		nullptr
	};
}

namespace mlibc {

exec_stack_data entry_stack;

[[gnu::constructor]]
void init_libc() {
	mlibc::parse_exec_stack(__dlapi_entrystack(), &entry_stack);
	mlibc::set_startup_data(entry_stack.argc, entry_stack.argv, entry_stack.envp);
}

void parse_exec_stack(void *opaque_sp, exec_stack_data *data) {
	auto sp = reinterpret_cast<uintptr_t *>(opaque_sp);
	data->argc = *sp++;
	data->argv = reinterpret_cast<char **>(sp);
	sp += data->argc; // Skip all arguments.
	__ensure(!*sp); // Skip the terminating null element.
	sp++;
	data->envp = reinterpret_cast<char **>(sp);
}

// TODO: This does not have to be here; we could also move it to options/internal.
void set_startup_data(int argc, char **argv, char **envp) {
	if(argc) {
		program_invocation_name = argv[0];

		if(auto slash = strrchr(argv[0], '/'); slash) {
			program_invocation_short_name = slash + 1;
		}else{
			program_invocation_short_name = argv[0];
		}
	}

	// Initialize environ.
	// TODO: Copy the arguments instead of pointing to them?
	auto should_ignore_when_secure = [](frg::string_view view) {
		auto it = secure_ignore;
		while(*it && view != *it)
			++it;

		return *it != nullptr;
	};

	auto ev = envp;
	while(*ev) {
		frg::string_view view{*ev};
		size_t s = view.find_first('=');
		if(s == size_t(-1)) {
			mlibc::panicLogger() << "mlibc: environment string \""
					<< frg::escape_fmt{view.data(), view.size()}
					<< "\" does not contain an equals sign (=)" << frg::endlog;
		}

		if(!mlibc::rtldConfig().secureRequired || !should_ignore_when_secure(view.sub_string(0, s))) {
			auto fail = mlibc::putenv(*ev);
			__ensure(!fail);
		}

		ev++;
	}
}

} // namespace mlibc

