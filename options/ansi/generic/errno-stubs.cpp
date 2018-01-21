
int __thread __mlibc_errno;

// TODO: Fix this by proving the real program name.
static const char *dummy = "program_invocation_name";

char *program_invocation_name = const_cast<char *>(dummy);
char *program_invocation_short_name = const_cast<char *>(dummy);

