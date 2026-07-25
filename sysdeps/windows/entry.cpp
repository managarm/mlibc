#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <mlibc/elf/startup.h>

namespace {

struct unicode_string {
	uint16_t length;
	uint16_t maximum_length;
	uint16_t *buffer;
};

struct rtl_user_process_parameters {
	uint8_t reserved[0x60];
	unicode_string image_path_name;
	unicode_string command_line;
	void *environment;
};

struct peb {
	uint8_t reserved[0x20];
	rtl_user_process_parameters *process_parameters;
};

struct string_list {
	char **strings;
	size_t count;
};

constexpr uint16_t first_high_surrogate = 0xD800;
constexpr uint16_t last_high_surrogate = 0xDBFF;
constexpr uint16_t first_low_surrogate = 0xDC00;
constexpr uint16_t last_low_surrogate = 0xDFFF;
constexpr uint32_t first_supplementary_codepoint = 0x10000;
constexpr uint32_t replacement_codepoint = 0xFFFD;

static_assert(offsetof(peb, process_parameters) == 0x20);
static_assert(offsetof(rtl_user_process_parameters, command_line) == 0x70);
static_assert(offsetof(rtl_user_process_parameters, environment) == 0x80);

peb *current_peb() {
	peb *result;
	asm volatile("movq %%gs:0x60, %0" : "=r"(result));
	return result;
}

[[noreturn]] void startup_failure() {
	__builtin_trap();
}

template<typename T>
T *allocate_array(size_t size) {
	auto result = static_cast<T *>(malloc(size * sizeof(T)));
	if(!result)
		startup_failure();
	return result;
}

bool is_high_surrogate(uint16_t code_unit) {
	return code_unit >= first_high_surrogate && code_unit <= last_high_surrogate;
}

bool is_low_surrogate(uint16_t code_unit) {
	return code_unit >= first_low_surrogate && code_unit <= last_low_surrogate;
}

uint32_t decode_surrogate_pair(uint16_t high, uint16_t low) {
	return first_supplementary_codepoint + ((high - first_high_surrogate) << 10) + (low - first_low_surrogate);
}

char *append_utf8_codepoint(char *output, uint32_t codepoint) {
	if(codepoint < 0x80) {
		*output++ = codepoint;
	}else if(codepoint < 0x800) {
		*output++ = 0xC0 | (codepoint >> 6);
		*output++ = 0x80 | (codepoint & 0x3F);
	}else if(codepoint < 0x10000) {
		*output++ = 0xE0 | (codepoint >> 12);
		*output++ = 0x80 | ((codepoint >> 6) & 0x3F);
		*output++ = 0x80 | (codepoint & 0x3F);
	}else{
		*output++ = 0xF0 | (codepoint >> 18);
		*output++ = 0x80 | ((codepoint >> 12) & 0x3F);
		*output++ = 0x80 | ((codepoint >> 6) & 0x3F);
		*output++ = 0x80 | (codepoint & 0x3F);
	}
	return output;
}

char *utf16_to_utf8(const uint16_t *input, size_t length) {
	auto utf8 = allocate_array<char>(length * 3 + 1);

	char *output = utf8;
	for(size_t i = 0; i < length; i++) {
		uint32_t codepoint = input[i];
		if(is_high_surrogate(input[i])) {
			if(i + 1 < length && is_low_surrogate(input[i + 1])) {
				auto high_surrogate = input[i];
				auto low_surrogate = input[++i];
				codepoint = decode_surrogate_pair(high_surrogate, low_surrogate);
			}else{
				codepoint = replacement_codepoint;
			}
		}else if(is_low_surrogate(input[i])) {
			codepoint = replacement_codepoint;
		}
		output = append_utf8_codepoint(output, codepoint);
	}
	*output = '\0';
	return utf8;
}

bool is_argument_separator(uint16_t character) {
	return character == ' ' || character == '\t';
}

uint16_t *append_backslashes(uint16_t *output, size_t count) {
	for(size_t i = 0; i < count; i++)
		*output++ = '\\';
	return output;
}

uint16_t *parse_argument(const uint16_t *&input, const uint16_t *end, uint16_t *output) {
	bool inside_quotes = false;
	while(input != end) {
		if(*input == '\\') {
			size_t backslash_count = 0;
			while(input != end && *input == '\\') {
				backslash_count++;
				input++;
			}

			if(input == end || *input != '"') {
				output = append_backslashes(output, backslash_count);
				continue;
			}

			output = append_backslashes(output, backslash_count / 2);
			if(backslash_count & 1)
				*output++ = *input++;
			else {
				inside_quotes = !inside_quotes;
				input++;
			}
		}else if(*input == '"') {
			inside_quotes = !inside_quotes;
			input++;
		}else if(!inside_quotes && is_argument_separator(*input)) {
			break;
		}else{
			*output++ = *input++;
		}
	}
	return output;
}

string_list build_arguments(const unicode_string &command_line) {
	auto code_unit_count = command_line.length / sizeof(uint16_t);
	auto strings = allocate_array<char *>(code_unit_count / 2 + 1);
	auto argument_buffer = allocate_array<uint16_t>(code_unit_count + 1);

	const uint16_t *input = command_line.buffer;
	const uint16_t *end = command_line.buffer + code_unit_count;
	size_t argument_count = 0;
	while(input != end) {
		while(input != end && is_argument_separator(*input))
			input++;
		if(input == end)
			break;

		auto output = parse_argument(input, end, argument_buffer);
		*output = '\0';
		strings[argument_count++] = utf16_to_utf8(argument_buffer,
				output - argument_buffer);
	}
	free(argument_buffer);
	return {strings, argument_count};
}

const uint16_t *find_string_end(const uint16_t *string) {
	while(*string)
		string++;
	return string;
}

string_list build_environment(const uint16_t *environment) {
	if(!environment)
		return {nullptr, 0};

	size_t variable_count = 0;
	for(auto input = environment; *input;) {
		variable_count++;
		input = find_string_end(input) + 1;
	}

	if(!variable_count)
		return {nullptr, 0};
	auto strings = allocate_array<char *>(variable_count);

	auto input = environment;
	for(size_t i = 0; i < variable_count; i++) {
		auto end = find_string_end(input);
		strings[i] = utf16_to_utf8(input, end - input);
		input = end + 1;
	}
	return {strings, variable_count};
}

uintptr_t *build_initial_stack() {
	auto parameters = current_peb()->process_parameters;
	if(!parameters)
		startup_failure();

	auto arguments = build_arguments(parameters->command_line);
	auto environment = build_environment(static_cast<uint16_t *>(parameters->environment));

	auto stack = allocate_array<uintptr_t>(arguments.count + environment.count + 3);
	auto output = stack;
	*output++ = arguments.count;
	for(size_t i = 0; i < arguments.count; i++)
		*output++ = reinterpret_cast<uintptr_t>(arguments.strings[i]);
	*output++ = 0;
	for(size_t i = 0; i < environment.count; i++)
		*output++ = reinterpret_cast<uintptr_t>(environment.strings[i]);
	*output = 0;

	free(arguments.strings);
	free(environment.strings);
	return stack;
}

uintptr_t *initial_stack;

} // namespace

extern "C" uintptr_t *__dlapi_entrystack() {
	if(!initial_stack)
		initial_stack = build_initial_stack();
	return initial_stack;
}

extern "C" void __mlibc_windows_init_tcb();
void __mlibc_windows_init_file_table();

extern "C" void __mlibc_windows_init_libc() {
	__mlibc_windows_init_tcb();
	__mlibc_windows_init_file_table();
}

extern "C" void __mlibc_run_ctors(uintptr_t *begin) {
	if(*begin == static_cast<uintptr_t>(-1))
		begin++;

	auto end = begin;
	while(*end)
		end++;

	while(end != begin) {
		auto function = reinterpret_cast<void (*)()>(*--end);
		function();
	}
}

extern "C" void __mlibc_entry(uintptr_t *constructors,
		int (*main_fn)(int argc, char *argv[], char *env[])) {
#if defined(MLIBC_STATIC_BUILD)
	__mlibc_windows_init_libc();
#endif
	__mlibc_run_ctors(constructors);
	auto result = main_fn(mlibc::entry_stack.argc, mlibc::entry_stack.argv,
			mlibc::entry_stack.envp);
	exit(result);
}
