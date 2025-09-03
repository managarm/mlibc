#include <assert.h>
#include <bits/ensure.h>
#include <bits/getopt.h>
#include <frg/optional.hpp>
#include <mlibc-config.h>
#include <mlibc/debug.hpp>
#include <mlibc/getopt.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <variant>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt;

namespace {

int __optpos = 1;

int getopt_common_internal(int argc, char * const argv[], const char *optstring, const struct option *longopts,
		int *longindex, enum mlibc::GetoptMode mode) {
	// find a matching longopt for an `arg` of length `n`
	// returns a size_t of the index of the matched longopt, preferring an exact over a partial match
	// returns a char of the error that getopt should return if multiple matches were available
	// returns a std::monostate if no longopt resulted in a exact or partial match
	auto longopt_find = [&](const char *arg, size_t n) -> std::variant<size_t, char, std::monostate> {
		assert(mode != mlibc::GetoptMode::Short);

		frg::optional<size_t> i = frg::null_opt;

		// first, attempt to find exactly one exact match
		for(size_t longopt = 0; longopts[longopt].name; longopt++) {
			if(strncmp(arg, longopts[longopt].name, n) || longopts[longopt].name[n])
				continue;

			if(i) {
				if(opterr)
					fprintf(stderr, "Multiple option declaration detected: %s\n", arg);
				optind++;
				return '?';
			}

			i = longopt;
		}

		if(i)
			return *i;

		// because no exact match was found, we now search for longopts with partial matches
		for(size_t longopt = 0; longopts[longopt].name; longopt++) {
			if(strncmp(arg, longopts[longopt].name, n))
				continue;

			if(i) {
				if(opterr)
					fprintf(stderr, "Multiple option declaration detected: %s\n", arg);
				optind++;
				return '?';
			}

			i = longopt;
		}

		if(i)
			return *i;

		return std::monostate{};
	};

	auto longopt_consume = [&](const char *arg, char *s, int k, bool colon) -> frg::optional<int> {
		assert(mode != mlibc::GetoptMode::Short);

		// Consume the option and its argument.
		if(longopts[k].has_arg == required_argument) {
			if(s) {
				// Consume the long option and its argument.
				optarg = s + 1;
				optind++;
			}else if(optind + 1 < argc && argv[optind + 1]) {
				// Consume the long option.
				optind++;

				// Consume the option's argument.
				optarg = argv[optind];
				optind++;
			}else{
				/*	If an error was detected, and the first character of optstring is not a colon,
					and the external variable opterr is nonzero (which is the default),
					getopt() prints an error message. */
				if(!colon && opterr)
					fprintf(stderr, "--%s requires an argument.\n", arg);

				optopt = longopts[k].val;
				optind++;

				/*	If the first character of optstring is a colon (':'), then getopt()
					returns ':' instead of '?' to indicate a missing option argument. */
				return colon ? ':' : '?';
			}
		}else if(longopts[k].has_arg == optional_argument) {
			if(s) {
				// Consume the long option and its argument.
				optarg = s + 1;
				optind++;
			}else{
				// Consume the long option.
				optarg = nullptr;
				optind++;
			}
		}else{
			__ensure(longopts[k].has_arg == no_argument);

			// did we get passed a value?
			if(s && strlen(s)) {
				optind++;
				return colon ? ':' : '?';
			}

			// Consume the long option.
			optind++;
			optarg = nullptr;
		}

		return frg::null_opt;
	};

	bool colon = optstring[0] == ':';
	bool stop_at_first_nonarg = (optstring[0] == '+' || getenv("POSIXLY_CORRECT"));

	// if optstring contains "W;", then "-W foo" is treated as the long option "--foo".
	bool w_long_options = [&]{
		if(mode == mlibc::GetoptMode::Short)
			return false;

		const char *W = strchr(optstring, 'W');
		if (!W)
			return false;
		return W[1] == ';';
	}();

	auto isOptionArg = [](char *arg){
		// If the first character of arg '-', and the arg is not exactly
		// equal to "-" or "--", then the arg is an option argument.
		return arg[0] == '-' && strcmp(arg, "-") && strcmp(arg, "--");
	};

	while(optind < argc) {
		char *arg = argv[optind];
		if(!isOptionArg(arg)) {
			if(!strcmp(arg, "--")) {
				optind++;
				return -1;
			}

			if(stop_at_first_nonarg) {
				optarg = nullptr;
				return -1;
			}

			bool further_options = false;
			int skip = optind;

			for(; skip < argc; ++skip) {
				if(isOptionArg(argv[skip])) {
					further_options = true;
					break;
				}
			}

			if(further_options) {
				optind += skip - optind;
				continue;
			} else {
				optarg = nullptr;
				return -1;
			}
		}

		if(arg[1] == '-' && mode != mlibc::GetoptMode::Short) {
			arg += 2;

			// Determine the end of the option name (vs. the start of the argument).
			auto s = strchr(arg, '=');
			size_t n = s ? (s - arg) : strlen(arg);

			auto k = longopt_find(arg, n);
			if(std::holds_alternative<std::monostate>(k)) {
				if(opterr)
					fprintf(stderr, "--%s is not a valid option.\n", arg);
				optind++;
				return '?';
			} else if(std::holds_alternative<char>(k)) {
				return std::get<char>(k);
			}

			if(longindex)
				*longindex = std::get<size_t>(k);

			if(auto r = longopt_consume(arg, s, std::get<size_t>(k), colon); r)
				return r.value();

			if(!longopts[std::get<size_t>(k)].flag) {
				return longopts[std::get<size_t>(k)].val;
			}else{
				*longopts[std::get<size_t>(k)].flag = longopts[std::get<size_t>(k)].val;
				return 0;
			}
		}else{
			/* handle short options, i.e. options with only one dash prefixed; e.g. `program -s` */
			unsigned int i = __optpos;
			while(true) {
				if(mode == mlibc::GetoptMode::LongOnly) {
					const char *lo_arg = &arg[1];
					auto s = strchr(lo_arg, '=');
					size_t n = s ? (s - lo_arg) : strlen(lo_arg);

					auto longopt_res = longopt_find(lo_arg, n);

					if(std::holds_alternative<char>(longopt_res)) {
						return std::get<char>(longopt_res);
					} else if(std::holds_alternative<size_t>(longopt_res)) {
						auto k = std::get<size_t>(longopt_res);
						if(auto r = longopt_consume(lo_arg, s, k, colon); r)
							return r.value();

						if(!longopts[k].flag) {
							return longopts[k].val;
						}else{
							*longopts[k].flag = longopts[k].val;
							return 0;
						}
					}
				}

				auto opt = strchr(optstring, arg[i]);
				if(opt) {
					if(opt[0] == 'W' && w_long_options) {
						const char *lo_arg = [&]() {
							if(opt[1]) {
								return &arg[i] + 1;
							} else {
								return &arg[i + 1];
							}
						}();

						auto s = strchr(lo_arg, '=');
						size_t n = s ? (s - lo_arg) : strlen(lo_arg);

						if(!n) {
							optopt = 'W';
							optind++;
							return colon ? ':' : '?';
						}

						auto longopt_res = longopt_find(lo_arg, n);

						if(std::holds_alternative<char>(longopt_res)) {
							return std::get<char>(longopt_res);
						} else if(std::holds_alternative<size_t>(longopt_res)) {
							auto k = std::get<size_t>(longopt_res);
							if(auto r = longopt_consume(lo_arg, s, k, colon); r)
								return r.value();

							if(!longopts[k].flag) {
								return longopts[k].val;
							}else{
								*longopts[k].flag = longopts[k].val;
								return 0;
							}
						} else {
							optind++;
							return colon ? ':' : '?';
						}
					} else if(opt[1] == ':') {
						// one colon means the option requires an argument
						// two colons mean the option takes an optional argument as part of the
						// same argv element (in the same word as the option name itself)
						bool required = (opt[2] != ':');

						if(arg[i+1]) {
							optarg = arg + i + 1;
						} else if(optind + 1 < argc && argv[optind + 1] && required && argv[optind + 1][0] != '-') {
							/* there is an argument to this short option, separated by a space,
								* and the shortopt specification does not specify an optional arg */
							optarg = argv[optind + 1];
							optind++;
							__optpos = 1;
						} else if(!required) {
							optarg = nullptr;
						} else {
							__optpos = 1;
							optopt = arg[i];
							return colon ? ':' : '?';
						}
						optind++;
					} else {
						if(arg[i+1]) {
							__optpos++;
						} else if(arg[i]) {
							optind++;
						} else {
							optarg = nullptr;
							return -1;
						}
					}

					return arg[i];
				} else {
					/*	If getopt() does not recognize an option character, it prints an error message to stderr,
						stores the character in optopt, and returns '?'. The calling program may prevent
						the error message by setting opterr to 0. */
					optopt = arg[1];
					if(opterr)
						fprintf(stderr, "%s is not a valid option.\n", arg);
					optind++;
					return '?';
				}
			}
		}
	}

	optarg = nullptr;
	return -1;
}

void permute(char **argv, int dest, int src) {
	assert(src > dest);
	char *tmp = argv[src];
	for(int i = src; i > dest; i--) {
		argv[i] = argv[i - 1];
	}
	argv[dest] = tmp;
}

} // namespace

