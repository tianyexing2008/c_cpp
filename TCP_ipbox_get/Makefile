################################
# brief: server需要在ARM板子上运行
# client_get 在ubuntu 上运行
################################
CC_SVR = arm-linux-gnueabihf-gcc
CC_CLI = gcc


CFLAGS = -Werror -g 
LIB = -lpthread
RM = rm -rf 

TARGET = server client_get
all: $(TARGET)

server: IPBOX_Server.c
	$(CC_SVR) $(CFLAGS) -o $@ $^ $(LIB)

	@# 放到这个目录，方便在板子上wget获取server
	mv $@ /home/daikin/workspace/INLM_IPBOX/target/
client_get: IPBOX_get.c
	$(CC_CLI) $(CFLAGS) -o $@ $< $(LIB)

clean:
	$(RM) $(TARGET)
