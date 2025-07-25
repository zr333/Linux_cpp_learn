CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lmysqlclient -ljsoncpp -lpthread

TARGET = main
SRCS = main.cpp connPool.cpp MysqlConn.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
