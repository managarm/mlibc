

#include <assert.h>
#include <bits/ensure.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include <mlibc/debug.hpp>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt;

int getopt_long(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
	// glibc extension: Setting optind to zero causes a full reset.
	// TODO: Should we really reset opterr and the other flags?
	if(!optind) {
		optarg = nullptr;
		optind = 1;
		opterr = 1;
		optopt = 0;
	}

	while(optind < argc) {
		char *arg = argv[optind];
		if(arg[0] != '-')
			return -1;

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
						fprintf(stderr, "Multiple option declaration detected.\n", arg);
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

			// We do not support arguments yet
			if(longopts[k].has_arg == required_argument) {
				if(!s) {
					if(opterr)
						fprintf(stderr, "--%s is not a valid option.\n", arg);
					return '?';
				}
				optarg = s + 1;
			}else{
				__ensure(longopts[k].has_arg == no_argument);
			}
			if(!longopts[k].flag) {
				optind++;
				return longopts[k].val;
			}
					
			*longopts[k].flag = longopts[k].val;
			optind++;
			return 0;
		}else{
			__ensure((strlen(argv[optind]) == 2) && "We do not support concatenated short options yet.");
			unsigned int i = 1;
			while(true) {
				auto opt = strchr(optstring, arg[i]);
				if(opt) {
					__ensure((opt[1] != ':') && "We do not support option arguments.");
					optind++;
					return arg[i];
				}else {
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

