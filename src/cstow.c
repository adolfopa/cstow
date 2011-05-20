/*
 * CStow - manage the installation of software packages
 *
 * Copyright (c) 2011, Adolfo Perez Alvarez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following
 *  disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *
 *  3. Neither the name of the author nor the names of its
 *  contributors may be used to endorse or promote products derived
 *  from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE

#include <sys/stat.h>

#include <assert.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum mode { INSTALL, UNINSTALL };

struct options {
     int conflicts;
     int verbose;
     int pretend;
     int operation_mode;
     char *package_name;
     char *source_dir;
     char *target_dir;
     char *package_dir;
};

#define BEING_VERBOSE(o) ((o)->verbose)
#define CHECKING_CONFLICTS(o) ((o)->conflicts)
#define PRETENDING(o) ((o)->pretend)
#define INSTALLING(o) ((o)->operation_mode == INSTALL)
#define UNINSTALLING(o) ((o)->operation_mode == UNINSTALL)

static char *append_path(char *, char *);
static void create_dir(struct options *, char *, int);
static void create_link(struct options *, char *, char *, char *);
static void delete_dir(struct options *, char *);
static void delete_link(struct options *, char *, char *);
static void detect_conflict(struct options *, char *);
int main(int, char **);
static void options_free(struct options *);
static void options_init(struct options *, int, char **);
static void process_directory(struct options *, char *, char *);
static void process_package(struct options *, char *, char *);
static void usage(int);
static void *xmalloc(size_t);
static void *xstrdup(char *);

static void *
xmalloc(size_t n)
{
     void *p;

     assert(n > 0);

     p = malloc(n);

     if (p == NULL)
          err(EXIT_FAILURE, NULL);

     return p;
}

static void *
xstrdup(char *s)
{
     void *p;

     assert(s != NULL);

     p = strdup(s);

     if (p == NULL)
          err(EXIT_FAILURE, NULL);

     return p;
}

static char *
append_path(char *s, char *t)
{
     char * result;
     size_t slen;
     size_t tlen;

     assert(s != NULL);
     assert(t != NULL);

     slen = strlen(s);
     tlen = strlen(t);
     result = xmalloc(sizeof(char) * (slen + tlen + 2));

     (void)memcpy(result, s, slen);

     if (*t != '/' && s[slen - 1] != '/')
          result[slen] = '/';
     
     (void)memcpy(result + slen + 1, t, tlen + 1);

     result[slen + tlen + 1] = '\0';

     return result;
}

static void
process_directory(struct options *options, char *source, char *destination)
{
     int status;
     size_t len;
     struct dirent *entry;
     struct dirent *result;
     DIR *dir;

     assert(options != NULL);
     assert(source != NULL);
     assert(destination != NULL);

     dir = opendir(source);

     if (!dir)
          err(EXIT_FAILURE, "Couldn't read dir '%s'", source);

     /*
      * From the Linux man page:
      * 
      * Since  POSIX.1 does not specify the size of the d_name field, and other
      * non-standard fields may precede that field within the dirent structure,
      * portable  applications  that use readdir_r() should allocate the buffer
      * whose address is passed in entry as follows:
      */
     len = offsetof(struct dirent, d_name) + pathconf(source, _PC_NAME_MAX) + 1;
     entry = xmalloc(len);

     while ((status = readdir_r(dir, entry, &result)) == 0 && result != NULL) {
          if (strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".")) {
               char *child_name = append_path(source, entry->d_name);

               process_package(options, child_name, destination);
               
               free(child_name);
          }
     }

     free(entry);

     if (status)
          err(EXIT_FAILURE, "couldn't read '%s' contents", source);

     status = closedir(dir);

     if (status == -1)
          err(EXIT_FAILURE, "couldn't close dir '%s'", source);
}

