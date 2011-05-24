CFLAGS = -ansi -pedantic -Wall -Werror -Wextra -O2

cstow: cstow.o
	cc $(CFLAGS) -o cstow cstow.o
cstow.o:

.PHONY: clean test

test:
	atf-run | atf-report

clean:
	rm -f cstow cstow.o *~
