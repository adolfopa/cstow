##
## Where to install the binaries
##

INSTALL_DIR = /usr/local/bin

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
	cd src && $(MAKE) INSTALL_DIR=$(INSTALL_DIR) install

test:
	cd src && $(MAKE) CFLAGS="$(CFLAGS)" test

test-coverage:
	cd src && $(MAKE) CFLAGS="$(CFLAGS_CC)" test
	cd src && gcov *.c
