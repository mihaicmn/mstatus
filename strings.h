#include <string.h>

#define STARTS_WITH(prefix, str) (strncmp(prefix, str, strlen(prefix)) == 0)
#define EQUALS(str1, str2) (strcmp(str1, str2) == 0)
