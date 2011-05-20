INSTALL_DIR = /usr/local/bin

compile:
	cd src && $(MAKE)

clean:
	cd src && $(MAKE) clean

install: compile
	install -S src/cstow $(INSTALL_DIR)

test:
	cd src && $(MAKE) test
