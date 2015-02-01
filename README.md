[![Build Status](https://travis-ci.org/adolfopa/cstow.svg?branch=master)](https://travis-ci.org/adolfopa/cstow)
[![License](https://img.shields.io/badge/license-BSD-blue.svg)](https://raw.githubusercontent.com/adolfopa/cstow/master/LICENSE)

CStow is a software package management tool.  Essentially, it is a
simplified clone of GNU Stow.  The main differences between GNU Stow
and CStow are:

  1. CStow is implemented in C with almost no external dependencies.
  It should compile in any POSIX system out of the box (if it doesn't,
  it is a bug!).

  2. CStow will never create symbolic links for directories when
  installing packages; only files will be symlinked.  This behaviour
  is intentional.

  3. The -V flag isn't recognized.

The lack of dependencies makes CStow useful when you want to use GNU
Stow but aren't allowed to (or don't want to) install Perl on the
system.  Also, as CStow doesn't symlink directories, it works well in
conjunction with other tools like FreeBSD ports o NetBSD pkgsrc; this
way, you can have all your local software in /usr/local instead of
having multiple local trees.

If you don't mind having Perl around, or don't use other package
management tools that might have problems with symlinked directories,
you shouldn't use CStow; GNU Stow is more mature and featureful.

Installation
------------

To compile and install cstow in your system:

  1. Edit the `Makefile' file and change the INSTALL_DIR variable to
  suit your system.

  2. Execute the `make install' command (you may need root privileges
  depending on the value of INSTALL_DIR).  This will compile the
  sources and copy the executable to the place you defined in the
  previous step.

  3. (OPTIONAL) If you have ATF (http://www.netbsd.org/~jmmv/atf/)
  installed on your system, you can execute the tests with the command
  `make test'.

And that's it! This should work in any reasonably POSIX system (I've
tested it with FreeBSD 8.2 and Ubuntu 11.4).  If that's not the case,
please open an issue with the details at the github project page
(https://github.com/adolfopa/cstow) or send an email to
adolfo.pa(at)gmail(dot)com.  I promise I'll try to fix it!.

Usage
-----

Right now, the only documentation available is this crappy text file
and the output of the `-h' flag :-(.  For more details on the use of
CStow you can use the (excellent) documentation for the GNU Stow
package available at http://www.gnu.org/software/stow/.

Known bugs
----------

CStow will fail to deinstall a package that contains a symbolic link
pointing outside the package directory.  This happens because cstow
uses the `realpath()' function to resolve filenames; as this function
returns the target filename for symbolic links, cstow thinks that it
is trying to delete a file that is not owned by the package and
refuses to remove it.

As this is not a common scenario, I won't fix it until someone opens
an issue about it.
