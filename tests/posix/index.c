#include <assert.h>
#include <strings.h>
#include <stddef.h>

int main() {
  char str[] = "This is a sample string";
  char *pch;
  // The character 's' is at position 4, 7, 11 and 18
  pch = index(str, 's');
  assert(pch - str + 1 == 4);
  pch = index(pch + 1, 's');
  assert(pch - str + 1 == 7);
  pch = index(pch + 1, 's');
  assert(pch - str + 1 == 11);
  pch = index(pch + 1, 's');
  assert(pch - str + 1 == 18);
  pch = index(pch + 1, 's');
  assert(pch == NULL);
  return 0;
}
