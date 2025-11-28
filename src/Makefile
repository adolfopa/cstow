all: cstow

cstow: cstow.o path.o
	$(CC) $(CFLAGS) -o cstow cstow.o path.o

test: cstow
	@PATH=$$PATH:$$(pwd) ./ts TESTS.md

clean:
	rm -f cstow *.o *~

install: cstow
	@test x$(INSTALL_DIR) != x || ( echo "INSTALL_DIR not defined!. You should be calling this target from the toplevel makefile." && false )
	test ! -d $(INSTALL_DIR) && mkdir -p $(INSTALL_DIR) || true
	install -Sc cstow $(INSTALL_DIR)/cstow
