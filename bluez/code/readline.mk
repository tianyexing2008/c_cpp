LIB_TAR = readline-6.3
LIB_PATH = $(PWD)/../libtar/readline-6.3.tar.gz

all:
	cd $(LIB_TAR) && ./configure --prefix=$(OUT_DIR) --host=arm-linux CC=$(LIB_CC) bash_cv_wcwidth_broken=yes
	cd $(LIB_TAR) && make && make install

prapre:
	rm -rf $(LIB_TAR)
	tar -xvzf $(LIB_PATH)  -C ./

clean:
	cd $(LIB_TAR) && make clean

distclean:
	rm -rf $(LIB_TAR)