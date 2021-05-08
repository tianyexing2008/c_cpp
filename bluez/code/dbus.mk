LIB_TAR = dbus-1.9.4
LIB_PATH = $(PWD)/../libtar/dbus-1.9.4.tar.gz

all:
	cd $(LIB_TAR) && ./configure --prefix=$(OUT_DIR) --host=arm-linux --with-x=no --disable-tests --enable-abstract-sockets CC=$(LIB_CC) CPPFLAGS="-I$(OUT_DIR)/include/" LDFLAGS="-L$(OUT_DIR)/lib/"
	cd $(LIB_TAR) && make && make install

prapre:
	rm -rf $(LIB_TAR)
	tar -xvzf $(LIB_PATH)  -C ./

clean:
	cd $(LIB_TAR) && make clean

distclean:
	rm -rf $(LIB_TAR)