COMPILE_DIR = compile
BIN_DIR = bin

CC = g++ -std=c++11
CFLAGS = -Werror
INCLUDE = -I../include/
LIB = -L../lib/ -lpaho-mqtt3a -ljson -lpthread

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, $(COMPILE_DIR)/%.o, $(SRCS))
DEP = $(patsubst %.o, %.d, $(OBJS))

$(shell if [ ! -d $(COMPILE_DIR) ]; then mkdir $(COMPILE_DIR); fi)
$(shell if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi)


TARGET=a.out

all: $(TARGET)

-include $(DEP)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIB)
	@mv $@ $(BIN_DIR)

$(COMPILE_DIR)/%.o: %.cpp $(COMPILE_DIR)/%.d
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@


$(COMPILE_DIR)/%.d: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -MM -E -c $< -o $@
	@sed 's/.*\.o/$(subst /,\/,$(dir $@))&/g' $@ > $@.tmp
	@mv $@.tmp $@

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/$(TARGET) $(OBJS) $(DEP)