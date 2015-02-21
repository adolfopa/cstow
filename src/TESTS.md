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
$ mkdir -p tmp/pkg
$ cd tmp && cstow pkg
```

If more flexibility is needed, the `-d` flag allows you to choose
where packages will be looked up:

```sh
$ cstow -d tmp pkg
```

If no package exists with the given name, cstow will fail.

```sh
$ cstow xyz
@ cstow: Couldn't read dir '$(pwd)/xyz': No such file or directory
? 1
```

## Unstowing packages

To unstow (uninstall) a package, use the `-D` flag:

```sh
$ cd tmp && cstow pkg && cstow -D pkg
```

The `-D` requires an argument, the package name:

```sh
$ cstow -D
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

## Verbose mode

By default, `cstow` will show no output on success. By using the `-v`
flag, `cstow` will show a detailed log with all executed actions:

```sh
$ mkdir -p tmp/pkg/x/y && touch tmp/pkg/x/y/foo
$ cd tmp && cstow -v pkg
| mkdir $(pwd)/x
| mkdir $(pwd)/x/y
| ln -s $(pwd)/tmp/pkg/x/y/foo $(pwd)/x/y/foo
```

It works both when stowing and unstowing:

```sh
$ cd tmp && cstow -v -D pkg
| rm $(pwd)/x/y/foo
| rmdir $(pwd)/x/y
| rmdir $(pwd)/x
```