#if __MLIBC_BSD_OPTION
extern "C" int optreset;
#endif /*__MLIBC_BSD_OPTION */

namespace mlibc {

int getopt_common(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex, enum GetoptMode mode) {
	// glibc extension: Setting optind to zero causes a full reset.
	// TODO: Should we really reset opterr and the other flags?
	if(!optind
#if __MLIBC_BSD_OPTION
		|| optreset
#endif //__MLIBC_BSD_OPTION
		) {
		optarg = nullptr;
		optind = 1;
		optopt = 0;
		__optpos = 1;
#if __MLIBC_BSD_OPTION
		optreset = 0;
#endif //__MLIBC_BSD_OPTION
	}

	int skipped = optind;

	if(optstring[0] != '+' && optstring[0] != '-') {
		int i = optind;

		for(;; i++) {
			if(i >= argc || !argv[i]) {
				optarg = nullptr;
				return -1;
			}
			if(argv[i][0] == '-' && argv[i][1])
				break;
		}

		optind = i;
	}

	int resumed = optind;
	auto ret = getopt_common_internal(argc, argv, optstring, longopts, longindex, mode);

	if(resumed > skipped) {
		for(int i = 0; i < (optind - resumed); i++) {
			permute(const_cast<char **>(argv), skipped, optind - 1);
		}
		optind = skipped + (optind - resumed);
	}

	return ret;
}

} // namespace mlibc
