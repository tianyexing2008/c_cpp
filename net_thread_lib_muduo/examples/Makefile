COMPILE_DIR = compile
BIN_DIR = bin

# 可编译arm版本
# CROSS = arm-himix200-linux-
CC = $(CROSS)g++ -std=c++11
CFLAGS = -Werror -g
INCLUDE = -I../include -I../logging -I../net
LIB = -L../net/lib/ -L../lib/ -L../logging/lib/ -lNet -lLogging -lmyThread  -lpthread

# 创建编译目录和二进制目录
$(shell mkdir -p $(COMPILE_DIR) $(BIN_DIR))

# ************************** Echo目标配置 **************************
# Echo目标的源文件（仅包含simple目录下的echo相关文件）
ECHO_SRCS = simple/echo.cpp
# Echo目标的输出二进制文件
ECHO_TARGET = $(BIN_DIR)/echo
# Echo目标的中间目标文件（.o文件）
ECHO_OBJS = $(patsubst %.cpp, $(COMPILE_DIR)/%.o, $(ECHO_SRCS))

# ************************** 其他目标可在此处添加 **************************
FINGER_SRCS = simple/fingerShutdown.cpp
FINGER_TARGET = $(BIN_DIR)/finger
FINGER_OBJS = $(patsubst %.cpp, $(COMPILE_DIR)/%.o, $(FINGER_SRCS))


# 默认目标（不指定标签时执行）
#all: Echo

# Echo目标编译规则
Echo: $(ECHO_TARGET)
Finger: $(FINGER_TARGET)

# 生成Echo二进制文件
$(ECHO_TARGET): $(ECHO_OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIB)

$(FINGER_TARGET): $(FINGER_OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIB)

# 编译Echo的源文件为.o文件
$(COMPILE_DIR)/simple/%.o: simple/%.cpp
	@mkdir -p $(dir $@)  #确保编译目录的子目录存在
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@


# ************************** 其他目标的编译规则可在此处添加 **************************
# 示例：Test目标
# Test: $(TEST_TARGET)
# 
# $(TEST_TARGET): $(TEST_OBJS)
# 	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIB)
# 	@echo "Test目标编译完成: $@"
# 
# $(COMPILE_DIR)/simple/%.o: simple/%.cpp
# 	@mkdir -p $(dir $@)
# 	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@


# 清理规则
.PHONY: clean Echo  # 确保标签被识别为伪目标
clean:
	rm -rf $(COMPILE_DIR) $(BIN_DIR)
    