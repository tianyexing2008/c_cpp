CC=gcc
#SRCS = $(wildcard ./*.c)
SRCS = $(shell ls -c | grep ".c$$" | head -1)
OBJS = $(SRCS:.c=.o)
TARGET = a.out
LIB = -lpthread

CFLAGS = -g -Wall 

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)
	rm $(OBJS)
#.c.o:
#	$(CC) -c $<

$(OBJS):$(SRCS)
	$(CC) -c $< -o $@ $(LIB)

clean:
	rm -rf $(TARGET) 



