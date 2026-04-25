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
@ Usage: cstow [-cdDhnoRStv] <package-name>
@   -c,     Do not exit when a conflict is found, continue as if
@           nothing happened.  This options implies -n.
@   -d DIR, Set the package directory to DIR.  If not
@           specified the current directory will be used.
@   -D,     Delete the package instead of installing it.
@   -h,     Show this help message.
@   -n,     Do not perform any of the operations, only pretend.
@   -o,     Enable dotfile translation.  Replaces the \'dot.\' prefix
@           with a literal \'.\' in symbolic link names.
@   -R,     Reinstall a package.  Equivalent to invoking cstow
@           to install and deinstall in sequence.
@   -S,     Install the package.
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
| Usage: cstow [-cdDhnoRStv] <package-name>
|   -c,     Do not exit when a conflict is found, continue as if
|           nothing happened.  This options implies -n.
|   -d DIR, Set the package directory to DIR.  If not
|           specified the current directory will be used.
|   -D,     Delete the package instead of installing it.
|   -h,     Show this help message.
|   -n,     Do not perform any of the operations, only pretend.
|   -o,     Enable dotfile translation.  Replaces the \'dot.\' prefix
|           with a literal \'.\' in symbolic link names.
|   -R,     Reinstall a package.  Equivalent to invoking cstow
|           to install and deinstall in sequence.
|   -S,     Install the package.
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

It is not really necessary, as installing is the default action, but you can
explicitly tell `cstow` to install a package with the `-S` flag.

```sh
$ cd packages && cstow -S pkg
$ [ -d d0 ]
$ [ -d d0/d1 ]
$ [ -L d0/f0 ]
$ [ -L d0/d1/f1 ]
$ rm -rf d0
```

`cstow` will ignore any trailing slashes in the package name:

```sh
$ cd packages && cstow pkg///
$ [ -d d0 ]
$ [ -d d0/d1 ]
$ [ -L d0/f0 ]
$ [ -L d0/d1/f1 ]
$ rm -rf d0
```

If no package exists with the given name, cstow will fail.

```sh
$ cstow xyz
@ cstow: couldn't read dir $(pwd)/xyz: No such file or directory
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

Any directory works, the package and target directory don't have to share a common parent:

```sh
$ mkdir /tmp/target
$ cstow -d packages -t /tmp/target pkg
$ [ -d /tmp/target/d0 ]
$ rm -rf /tmp/target
```

As with the `-d` flag, if more than one is provided, the last one is used:

```sh
$ mkdir target
$ cstow -d packages -t foo -t target pkg
$ [ -d target/d0 ]
$ rm -rf target
```

The `-o` flags will replace the `dot.` prefix with a `.` in the target link
file name.  This is useful when working with packages that contain config
files that would be hidden if beginning with a dot.

```sh
$ touch packages/pkg/d0/dot.f1
$ cd packages && cstow pkg
$ [ -f d0/dot.f1 ]
$ rm -rf d0
$ cd packages && cstow -o pkg
$ [ -f d0/.f1 ]
$ rm -rf d0
$ rm packages/pkg/d0/dot.f1
```

The `-o` flag will also replace the `dot.` prefix in directory names.

```sh
$ mkdir packages/pkg/dot.dh
$ touch packages/pkg/dot.dh/dot.fh
$ cd packages && cstow pkg
$ [ -f dot.dh/dot.fh ]
$ rm -rf d0 dot.dh
$ cd packages && cstow -o pkg
$ [ -f .dh/.fh ]
$ rm -rf d0 .dh
$ rm -rf packages/pkg/dot.dh

A file named exactly `dot.` with the `-o` flag should not produce an
empty filename (which would cause a spurious conflict on the parent
directory).

```sh
$ rm -rf d0
$ touch packages/pkg/d0/dot.
$ cd packages && cstow -o pkg
$ [ -L d0/dot. ]
$ rm -rf d0
$ rm packages/pkg/d0/dot.
```

## Unstowing packages

To unstow (uninstall) a package, use the `-D` flag:

