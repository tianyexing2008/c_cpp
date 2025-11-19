#中间文件存放目录，如.o 和 .d 文件

COMPILE_DIR = compile
BIN_DIR = bin


# 可编译arm版本
#CROSS = arm-himix200-linux-
CC = $(CROSS)g++ -std=c++11
CFLAGS = -Werror -g
INCLUDE = -I../include
LIB = -lpthread
#SRCS = $(wildcard *.cpp)
SRCS = $(shell ls -t | grep "\.cpp$$" | head -1)
#SRCS += main.cpp counter.cpp
OBJS = $(patsubst %.cpp, $(COMPILE_DIR)/%.o, $(SRCS))
DEP = $(patsubst %.cpp, $(COMPILE_DIR)/%.cpp.d, $(SRCS))

$(shell if [ ! -d $(COMPILE_DIR) ]; then mkdir $(COMPILE_DIR); fi)
$(shell if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi)


BIN =
ifeq ($(target), ) #如果是空的
BIN = a.out
else
BIN := $(target)
endif


TARGET=$(BIN_DIR)/$(BIN)
LIB_TARGET=../lib/myThread.a

all: $(TARGET)

-include $(DEP)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIB_TARGET) $(LIB)

$(COMPILE_DIR)/%.o: %.cpp $(COMPILE_DIR)/%.cpp.d
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(COMPILE_DIR)/%.cpp.d: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -MM -E -c $< -o $@
	@sed 's/.*\.o/$(subst /,\/,$(dir $@))&/g' $@ > $@.tmp
	@mv $@.tmp $@

.PHONY: clean
clean:
	rm -rf $(COMPILE_DIR) $(BIN_DIR)
