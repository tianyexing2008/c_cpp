
CC = g++ -std=c++11
CFLAGS = -Werror -g
INCLUDE = -I../include -I../../curl-7.75.0/include/curl
LIB = -L../lib -L/usr/local/openssl/lib/ -lssl -lcurl -lcrypto -ldl -lpthread -lz -ljsoncpp

SRC = $(wildcard *.cpp)
OJB = $(SRC:.cpp=.o)
DEP = $(OJB:.o=.cpp.d)
TARGET = a.out

All: $(TARGET) $(DEP)

include $(DEP)

$(TARGET): $(OJB) 
	$(CC) $(INCLUDE) $(CFLAGS) $(WRAPFUNC) $^ -o $@  $(LIB)

%.o: %.cpp %.cpp.d
	@$(CC) $(INCLUDE) $(CFLAGS) $(WRAPFUNC) -c $< -o $@ $(LIB)

%.cpp.d: %.cpp
	@$(CC) $(INCLUDE) $(CFLAGS) -MM -E $^ > $@



clean:
	rm -rf $(OJB) $(TARGET) $(DEP)