```sh
$ cstow -d packages pkg
$ [ -d d0 ]
$ cstow -d packages -D pkg
$ [ ! -d d0 ]
```

If a regular file was found where a link into the package was
expected, `cstow` will show a warning and ignore the file (you'll have
to clean it up manually).

```sh
$ cstow -d packages pkg
$ rm d0/f0 && touch d0/f0
$ cstow -d packages -D pkg
@ cstow: $(pwd)/d0/f0 not a valid symlink
$ rm -rf d0
```

Also, if a link is found, but it doesn't point into the package,
`cstow` will also ignore it:

```sh
$ cstow -d packages pkg
$ touch dummy && rm d0/f0 && ln -s $(pwd)/dummy d0/f0
$ cstow -d packages -D pkg
@ cstow: $(pwd)/d0/f0 not a valid symlink (points to $(pwd)/dummy)
? 1
$ rm -rf d0 dummy
```

The `-D` requires an argument, the package name:

```sh
$ cstow -D
@ Usage: cstow [-cdDhnoRStv] <package-name>
@   -c,     Do not exit when a conflict is found, continue as if
@           nothing happened.  This options implies -n.
@   -d DIR, Set the package directory to DIR.  If not
@           specified the current directory will be used.
@   -D,     Delete the package instead of installing it.
@   -h,     Show this help message.
@   -n,     Do not perform any of the operations, only pretend.
@   -o,     Enable dotfile translation.  Replaces the \'dot.\' prefix
@           with a literal \'.\' in symbolic link names.
@   -R,     Reinstall a package.  Equivalent to invoking cstow
@           to install and deinstall in sequence.
@   -S,     Install the package.
@   -t DIR, Set the target directory to DIR.  If not
@           specified the parent directory will be used.
@   -v,     Be verbose, showing each operation performed.
? 1
```

If the package was installed with the `-o` flag, you must supply the `-o` flag
again when uninstalling.

```sh
$ touch packages/pkg/d0/dot.f1
$ cd packages && cstow -o pkg
$ [ -f d0/.f1 ]
$ cd packages && cstow -Do pkg
$ [ ! -f d0/.f1 ]
$ rm packages/pkg/d0/dot.f1

The `-D` action validates that symlinks point into the package depot.
A prefix collision (e.g. `/a/b/stow` matching `/a/b/stow-extra`) is
correctly rejected.

```sh
$ mkdir -p packages-EVIL
$ touch packages-EVIL/evil
$ cstow -d packages pkg
$ rm d0/f0
$ ln -s $(pwd)/packages-EVIL/evil d0/f0
$ cstow -d packages -D pkg
@ cstow: $(pwd)/d0/f0 not a valid symlink (points to $(pwd)/packages-EVIL/evil)
? 1
$ rm -rf d0 packages-EVIL
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
@ cstow: CONFLICT: link $(pwd)/d0/d1/f1 points to ../../packages/pkg/d0/d1/f1
? 1
$ rm -rf d0
```

While the `-n` flag will detect conflicts, it will stop on the first
failure. If you want to see all conflicts, you need to use the `-c`
flag. In this example, `cstow` detects that there are already two links
`f0` and `f1`, and will tell you where do they point:


```sh
$ cstow -d packages pkg
$ cstow -d packages -c pkg
@ cstow: CONFLICT: link $(pwd)/d0/d1/f1 points to ../../packages/pkg/d0/d1/f1
@ cstow: CONFLICT: link $(pwd)/d0/f0 points to ../packages/pkg/d0/f0
$ rm -rf d0
```

Here another example, where a regular file already exists in the
target directory:

```sh
$ mkdir d0 && touch d0/f0
$ cstow -d packages pkg
@ cstow: CONFLICT: regular file $(pwd)/d0/f0 already exists
? 1
$ rm -rf d0

When a directory is found where a symlink is expected, the warning
message says "directory" instead of "regular file":

```sh
$ mkdir -p d0/f0
$ cstow -d packages pkg
@ cstow: CONFLICT: directory $(pwd)/d0/f0 already exists
? 1
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

## Clean up

```
$ [ -d packages ] && rm -rf packages
```
