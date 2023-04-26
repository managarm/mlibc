#include <nl_types.h>

int catclose(nl_catd catd) 
{
	return 0;
}

char *catgets (nl_catd catd, int set_id, int msg_id, const char *s)
{
	return (char *)s;
}

nl_catd catopen (const char *name, int oflag)
{
	return (nl_catd)-1;
}
