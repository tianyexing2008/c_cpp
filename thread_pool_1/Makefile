CC = g++
CFLAGS = -Werror -g 
LIB = -lpthread

SRC = $(shell ls -c | grep \.cpp | head -1)
OJB = $(SRC:.c=.o)

TARGET = a.out

$(TARGET): $(OJB)
	$(CC) $(CFLAGS) $^ -o $@
	rm -rf $(OJB)
%.o: %.cpp 
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(OJB) $(TARGET)