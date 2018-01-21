
#ifndef _CTYPE_H
#define _CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

// Character classification function [7.4.1]
int isalnum(int c);
int isalpha(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

// Character case mapping functions [7.4.2]
int tolower(int c);
int toupper(int c);

#ifdef __cplusplus
}
#endif

#endif // _CTYPE_H

