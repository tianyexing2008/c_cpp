LIB_TAR = zlib-1.2.11
LIB_PATH = $(PWD)/../libtar/zlib-1.2.11.tar.gz

all:
	cd $(LIB_TAR) &&  CC=$(LIB_CC)  ./configure --prefix=$(OUT_DIR)
	cd $(LIB_TAR) && make && make install

prapre:
	rm -rf $(LIB_TAR)
	tar -xvzf $(LIB_PATH)  -C ./

clean:
	cd $(LIB_TAR) && make clean

distclean:
	rm -rf $(LIB_TAR)