#ifndef CSTOW_PATH_H
#define CSTOW_PATH_H

extern char *absolute_path(char *);
extern char *append_path(char *, char *);
extern int make_relative_path(char *, char *, char *, size_t);
extern char *directory_name(char *);
extern void make_absolute_path(char **);
extern char *relative_path(char *, char *);

#endif
