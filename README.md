[![Build Status](https://travis-ci.org/adolfopa/cstow.svg?branch=master)](https://travis-ci.org/adolfopa/cstow)
[![Codecov](https://img.shields.io/codecov/c/github/adolfopa/cstow.svg)](https://codecov.io/gh/adolfopa/cstow/)
[![License](https://img.shields.io/badge/license-BSD-blue.svg)](https://raw.githubusercontent.com/adolfopa/cstow/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/adolfopa/cstow.svg)](https://github.com/adolfopa/cstow/releases/latest)

CStow is a software package management tool.  Essentially, it is a
simplified clone of GNU Stow.  The main differences between GNU Stow
and CStow are:

  1. CStow is implemented in C with almost no external dependencies.
  It should compile in any POSIX system out of the box.

  2. CStow will never create symbolic links for directories when
  installing packages; only files will be symlinked.  This behaviour
  is intentional.

  3. CStow is *way* simpler than GNU Stow. It cannot deferr, override,
  adopt or ignore files, and it doesn't support ignore files.

The lack of dependencies makes CStow useful when you want to use GNU
Stow but aren't allowed to (or don't want to) install Perl and all its
transitive dependencies on the system.  Also, as CStow doesn't symlink
directories, it works well in conjunction with other tools like
FreeBSD ports o NetBSD pkgsrc; this way, you can have all your local
software in `/usr/local` instead of having multiple local trees.

If you don't mind having Perl around, or don't use other package
management tools that might have problems with symlinked directories,
you shouldn't use CStow; GNU Stow is more mature and featureful.

Installation
------------

To compile and install cstow in your system:

  1. Edit the `Makefile` file and change the `INSTALL_DIR` variable to
  suit your system.

  2. Optionally, you can tune the `CFLAGS` variable.

  3. Execute the `make install` command (you may need root privileges
  depending on the value of `INSTALL_DIR`).  This will compile the
  sources and copy the executable to the place you defined in the
  previous step.

If you want to run the tests:

```sh
$ git submodule init
$ git submodule update
$ make test
```

This should work in any modern POSIX-like system (tested with FreeBSD
11-STABLE, Mac OS X 10.11.6, Debian 8 and Ubuntu 12.04 LTS).  If
that's not the case, please open an issue with the details.

Usage
-----

For the moment, the only documentation available is the output of the
`-h` flag and the `src/TESTS.md` file.

As CStow supports a subset of GNU Stow functionality, you can read the
documentation for the GNU Stow package available at
http://www.gnu.org/software/stow/ to get more details.

Known bugs
----------

CStow will fail to unstow a package that contains a symbolic link
pointing outside the package directory.  This happens because cstow
uses the `realpath()` function to resolve filenames; as this function
returns the target filename for symbolic links, cstow thinks that it
is trying to delete a file that is not owned by the package and
refuses to remove it.

As this is not a common scenario, I won't fix it until someone opens
an issue about it.
