#中间文件存放目录，如.o 和 .d 文件

COMPILE_DIR = compile
BIN_DIR = bin
LIB_DIR = lib

# 可编译arm版本
CC = $(CROSS)g++ -std=c++11
CFLAGS = -Werror -g
LIB = -L../lib -lmyThread -lpthread
INCLUDE = -I/home/tianyexing/practice/muduo/include
#SRCS = $(wildcard *.cpp)
SRCS = timestamp.cpp \
		logStream.cpp \
		logging.cpp	\
		asyncLogging.cpp \
		fileUtil.cpp	\
		logFile.cpp		\
		processInfo.cpp
OBJS = $(patsubst %.cpp, $(COMPILE_DIR)/%.o, $(SRCS))
DEP = $(patsubst %.cpp, $(COMPILE_DIR)/%.cpp.d, $(SRCS))

$(shell if [ ! -d $(COMPILE_DIR) ]; then mkdir $(COMPILE_DIR); fi)
$(shell if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi)
$(shell if [ ! -d $(LIB_DIR) ]; then mkdir $(LIB_DIR); fi)

TARGET=$(LIB_DIR)/libLogging.a
TEST=$(BIN_DIR)/a.out

all: $(TARGET)
test: $(TEST)

-include $(DEP)

$(TARGET): $(OBJS)
	ar -rcs $@ $^

$(TEST): asyncLoggingTest.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@ ./lib/libLogging.a ../lib/libmyThread.a -lpthread

$(COMPILE_DIR)/%.o: %.cpp $(COMPILE_DIR)/%.cpp.d
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(COMPILE_DIR)/%.cpp.d: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -MM -E -c $< -o $@
	@sed 's/.*\.o/$(subst /,\/,$(dir $@))&/g' $@ > $@.tmp
	@mv $@.tmp $@

.PHONY: clean
clean:
	rm -rf $(COMPILE_DIR) $(BIN_DIR) $(LIB_DIR)
