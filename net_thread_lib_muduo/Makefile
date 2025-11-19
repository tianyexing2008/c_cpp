#中间文件存放目录，如.o 和 .d 文件

COMPILE_DIR = compile
BIN_DIR = bin
LIB_SRC = lib/Src

# 可编译arm版本
#CROSS = arm-himix200-linux-
CC = $(CROSS)g++ -std=c++11
CFLAGS = -Werror -g
INCLUDE = -I./include
LIB = -lpthread
#SRCS = $(wildcard *.cpp)
SRCS = $(shell ls -t | grep "\.cpp$$" | head -1)
#SRCS += main.cpp counter.cpp
OBJS = $(patsubst %.cpp, $(COMPILE_DIR)/%.o, $(SRCS))
DEP = $(patsubst %.cpp, $(COMPILE_DIR)/%.cpp.d, $(SRCS))

LIBSRCS = $(wildcard $(LIB_SRC)/*.cpp)
LIBOBJS = $(patsubst $(LIB_SRC)/%.cpp, $(LIB_SRC)/$(COMPILE_DIR)/%.o, $(LIBSRCS))
LIBDEP = $(patsubst $(LIB_SRC)/%.cpp, $(LIB_SRC)/$(COMPILE_DIR)/%.cpp.d, $(LIBSRCS))

#$(shell if [ ! -d $(COMPILE_DIR) ]; then mkdir $(COMPILE_DIR); fi)
#$(shell if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi)

$(shell if [ ! -d $(LIB_SRC)/$(COMPILE_DIR) ]; then mkdir $(LIB_SRC)/$(COMPILE_DIR); fi)

BIN =
ifeq ($(target), ) #如果是空的
BIN = a.out
else
BIN := $(target)
endif


TARGET=$(BIN_DIR)/$(BIN)
LIB_TARGET=lib/libmyThread.a

#all: $(TARGET)
Lib: $(LIB_TARGET)

-include $(DEP)
-include $(LIBDEP)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIB_TARGET) $(LIB)

$(COMPILE_DIR)/%.o: %.cpp $(COMPILE_DIR)/%.cpp.d
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(COMPILE_DIR)/%.cpp.d: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -MM -E -c $< -o $@
	@sed 's/.*\.o/$(subst /,\/,$(dir $@))&/g' $@ > $@.tmp
	@mv $@.tmp $@

$(LIB_TARGET): $(LIBOBJS)
	ar -rcs $@ $^
$(LIB_SRC)/$(COMPILE_DIR)/%.o: $(LIB_SRC)/%.cpp $(LIB_SRC)/$(COMPILE_DIR)/%.cpp.d
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(LIB_SRC)/$(COMPILE_DIR)/%.cpp.d: $(LIB_SRC)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -MM -E -c $< -o $@
	@sed 's/.*\.o/$(subst /,\/,$(dir $@))&/g' $@ > $@.tmp
	@mv $@.tmp $@

.PHONY: clean clean_lib
clean:
	rm -rf $(COMPILE_DIR) $(BIN_DIR)
clean_lib:
	rm -rf $(LIB_SRC)/$(COMPILE_DIR)
