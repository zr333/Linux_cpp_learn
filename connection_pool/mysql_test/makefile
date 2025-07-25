# 编译器
CXX = g++

# 源文件和目标文件
SRC = mysql_test.cpp
TARGET = mysqltest

# 使用 mysql_config 获取编译参数和链接库
CXXFLAGS = `mysql_config --cflags`
LDFLAGS  = `mysql_config --libs`

# 默认目标

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# 清理目标
clean:
	rm -f $(TARGET)
