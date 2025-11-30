PREFIX ?= /usr/local
CFLAGS = -Wall -Wextra -O3

.PHONY: clean install test test-coverage
cstow: cstow.o
test: cstow
	@PATH=$$PATH:$$(pwd) ./ts TESTS.md
clean:
	rm -f cstow *.o *~
	rm -rf packages/
install: cstow
	install -d $(PREFIX)/bin
	install cstow $(PREFIX)/bin
