CC = g++ 
CFLAGS = -Werror -g -std=c++11

SRC = $(shell ls -t | grep "\.cpp$$" | head -1)
OJB = $(SRC:.cpp=.o)
DEP = $(OJB:.o=.cpp.d)

#LIB = -L/usr/local/openssl/lib -lssl -lcrypto -ldl -lpthread
TARGET = a.out

LIB = -L/usr/local/openssl/lib -lssl -lcrypto -ldl -lpthread -ljson
INCLUDE = -I /home/tianyexing/Documents/code/include
LIB_PATH = -L /home/tianyexing/Documents/code/lib

All: $(TARGET) $(DEP)

include $(DEP)

$(TARGET): $(OJB) 
	$(CC) $(INCLUDE) $(LIB_PATH) $(CFLAGS) $(WRAPFUNC) $^ -o $@ $(LIB)
%.o: %.cpp 
	@$(CC) $(INCLUDE) $(LIB_PATH) $(CFLAGS) $(WRAPFUNC) -c $< -o $@ $(LIB)
%.cpp.d: %.cpp
	@$(CC)  $(CFLAGS) -MM -E $^ > $@


clean:
	rm -rf $(OJB) $(TARGET)