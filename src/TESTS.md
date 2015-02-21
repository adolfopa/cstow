# CSTOW tests

This file contains all tests for _cstow_. To run it, execute `make
test` in this or the parent directory. If everything goes well, no
output should be produced.

## Executing CSTOW with no arguments

CStow expects at least a non-flag argument, the package to
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

When given the `-h` flag, `cstow` will show a usage message and exit
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

## Stowing packages

The first non flag argument to `cstow` will be interpreted as a
package name. By default, `cstow` will look for the package in the
current directory, and will install it in the parent directory.

```sh
$ mkdir -p tmp/package && cd tmp && cstow package
```

If more flexibility is needed, the `-d` flag allows you to choose
where packages will be looked up:

```sh
$ mkdir -p tmp/package && cstow -d tmp package
```

If no package exists with the given name, cstow will fail.

```sh
$ cstow package
@ cstow: Couldn't read dir '$(pwd)/package': No such file or directory
? 1
```
