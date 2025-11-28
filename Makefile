##
## Where to install the binaries
##

PREFIX ?= /usr/local

##
## Compiler flags
##

# GCC & Clang

CFLAGS = -Wall -Wextra -Werror -O2

CFLAGS_CC = -Wall -Wextra -Werror -O0 -coverage

##
## YOU SHOULD NOT NEED TO MODIFY ANYTHING BELOW HERE
##

compile:
	cd src && $(MAKE) CFLAGS="$(CFLAGS)"

clean:
	cd src && $(MAKE) clean

install: compile
	cd src && $(MAKE) PREFIX=$(PREFIX) install

test:
	cd src && $(MAKE) CFLAGS="$(CFLAGS)" test

test-coverage:
	cd src && $(MAKE) CFLAGS="$(CFLAGS_CC)" test
	cd src && gcov *.c
