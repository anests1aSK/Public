#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

  char *ghh = malloc(30);
  strncpy(ghh, "Hey i a memory space", 20);
  printf("%s - ", ghh);
  free(ghh);
  puts(" Love hacking ");
  return 0;

}
