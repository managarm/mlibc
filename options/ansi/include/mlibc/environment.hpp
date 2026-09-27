#ifndef MLIBC_ENVIRONMENT_HPP
#define MLIBC_ENVIRONMENT_HPP

namespace mlibc {

char *getenv(const char *name);
int putenv(char *string);

} // namespace mlibc

#endif // MLIBC_ENVIRONMENT_HPP
