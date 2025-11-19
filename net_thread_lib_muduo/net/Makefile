#中间文件存放目录，如.o 和 .d 文件

COMPILE_DIR = compile
BIN_DIR = bin
LIB_DIR = lib

# 可编译arm版本
CC = $(CROSS)g++ -std=c++11
CFLAGS = -Werror -g
LIB = -L../lib -L../logging/lib -llogging -lmyThread -lpthread
INCLUDE = -I../logging/ -I../include
#SRCS = $(wildcard *.cpp)
SRCS = inetAddress.cpp \
		socketsOps.cpp	\
		tcpServer.cpp	\
		acceptor.cpp	\
		channel.cpp 	\
		eventLoop.cpp 	\
		inetAddress.cpp \
		poller.cpp 		\
		pollPoller.cpp 	\
		socketsOps.cpp 	\
		timer.cpp 		\
		timerQueue.cpp 	\
		socket.cpp		\
		buffer.cpp 		\
		eventLoopThread.cpp \
		eventLoopThreadPool.cpp \
		tcpConnection.cpp \
		epollPoller.cpp \
		defaultPoller.cpp

OBJS = $(patsubst %.cpp, $(COMPILE_DIR)/%.o, $(SRCS))
DEP = $(patsubst %.cpp, $(COMPILE_DIR)/%.cpp.d, $(SRCS))

$(shell if [ ! -d $(COMPILE_DIR) ]; then mkdir $(COMPILE_DIR); fi)
$(shell if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi)
$(shell if [ ! -d $(LIB_DIR) ]; then mkdir $(LIB_DIR); fi)

TARGET=$(LIB_DIR)/libNet.a

all: $(TARGET)

-include $(DEP)

$(TARGET): $(OBJS)
	ar -rcs $@ $^

$(COMPILE_DIR)/%.o: %.cpp $(COMPILE_DIR)/%.cpp.d
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(COMPILE_DIR)/%.cpp.d: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -MM -E -c $< -o $@
	@sed 's/.*\.o/$(subst /,\/,$(dir $@))&/g' $@ > $@.tmp
	@mv $@.tmp $@

.PHONY: clean
clean:
	rm -rf $(COMPILE_DIR) $(BIN_DIR) $(LIB_DIR)
