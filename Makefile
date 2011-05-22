INSTALL_DIR = /usr/local/bin

compile:
	cd src && $(MAKE)

clean:
	cd src && $(MAKE) clean

install: compile
	test ! -d $(INSTALL_DIR) && mkdir $(INSTALL_DIR) || true
	install -Sc src/cstow $(INSTALL_DIR)/cstow

test:
	cd src && $(MAKE) test
