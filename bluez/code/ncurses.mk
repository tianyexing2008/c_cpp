LIB_TAR = ncurses-5.9
LIB_PATH = $(PWD)/../libtar/ncurses-5.9.tar.gz

all:
	cd $(LIB_TAR) && ./configure --prefix=$(OUT_DIR) --host=arm-linux CC=$(LIB_CC) --with-shared
	cd $(LIB_TAR) && make && make install

prapre:
	rm -rf $(LIB_TAR)
	tar -xvzf $(LIB_PATH)  -C ./
	cd .. && git apply $(PWD)/patch/ncurses/001-ncurses-curses-note-error.patch

clean:
	cd $(LIB_TAR) && make clean

distclean:
	rm -rf $(LIB_TAR)