static void
detect_conflict(struct options *options, char *destination)
{
     struct stat buf;
     int status;

     assert(options != NULL);
     assert(destination != NULL);

     status = lstat(destination, &buf);

     if (status == -1 && errno != ENOENT) {
          err(EXIT_FAILURE, NULL);
     } else if (status != -1) {
       if (S_ISLNK(buf.st_mode)) {
            char linked_file[_POSIX_PATH_MAX];
            ssize_t len;

            /* FIXME: a link to a link will report the incorrect name. */
            len = readlink(destination, linked_file, _POSIX_PATH_MAX);

            if (len == -1)
                 err(EXIT_FAILURE, NULL);

            linked_file[len == _POSIX_PATH_MAX ? len - 1 : len] = '\0';

            warnx("CONFLICT: %s vs %s\n", destination, linked_file);
       } else {
            warnx("CONFLICT: %s\n", destination);
       }

       if (!CHECKING_CONFLICTS(options))
            exit(EXIT_FAILURE);
     }
}

static void
create_link(struct options *options,
            char *source, char *destination, char *filename)
{
     char *real_src;
     char *real_dest;
     char *full_dest;

     assert(options != NULL);
     assert(source != NULL);
     assert(destination != NULL);
     assert(filename != NULL);

     real_src = realpath(source, NULL);

     if (real_src == NULL && !PRETENDING(options))
          err(EXIT_FAILURE, "couldn't obtain realpath for %s", source);

     real_dest = realpath(destination, NULL);

     if (real_dest == NULL && !PRETENDING(options))
          err(EXIT_FAILURE, "couldn't obtain realpath for %s", destination);

     full_dest = append_path(real_dest, filename);
     
     if (BEING_VERBOSE(options))
          (void)printf("ln -s %s %s\n", real_src, full_dest);

     detect_conflict(options, full_dest);

     if (!PRETENDING(options) && symlink(real_src, full_dest) == -1)
          err(EXIT_FAILURE, "couldn't link %s to %s", real_src, full_dest);

     free(full_dest);
     free(real_dest);
     free(real_src);
}

static void
delete_link(struct options *options, char *destination, char *filename)
{
     char *full_dest;
     struct stat buf;
     int status;

     assert(options != NULL);
     assert(destination != NULL);
     assert(filename != NULL);
     
     full_dest = append_path(destination, filename);

     if (BEING_VERBOSE(options))
          (void)printf("rm %s\n", full_dest);

     status = lstat(full_dest, &buf);

     if (status  == -1 && errno != ENOENT) {
          err(EXIT_FAILURE, "couldn't access link %s", full_dest);
     } else if (status != -1) {
          if (S_ISLNK(buf.st_mode)) {
               /*
                * When unstowing a package, we should fail if a non
                * valid link is found, so as to avoid breaking other
                * stowed packages.  A non valid link is one that
                * doesn't point inside the package we are unstowing.
                */
               
               char link_target[_POSIX_PATH_MAX];
               char *p;
               ssize_t len;

               len = readlink(full_dest, link_target, _POSIX_PATH_MAX);

               if (len == -1)
                    err(EXIT_FAILURE, "couldn't read link %s", full_dest);

               link_target[len == _POSIX_PATH_MAX ? len - 1 : len] = '\0';

               p = strstr(link_target, options->source_dir);

               if (p == NULL || p != link_target)
                    err(EXIT_FAILURE, "%s no a valid symlink (points to %s)",
                        full_dest, link_target);
          } else {
               /*
                * Ignore regular files or directories with the same
                * name as the link we were trying to delete.  This
                * way, the user can remove partially stowed packages
                * without messing things up.
                */

               warnx("%s not a valid symlink", full_dest);
               goto cleanup;
          }
     }

     if (!PRETENDING(options) && unlink(full_dest) == -1 && errno != ENOENT)
          err(EXIT_FAILURE, "couldn't delete link %s", full_dest);
cleanup:
     free(full_dest);
}

