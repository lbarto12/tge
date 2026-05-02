.PHONY: run clean

SRCS = main.cpp $(shell find src -name "*.cpp")
INCLUDE_DIRS = $(shell find src -type d)
INCLUDE_FLAGS = $(addprefix -I,$(INCLUDE_DIRS))
CXX = g++
CXXFLAGS = -std=c++17 -Wall $(INCLUDE_FLAGS)
LDFLAGS = -pthread
TARGET = game
OBJS = $(SRCS:.cpp=.o)

run: clean $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
