#include <string.h>

#define STARTS_WITH(prefix, str, len) (strncmp(prefix, str, len) == 0)
#define EQUALS(str1, str2) (strcmp(str1, str2) == 0)
