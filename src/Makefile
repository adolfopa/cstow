all: cstow

cstow: cstow.o path.o
	$(CC) $(CFLAGS) -o cstow cstow.o path.o

test: cstow
	@PATH=$$PATH:$$(pwd) ./ts TESTS.md

clean:
	rm -f cstow *.o *~

install: cstow
	@test x$(PREFIX) != x || ( echo "PREFIX not defined!. You should be calling this target from the toplevel makefile." && false )
	install -d $(PREFIX)/bin
	install cstow $(PREFIX)/bin
