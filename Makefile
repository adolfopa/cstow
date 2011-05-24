INSTALL_DIR = /usr/local/bin

compile:
	cd src && $(MAKE)

clean:
	cd src && $(MAKE) clean

install: compile
	cd src && $(MAKE) INSTALL_DIR=$(INSTALL_DIR) install

test:
	cd src && $(MAKE) test
