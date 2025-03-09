#include <assert.h>
#include <bits/ensure.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <frg/optional.hpp>
#include <mlibc/debug.hpp>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt;

namespace {

int __optpos = 1;

enum GetoptMode {
	Short,
	Long,
	LongOnly,
};

int getopt_common_internal(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex, enum GetoptMode mode) {
	auto longopt_consume = [&](const char *arg, char *s, int k, bool colon) -> frg::optional<int> {
		// Consume the option and its argument.
		if(longopts[k].has_arg == required_argument) {
			if(s) {
				// Consume the long option and its argument.
				optarg = s + 1;
				optind++;
			}else if(argv[optind + 1]) {
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

			// Consume the long option.
			optind++;
		}

		return frg::null_opt;
	};

	bool colon = optstring[0] == ':';
	bool stop_at_first_nonarg = (optstring[0] == '+' || getenv("POSIXLY_CORRECT"));

	auto isOptionArg = [](char *arg){
		// If the first character of arg '-', and the arg is not exactly
		// equal to "-" or "--", then the arg is an option argument.
		return arg[0] == '-' && strcmp(arg, "-") && strcmp(arg, "--");
	};

	while(optind < argc) {
		char *arg = argv[optind];
		if(!isOptionArg(arg)) {
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

		if(arg[1] == '-') {
			arg += 2;

			// Determine the end of the option name (vs. the start of the argument).
			auto s = strchr(arg, '=');
			size_t n = s ? (s - arg) : strlen(arg);

			int k = -1;
			for(int i = 0; longopts[i].name; i++) {
				if(strncmp(arg, longopts[i].name, n) || longopts[i].name[n])
					continue;

				if(k >= 0) {
					if(opterr)
						fprintf(stderr, "Multiple option declaration detected: %s\n", arg);
					return '?';
				}
				k = i;
			}

			if(k == -1) {
				if(opterr)
					fprintf(stderr, "--%s is not a valid option.\n", arg);
				return '?';
			}

			if(longindex)
				*longindex = k;

			if(auto r = longopt_consume(arg, s, k, colon); r)
				return r.value();

			if(!longopts[k].flag) {
				return longopts[k].val;
			}else{
				*longopts[k].flag = longopts[k].val;
				return 0;
			}
		}else{
			/* handle short options, i.e. options with only one dash prefixed; e.g. `program -s` */
			unsigned int i = __optpos;
			while(true) {
				if(mode == GetoptMode::LongOnly) {
					const char *lo_arg = &arg[1];
					auto s = strchr(lo_arg, '=');
					size_t n = s ? (s - lo_arg) : strlen(lo_arg);
					int k = -1;

					for(int longopt = 0; longopts[longopt].name; longopt++) {
						if(strncmp(lo_arg, longopts[longopt].name, n) || longopts[longopt].name[n])
							continue;

						if(k >= 0) {
							if(opterr)
								fprintf(stderr, "Multiple option declaration detected: %s\n", arg);
							return '?';
						}

						k = longopt;
					}

					if(k != -1) {
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
					if(opt[1] == ':') {
						bool required = (opt[2] != ':');

						if(arg[i+1]) {
							optarg = arg + i + 1;
						} else if(optind + 1 < argc && argv[optind + 1] && (required || argv[optind + 1][0] != '-')) {
							/* there is an argument to this short option, separated by a space */
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

int getopt_common(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex, enum GetoptMode mode) {
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

}

int getopt_long(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
	return getopt_common(argc, argv, optstring, longopts, longindex, GetoptMode::Long);
}

int getopt_long_only(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
	return getopt_common(argc, argv, optstring, longopts, longindex, GetoptMode::LongOnly);
}
