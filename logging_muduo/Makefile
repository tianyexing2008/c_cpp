COMPILE_DIR = compile
BIN_DIR = bin

# 可编译arm版本
#CROSS = arm-himix200-linux-
CC = $(CROSS)g++ -std=c++11
CFLAGS = -Werror -g
INCLUDE = -I./include
LIB = -lpthread
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, $(COMPILE_DIR)/%.o, $(SRCS))

DEP = $(patsubst %.o, %.d, $(OBJS))

$(shell if [ ! -d $(COMPILE_DIR) ]; then mkdir $(COMPILE_DIR); fi)
$(shell if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi)


TARGET=$(BIN_DIR)/a.out

all: $(TARGET)

-include $(DEP)

$(TARGET): $(OBJS)
	$(CC) $(INCLUDE) $(CFLAGS) $^ -o $@ $(LIB)

$(COMPILE_DIR)/%.o: %.cpp $(COMPILE_DIR)/%.d
	$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@ $(LIB)


$(COMPILE_DIR)/%.d: %.cpp
	@$(CC) $(CFLAGS) $(INCLUDE) -MM -E -c $< -o $@
	@sed 's/.*\.o/$(subst /,\/,$(dir $@))&/g' $@ > $@.tmp
	@mv $@.tmp $@

.PHONY: clean
clean:
	rm -rf $(COMPILE_DIR) $(BIN_DIR)
