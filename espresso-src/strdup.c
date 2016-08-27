#include <stdlib.h>
#include <string.h>

char *strdup(const char * src) {
  char * dup;
  unsigned int len;

  if (!src) return NULL;

  len = strlen(src);

  if (len == 0) return NULL;

  dup = malloc(len+1);

  if (dup)
    strcpy(dup, src);

  return dup;
}
