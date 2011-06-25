#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void *
xmalloc(size_t n)
{
     void *p;

     assert(n > 0);

     p = malloc(n);

     if (p == NULL)
          err(EXIT_FAILURE, NULL);

     return p;
}

char *
xstrdup(char *s)
{
     void *p;

     assert(s != NULL);

     p = strdup(s);

     if (p == NULL)
          err(EXIT_FAILURE, NULL);

     return p;
}
