#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main() {
   int res;

   res = strverscmp("jan1", "jan10");
   assert(res < 0);

   res = strverscmp("jan11", "jan10");
   assert(res > 0);

   res = strverscmp("jan1", "jan1");
   assert(res == 0);

   return 0;
}
