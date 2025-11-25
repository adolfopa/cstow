#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>

#include "path.h"

#define PARENT_PATH "../"
#define ROOT_PATH "/"
#define FILE_SEPARATOR '/'

#define min(x,y) ((x) < (y) ? (x) : (y))

char *
relative_path(char *from, char *to)
{
	char *from_copy;
	char *to_copy;
	char buffer[PATH_MAX];
	int retval;

	assert(from != NULL);
	assert(to != NULL);

	if ((from_copy = strdup(from)) == NULL)
		err(EXIT_FAILURE, NULL);
	if ((to_copy = strdup(to)) == NULL)
		err(EXIT_FAILURE, NULL);

	retval = make_relative_path(from_copy, to_copy, buffer, PATH_MAX);

	free(to_copy);
	free(from_copy);

	return retval == 0 ? strdup(buffer) : NULL;
}

int
make_relative_path(char *from, char *to, char *buffer, size_t n)
{
	char *from_ctx = NULL;
	char *to_ctx = NULL;
	char *p;
	char *q;
	int depth;
	size_t remaining;

	assert(from != NULL);
	assert(to != NULL);
	assert(buffer != NULL);

	while (*from == FILE_SEPARATOR)
		from++;
	while (*to == FILE_SEPARATOR)
		to++;

	p = strtok_r(from, ROOT_PATH, &from_ctx);
	q = strtok_r(to, ROOT_PATH, &to_ctx);

	/* Skip the longest common prefix of `from' and `to' */
	while (p != NULL && q != NULL && strcmp(p, q) == 0) {
		p = strtok_r(NULL, ROOT_PATH, &from_ctx);
		q = strtok_r(NULL, ROOT_PATH, &to_ctx);
	}

	/*
	 * The relative depth of `from' respective to `to'. The initial
	 * value is always 0, except in the case of paths that do not
	 * share any common parent.
	 */
	depth = strcmp(from, to) == 0 ? 0 : 1;

	while (strtok_r(NULL, ROOT_PATH, &from_ctx) != NULL)
		depth++;

	remaining = n;

	memset(buffer, '\0', n);

	/* Add as many '../' as needed to go from `from' to the root of `to' */
	while (depth > 0 && remaining > 0) {
		memcpy(buffer + (n - remaining), PARENT_PATH, min(remaining, 3));
		remaining = remaining >= 3 ? remaining - 3 : 0;
		depth--;
	}

	/* Copy all remaining `to' elements onto the relative path created */
	while (q != NULL && remaining > 0) {
		size_t qlen = strlen(q);
		memcpy(buffer + (n - remaining), q, min(remaining, qlen));
		remaining -= qlen;

		q = strtok_r(NULL, ROOT_PATH, &to_ctx);

		if (q != NULL)
			buffer[n - remaining--] = FILE_SEPARATOR;
	}

	if (n > 0)
		buffer[n - 1] = '\0';

	return remaining == 0;
}

char *
append_path(char *s, char *t)
{
	char *result;
	size_t slen;
	size_t tlen;

	assert(s != NULL);
	assert(t != NULL);

	slen = strlen(s);
	tlen = strlen(t);
	if ((result = malloc(sizeof(char) * (slen + tlen + 2))) == NULL)
		err(EXIT_FAILURE, NULL);

	(void)memcpy(result, s, slen);

	if (*t != '/' && s[slen - 1] != '/')
		result[slen] = '/';

	(void)memcpy(result + slen + 1, t, tlen + 1);

	result[slen + tlen + 1] = '\0';

	return result;
}

char *
directory_name(char *path)
{
	char *copy;
	char *name;

	assert(path != NULL);

	if ((copy = strdup(path)) == NULL)
		err(EXIT_FAILURE, NULL);
	if ((name = strdup(dirname(copy))) == NULL)
		err(EXIT_FAILURE, NULL);
	free(copy);

	return name;
}
