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

#include "path.h"
#include "util.h"

enum mode { INSTALL, UNINSTALL, REINSTALL };

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
#define REINSTALLING(o) ((o)->operation_mode == REINSTALL)

static void create_dir(struct options *, char *, mode_t);
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


static void
process_directory(struct options *options, char *source, char *destination)
{
     struct dirent *entry;
     DIR *dir;

     assert(options != NULL);
     assert(source != NULL);
     assert(destination != NULL);

     dir = opendir(source);

     if (!dir)
          err(EXIT_FAILURE, "Couldn't read dir '%s'", source);

     errno = 0;

     while ((entry = readdir(dir))) {
          if (strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".")) {
               char *child_name = append_path(source, entry->d_name);

               process_package(options, child_name, destination);

               free(child_name);

	       errno = 0;
          }
     }

     if (errno)
          err(EXIT_FAILURE, "couldn't read '%s' contents", source);

     if (closedir(dir) == -1)
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
          err(EXIT_FAILURE, "couldn't lstat file %s", destination);
     } else if (status != -1) {
          if (S_ISLNK(buf.st_mode)) {
               char linked_file[_POSIX_PATH_MAX];
               ssize_t len;

               /* FIXME: a link to a link will report the incorrect name. */
               len = readlink(destination, linked_file, _POSIX_PATH_MAX);

               if (len == -1)
                    err(EXIT_FAILURE, "couldn't read link %s", destination);

               linked_file[len == _POSIX_PATH_MAX ? len - 1 : len] = '\0';

               warnx("CONFLICT: link %s points to %s",
                     destination, linked_file);
          } else {
               warnx("CONFLICT: regular file %s already exists", destination);
          }

          if (!CHECKING_CONFLICTS(options))
               exit(EXIT_FAILURE);
     }
}

static void
create_link(struct options *options,
            char *source, char *destination, char *filename)
{
     char *link_target;
     char *rpath;

     assert(options != NULL);
     assert(source != NULL);
     assert(destination != NULL);
     assert(filename != NULL);

     link_target = append_path(destination, filename);

     rpath = relative_path(link_target, source);

     if (!PRETENDING(options) && chdir(destination) == -1)
          err(EXIT_FAILURE, "%s", destination);

     if (BEING_VERBOSE(options))
          (void)printf("ln -s %s %s\n", source, link_target);

     detect_conflict(options, link_target);

     if (!PRETENDING(options) && symlink(rpath, filename) == -1)
          err(EXIT_FAILURE, "couldn't link %s to %s", rpath, filename);

     free(rpath);
     free(link_target);
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

     if (status == -1 && errno != ENOENT) {
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
               char *abs;
               char *p;
               ssize_t len;

               len = readlink(full_dest, link_target, _POSIX_PATH_MAX);

               if (len == -1)
                    err(EXIT_FAILURE, "couldn't read link %s", full_dest);

               if (chdir(destination) == -1)
                    err(EXIT_FAILURE, NULL);

               link_target[len == _POSIX_PATH_MAX ? len - 1 : len] = '\0';
               abs = absolute_path(link_target);

               p = strstr(abs, options->source_dir);

               if (p == NULL || p != abs) {
                    errx(EXIT_FAILURE, "%s not a valid symlink (points to %s)",
                         full_dest, link_target);
               }

               free(abs);
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
create_dir(struct options *options, char *dirname, mode_t mode)
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
          else if (UNINSTALLING(options))
               delete_link(options, destination, dirname);
          else
               err(EXIT_FAILURE,
                   "Assertion failed: Neither installing or uninstalling.");
     }
}

static void
usage(int status)
{
     FILE *stream = status ? stderr : stdout;

     (void)fprintf(stream,
                   "Usage: cstow [-cdDhnRtv] <package-name>\n"
                   "  -c,     Do not exit when a conflict is found, continue as if\n"
                   "          nothing happened.  This options implies -n.\n"
                   "  -d DIR, Set the package directory to DIR.  If not\n"
                   "          specified the current directory will be used.\n"
                   "  -D,     Delete the package instead of installing it.\n"
                   "  -h,     Show this help message.\n"
                   "  -n,     Do not perform any of the operations, only pretend.\n"
                   "          Useful for detecting errors without damaging anything.\n"
                   "  -R,     Reinstall a package.  Equivalent to invoking cstow\n"
                   "          to install and deinstall in sequence.\n"
                   "  -t DIR, Set the target directory to DIR.  If not\n"
                   "          specified the parent directory will be used.\n"
                   "  -v,     Be verbose, showing each operation performed.\n");

     exit(status);
}

static void
options_init(struct options *options, int argc, char **argv)
{
     int ch, t_flag;
     size_t len;

     assert(options != NULL);
     assert(argc > 0);
     assert(argv != NULL);

     options->operation_mode = INSTALL;
     options->verbose = options->pretend = options->conflicts = 0;

     options->source_dir = NULL;
     options->target_dir = NULL;

     t_flag = 0;                /* Whether the -t flag was found or not. */

     while ((ch = getopt(argc, argv, "cd:DhnRt:v")) != -1)
          switch (ch) {
          case 'c':
               options->conflicts = options->pretend = 1;
               break;
          case 'd':
               if (options->source_dir != NULL)
                    free(options->source_dir);
               options->source_dir = xstrdup(optarg);

               /*
                * If the target dir was set by a previous -d flag,
                * reset it.
                */
               if (options->target_dir != NULL && !t_flag) {
                    free(options->target_dir);
                    options->target_dir = NULL;
               }

               if (options->target_dir == NULL)
                    options->target_dir = directory_name(options->source_dir);
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
          case 'R':
               options->operation_mode = REINSTALL;
               break;
          case 't':
               if (options->target_dir != NULL)
                    free(options->target_dir);

               options->target_dir = xstrdup(optarg);

               t_flag = 1;
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

     /*
      * If no stow directory was given in the command line, use the
      * current directory.
      */
     if (options->source_dir == NULL) {
          options->source_dir = xmalloc(sizeof(char) * _POSIX_PATH_MAX);

          if (getcwd(options->source_dir, _POSIX_PATH_MAX) == NULL)
               err(EXIT_FAILURE, NULL);
     }

     make_absolute_path(&options->source_dir);

     /*
      * If no target directory was given in the command line, use
      * source directory parent.
      */
     if (options->target_dir == NULL)
          options->target_dir = directory_name(options->source_dir);

     make_absolute_path(&options->target_dir);

     options->package_name = xstrdup(argv[optind]);

     /* Remove trailing '/' from package name. */
     len = strlen(options->package_name);
     while (options->package_name[--len] == '/')
          options->package_name[len] = '\0';

     options->package_dir =
         append_path(options->source_dir, options->package_name);
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

     if (REINSTALLING(&options)) {
          options.operation_mode = UNINSTALL;
          process_directory(&options, options.package_dir, options.target_dir);
          options.operation_mode = INSTALL;
          process_directory(&options, options.package_dir, options.target_dir);
     } else {
          process_directory(&options, options.package_dir, options.target_dir);
     }

     options_free(&options);

     return EXIT_SUCCESS;
}
