CC=gcc
#SRCS = $(wildcard ./*.c)
SRCS = $(shell ls -c | grep ".c$$" | head -1)
OBJS = $(SRCS:.c=.o)
TARGET = pthread

CFLAGS = -g -Wall -lpthread

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	rm $(OBJS)
#.c.o:
#	$(CC) -c $<

$(OBJS):$(SRCS)
	$(CC) -c $< -o $@

clean:
	rm -rf $(TARGET) 



