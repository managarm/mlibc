#include <stdlib.h>

int rpmatch(const char *resp) {
	if(!resp || resp[0] == '\0')
		return -1;
	if(resp[0] == 'y' || resp[0] == 'Y')
		return 1;
	if(resp[0] == 'n' || resp[0] == 'N')
		return 0;
	return -1;
}