static void
create_dir(struct options *options, char *dirname, int mode)
{

     assert(options != NULL);
     assert(dirname != NULL);
     
     if (BEING_VERBOSE(options))
          (void)printf("mkdir %s\n", dirname);

     if (!PRETENDING(options) && mkdir(dirname, mode) == -1 && errno != EEXIST)
          err(EXIT_FAILURE, "couldn't create directory %s", dirname);
}

static void
delete_dir(struct options *options, char *dirname)
{

     assert(options != NULL);
     assert(dirname != NULL);

     if (BEING_VERBOSE(options))
          (void)printf("rmdir %s\n", dirname);

     if (!PRETENDING(options)) {
          int status = rmdir(dirname);

          /*
           * Ignore the case when the directory does not exist (so it
           * is not an error to unstow a directory multiple times) and
           * when the directory is not empty (probably because a non
           * valid link was found inside the directory; see the
           * delete_link() function above).
           */

          if (status == -1 && errno != ENOENT && errno != ENOTEMPTY)
               err(EXIT_FAILURE, "couldn't delete directory %s", dirname);
     }
}

static void
process_package(struct options *options, char *source, char *destination)
{
     struct stat buf;
     char *dest_dir;
     char *dirname;

     assert(options != NULL);
     assert(source != NULL);
     assert(destination != NULL);
     assert(INSTALLING(options) || UNINSTALLING(options));

     if (lstat(source, &buf) == -1)
          err(EXIT_FAILURE, "couldn't access file %s", source);

     dirname = basename(source);

     if (S_ISDIR(buf.st_mode)) {
          dest_dir = append_path(destination, dirname);

          if (INSTALLING(options))
               create_dir(options, dest_dir, buf.st_mode);

          process_directory(options, source, dest_dir);

          if (UNINSTALLING(options))
               delete_dir(options, dest_dir);

          free(dest_dir);
     } else {
          if (INSTALLING(options))
               create_link(options, source, destination, dirname);
          if (UNINSTALLING(options))
               delete_link(options, destination, dirname);
     }
}

static void
usage(int status)
{
     FILE *stream = status ? stderr : stdout;

     (void)fprintf(stream, "Usage: cstow [-cDhnv] <package-name>\n");

     exit(status);
}

static void
options_init(struct options *options, int argc, char **argv)
{
     int ch;
     size_t len;

     assert(options != NULL);
     assert(argc > 0);
     assert(argv != NULL);
     
     options->operation_mode = INSTALL;
     options->verbose = options->pretend = options->conflicts = 0;

     while ((ch = getopt(argc, argv, "cDhnv")) != -1)
          switch (ch) {
          case 'c':
               options->conflicts = options->pretend = 1;
               break;
          case 'D':
               options->operation_mode = UNINSTALL;
               break;
          case 'h':
               usage(EXIT_SUCCESS);
               break;
          case 'n':
               options->pretend = 1;
               break;
          case 'v':
               options->verbose = 1;
               break;
          case '?':
          default:
               usage(EXIT_FAILURE);
               break;
          }

     /* Package name is mandatory. */
     if (optind >= argc)
          usage(EXIT_FAILURE);

     options->source_dir = xmalloc(sizeof(char) * _POSIX_PATH_MAX);

     if (getcwd(options->source_dir, _POSIX_PATH_MAX) == NULL)
          err(EXIT_FAILURE, NULL);

     options->target_dir = xstrdup("..");
     options->package_name = xstrdup(argv[optind]);

     /* Remove trailing '/' from package name. */
     len = strlen(options->package_name);
     while (options->package_name[--len] == '/')
          options->package_name[len] = '\0';

     options->package_dir = append_path(options->source_dir, options->package_name);
}

static void
options_free(struct options *options)
{

     assert(options != NULL);

     free(options->package_dir);
     free(options->package_name);
     free(options->target_dir);
     free(options->source_dir);
}

int
main(int argc, char **argv)
{
     struct options options;

     options_init(&options, argc, argv);

     process_directory(&options, options.package_dir, options.target_dir);

     options_free(&options);

     return EXIT_SUCCESS;
}
