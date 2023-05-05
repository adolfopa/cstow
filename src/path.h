#ifndef CSTOW_PATH_H
#define CSTOW_PATH_H

extern char *append_path(char *, char *);
extern int make_relative_path(char *, char *, char *, size_t);
extern char *directory_name(char *);
extern char *relative_path(char *, char *);

#endif
