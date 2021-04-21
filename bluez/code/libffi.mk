LIB_TAR = libffi-3.0.13
LIB_PATH = $(PWD)/../libtar/libffi-3.0.13.tar.gz

all:
	cd $(LIB_TAR) && ./configure --prefix=$(OUT_DIR) --host=arm-linux CC=$(LIB_CC)
	cd $(LIB_TAR) && make && make install

prapre:
	rm -rf $(LIB_TAR)
	tar -xvzf $(LIB_PATH)  -C ./

clean:
	cd $(LIB_TAR) && make clean

distclean:
	rm -rf $(LIB_TAR)