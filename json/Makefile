CPP = g++
CFLAGS = -g -Wall -I ./include
RM = rm -rf 
LIB = libjson.a


TARGET = libjson.a
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all: $(TARGET) $(OBJ)


$(TARGET): $(OBJ)
	ar cr $@ $^ 
	$(RM) $(OBJ)
	@echo -e "\033[31m"
	@echo "##############################"
	@ls -l $(TARGET)
	@echo "##############################"
	@echo -e "\033[0m"
%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@




clean:
	$(RM) $(TARGET) $(OBJ)





