[![Coverage Status](https://coveralls.io/repos/github/adolfopa/cstow/badge.svg)](https://coveralls.io/github/adolfopa/cstow)
[![License](https://img.shields.io/badge/license-BSD-blue.svg)](https://raw.githubusercontent.com/adolfopa/cstow/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/adolfopa/cstow.svg)](https://github.com/adolfopa/cstow/releases/latest)

Cstow is a software package management tool.  It is a simplified version of GNU
Stow with minimal build and runtime dependencies.

Installation
------------

Cstow consists of a single self-contained binary.  The easiest way to build and
install cstow is:

	$ make prefix=/usr/local install

If you want to use cstow to manage cstow itself:

	$ make prefix=/usr/local/cstow/cstow install
	$ cd /usr/local/cstow
	$ ./cstow/bin/cstow cstow

Cstow comes with tests.  To run them:

	$ make test

Cstow should work in any POSIX-ish system (tested with FreeBSD 14.3, Fedora 42
and Morphos 3.19).
