CC = g++ #arm-himix200-linux-gcc
CFLAGS = -Wall -g --std=c++11
INCLUDE = -I../include/
LIB = -lpthread


SRC_APP = $(wildcard *.cpp)
OJB_APP = $(SRC_APP:%.cpp=%.o)
DEP_APP = $(SRC_APP:%.cpp=%.cpp.d)

OBJS = $(SRC_CPP_LIBARAY:.cpp=.o)

TARGET = a.out


All: $(TARGET) $(DEP_APP)


-include $(DEP_APP)

#a.out编译
$(TARGET): $(OJB_APP) 
	$(CC)  $(INCLUDE) $(CFLAGS) $^ -o $@  $(LIB)

%.o: %.cpp %.cpp.d
	$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@ $(LIB)

%.cpp.d: %.cpp
	@$(CC) $(INCLUDE) $(CFLAGS) -MM -E $^ > $@


clean:
	rm -rf $(OJB_APP) $(DEP_APP) $(TARGET)