# CSTOW tests

This file contains all tests for _cstow_. To run it, execute `make
test` in this or the parent directory. If everything goes well, no
output should be produced.

In the tests, I assume a sample `pkg` package exists:

```sh
$ mkdir -p packages/pkg/d0/d1
$ touch packages/pkg/d0/f0
$ touch packages/pkg/d0/d1/f1
```

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
$ cd packages && cstow pkg
$ [ -d d0 ]
$ [ -d d0/d1 ]
$ [ -L d0/f0 ]
$ [ -L d0/d1/f1 ]
$ rm -rf d0
```

If no package exists with the given name, cstow will fail.

```sh
$ cstow xyz
@ cstow: Couldn't read dir '$(pwd)/xyz': No such file or directory
? 1
```

If more flexibility is needed, the `-d` flag allows you to choose
where packages will be looked up:

```sh
$ cstow -d packages pkg
$ [ -d d0 ]
$ rm -rf d0
```

In case more than a `-d` flag is supplied, the last one is used:

```sh
$ cstow -d foo -d packages pkg
$ [ -d d0 ]
$ rm -rf d0
```

You can also change the place where packages will be installed with
the `-t` flag:

```sh
$ mkdir target
$ cstow -d packages -t target pkg
$ [ -d target/d0 ]
$ rm -rf target
```

As with the `-d` flag, if more than one is provided, the last one is used:

```sh
$ mkdir target
$ cstow -d packages -t foo -t target pkg
$ [ -d target/d0 ]
$ rm -rf target
```

## Unstowing packages

To unstow (uninstall) a package, use the `-D` flag:

```sh
$ cstow -d packages pkg
$ [ -d d0 ]
$ cstow -d packages -D pkg
$ [ ! -d d0 ]
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

## Restowing packages

The `-R` flag will reinstall the package. This is equivalent to
executing `cstow -D pkg && cstow pkg`:

```sh
$ cstow -d packages pkg
$ touch packages/pkg/d0/A
$ cstow -d packages -R pkg
$ [ -L d0/A ]
$ rm -rf d0 && rm packages/pkg/d0/A
```

## Simulating actions

The `-n` flag executes CStow without actually installing (or
uninstalling) anything.

```sh
$ cstow -d packages -n pkg
$ [ ! -d d0 ]
```

Note that when using the `-n` flag, CStow *will* detect any
conflicts. Thus, this option is useful to check for conflicts without
modifying the installed package.

```sh
$ cstow -d packages pkg
$ cstow -d packages -n pkg
@ cstow: CONFLICT: $(pwd)/d0/d1/f1 vs ../../packages/pkg/d0/d1/f1
@ 
? 1
$ rm -rf d0
```

While the `-n` flag will detect conflicts, it will stop on the first
failure. If you want to see all conflicts, you need to use the `-c` flag.

```sh
$ cstow -d packages pkg
$ cstow -d packages -c pkg
@ cstow: CONFLICT: $(pwd)/d0/d1/f1 vs ../../packages/pkg/d0/d1/f1
@ 
@ cstow: CONFLICT: $(pwd)/d0/f0 vs ../packages/pkg/d0/f0
@ 
$ rm -rf d0
```

## Verbose mode

By default, `cstow` will show no output on success. By using the `-v`
flag, `cstow` will show a detailed log with all executed actions:

```sh
$ cstow -d packages -v pkg
| mkdir $(pwd)/d0
| mkdir $(pwd)/d0/d1
| ln -s $(pwd)/packages/pkg/d0/d1/f1 $(pwd)/d0/d1/f1
| ln -s $(pwd)/packages/pkg/d0/f0 $(pwd)/d0/f0
```

It works both when stowing and unstowing:

```sh
$ cstow -d packages -v -D pkg
| rm $(pwd)/d0/d1/f1
| rmdir $(pwd)/d0/d1
| rm $(pwd)/d0/f0
| rmdir $(pwd)/d0
```
