

#include <assert.h>
#include <bits/ensure.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mlibc/debug.hpp>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt;

int __optreset = 0;
extern int optreset __attribute__((weak, alias ("__optreset")));

namespace {
	int __optpos = 1;
}

int getopt_long(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
	bool colon = optstring[0] == ':';
	bool stop_at_first_nonarg = (optstring[0] == '+' || getenv("POSIXLY_CORRECT"));

	// glibc extension: Setting optind to zero causes a full reset.
	// TODO: Should we really reset opterr and the other flags?
	if(!optind || optreset) {
		optarg = nullptr;
		optind = 1;
		opterr = 1;
		optopt = 0;
		optreset = 0;
		__optpos = 1;
	}

	auto isOptionArg = [](char *arg){
		// If the first character of arg '-', and the arg is not exactly
		// equal to "-" or "--", then the arg is an option argument.
		return arg[0] == '-' && strcmp(arg, "-") && strcmp(arg, "--");
	};

	while(optind < argc) {
		char *arg = argv[optind];
		if(!isOptionArg(arg)) {
			if(stop_at_first_nonarg) {
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
	return -1;
}

int getopt_long_only(int, char *const[], const char *, const struct option *, int *) {
	mlibc::infoLogger() << "\e[31mmlibc: getopt_long_only() is only a stub\e[39m" << frg::endlog;
	return -1;
}

