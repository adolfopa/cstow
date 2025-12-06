prefix?=$$HOME/.local/cstow/cstow
bindir=${prefix}/bin
CFLAGS=-Wall -Wextra -O3

.PHONY: clean install test
cstow: cstow.o
test: cstow
	PATH=$${pwd}:$$PATH ./ts TESTS.md
clean:
	rm -f cstow *.o *~
install: cstow
	install -d ${bindir}
	install cstow ${bindir}
