# TGE Demo Makefile
# Usage: place your main.cpp next to the tge/ folder and run `make`
#
# Your project should look like:
#   project/
#   ├── Makefile      ← this file
#   ├── main.cpp      ← your code
#   └── tge/
#       └── include/
#           └── tge/
#               ├── game.h
#               ├── graphics.h
#               ├── input.h
#               └── ...

.PHONY: run clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Idist/include
LDFLAGS = -pthread
TARGET = game

# Add any .cpp files you create to this list
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# If tge ships with a shared library, link it
TGE_LIB = $(wildcard tge/lib/libtge.so tge/lib/tge.dll)
ifneq ($(TGE_LIB),)
    LDFLAGS += -Ltge/lib -ltge -Wl,-rpath,'$$ORIGIN/tge/lib'
endif

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
