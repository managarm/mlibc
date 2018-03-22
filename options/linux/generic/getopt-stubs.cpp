
#include <getopt.h>

#include <frigg/debug.hpp>
#include <bits/ensure.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

char *optarg;
int optind = 1;
int opterr;
int optopt;

// TODO: Support all global variables
// TODO: Report ambigious options
// TODO: Support concatenated short options
// TODO: Support arguments

int getopt_long(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
	while(optind < argc) {
		char *arg = argv[optind];
		if(arg[0] != '-')
			return -1;
		if(arg[1] == '-') {
			for(int i = 0; longopts[i].name; i++) {	
				if(!strcmp(argv[optind] + 2, longopts[i].name)) {
					// We do not support arguments yet
					assert(longopts[i].has_arg == no_argument);
					if(!longopts[i].flag) {
						optind++;
						return longopts[i].val;
					}
							
					*longopts[i].flag = longopts[i].val;
					optind++;
					return 0;
				}
			}
			fprintf(stderr, "%s is not a valid option.\n", arg);
			return '?';
		}else {
			// We do not support multiple shortoptions yet. Ex: -dhV.
			assert(strlen(argv[optind]) == 2);
			unsigned int i = 1;
			while(true) {
				if(strchr(optstring, arg[i])) {
					optind++;
					return arg[i];
				}else {
					fprintf(stderr, "%s is not a valid option.\n", arg);
					return '?';
				}
			}
		}
	}
	return -1;
}

int getopt_long_only(int, char *const[], const char *, const struct option *, int *) {
	frigg::infoLogger() << "\e[31mmlibc: getopt_long_only() is only a stub\e[39m" << frigg::endLog;
	return -1;
}

