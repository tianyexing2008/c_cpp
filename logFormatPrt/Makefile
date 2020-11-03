CC = gcc
#SRCS = $(wildcard ./*.c)
SRCS = $(shell ls -c | grep ".c$$" | head -1)
OBJS = $(SRCS:.c=.o)
TARGET = a.out

CFLAGS = -g -Wall

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	rm $(OBJS)
%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -rf $(TARGET) 
