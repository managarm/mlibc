
#ifndef  _GETOPT_H
#define  _GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

struct option {
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

extern char **environ;
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

int getopt(int, char *const [], const char *);
int getopt_long(int, char *const[], const char *, const struct option *, int *);
int getopt_long_only(int, char *const[], const char *, const struct option *, int *);

#define no_argument 0
#define required_argument 1
#define optional_argument 2

#ifdef __cplusplus
}
#endif

#endif // _GETOPT_H

