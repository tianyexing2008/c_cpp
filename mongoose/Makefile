CC = g++ #arm-himix200-linux-gcc
CFLAGS = -Wall -g#-Wl,--wrap=fopen
#LIB = -L/usr/local/openssl/lib -lssl -lcrypto -ldl -lpthread
INCLUDE = -I../json/include 
LIB = -L../json/lib -lpthread -lssl -lcrypto -ljsoncpp
SRC = main.cpp #$(shell ls -t | grep "\.c$$" | head -1)
OJB = $(SRC:.cpp=.o)
DEP = $(OJB:.o=.cpp.d)
SO_LIBARAY = libWebServer.so

ifneq ($(target),)
	TARGET = $(target)
else
	TARGET = a.out
endif

All: $(SO_LIBARAY) $(TARGET)

include $(DEP)

$(TARGET): $(OJB) 
	$(CC)  $(CFLAGS) $^ -o $@ ./$(SO_LIBARAY) $(LIB)

$(OJB) : $(SRC) 
	@$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@ $(LIB)


#±àÒë¶¯Ì¬¿â
$(SO_LIBARAY): mongoose.o
	gcc $(CFLAGS) -shared $< -o $@ $(LIB)

mongoose.o: mongoose.c mongoose.h
	gcc $(CFLAGS) -fPIC -c $< -o $@ $(LIB)

%.cpp.d: %.cpp
	@$(CC)  $(CFLAGS) -MM -E $^ > $@
clean:
	rm -rf $(OJB) $(TARGET) $(SO_LIBARAY) $(DEP) mongoose.o
