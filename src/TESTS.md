# CSTOW tests

This file contains all tests for _cstow_. To run it, execute `make
test` in this or the parent directory. If everything goes well, no
output should be produced.

## Executing CSTOW with no arguments

CStow expects at least a non-flat argument, the package to
install. Executing `cstow` with no arguments will fail with a non-zero
exit status and will show a brief usage message.

```sh
$ cstow
@ Usage: cstow [-cdDhnRtv] <package-name>
@   -c,     Do not exit when a conflict is found, continue as if
@           nothing happened.  This options implies -n.
@   -d DIR, Set the package directory to DIR.  If not
@           specified the current directory will be used.
@   -D,     Delete the package instead of installing it.
@   -h,     Show this help message.
@   -n,     Do not perform any of the operations, only pretend.
@           Useful for detecting errors without damaging anything.
@   -R,     Reinstall a package.  Equivalent to invoking cstow
@           to install and deinstall in sequence.
@   -t DIR, Set the target directory to DIR.  If not
@           specified the parent directory will be used.
@   -v,     Be verbose, showing each operation performed.
? 1
```

## Getting help

When passsed the `-h` flat, `cstow` will show a usage message and exit
with a success status (0).

```sh
$ cstow -h
| Usage: cstow [-cdDhnRtv] <package-name>
|   -c,     Do not exit when a conflict is found, continue as if
|           nothing happened.  This options implies -n.
|   -d DIR, Set the package directory to DIR.  If not
|           specified the current directory will be used.
|   -D,     Delete the package instead of installing it.
|   -h,     Show this help message.
|   -n,     Do not perform any of the operations, only pretend.
|           Useful for detecting errors without damaging anything.
|   -R,     Reinstall a package.  Equivalent to invoking cstow
|           to install and deinstall in sequence.
|   -t DIR, Set the target directory to DIR.  If not
|           specified the parent directory will be used.
|   -v,     Be verbose, showing each operation performed.
```
