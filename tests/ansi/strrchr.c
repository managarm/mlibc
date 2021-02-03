#include <string.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  char str[] = "This is a sample string";
  char *pch;
  pch = strrchr(str, 's');
  // The last occurence of 's' is at position 18
  assert(pch - str + 1 == 18);
  return 0;
}
