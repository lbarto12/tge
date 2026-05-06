.PHONY: run clean dist

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. -fPIC -DTGE_BUILD
LDFLAGS = -pthread
TARGET = game

GAME_SRCS = main.cpp
GAME_OBJS = $(GAME_SRCS:.cpp=.o)

TGE_SRCS = $(shell find tge -name "*.cpp")
TGE_OBJS = $(TGE_SRCS:.cpp=.o)

run: clean $(TARGET)
	LD_LIBRARY_PATH=. ./$(TARGET)

# If there are .cpp files, build and link the .so
# If not, just compile main directly
ifneq ($(TGE_SRCS),)
libtge.so: $(TGE_OBJS)
	$(CXX) -shared -o $@ $(TGE_OBJS) $(LDFLAGS)

$(TARGET): $(GAME_OBJS) libtge.so
	$(CXX) $(CXXFLAGS) $(GAME_OBJS) -o $(TARGET) -L. -ltge -Wl,-rpath,'$$ORIGIN' $(LDFLAGS)
else
$(TARGET): $(GAME_OBJS)
	$(CXX) $(CXXFLAGS) $(GAME_OBJS) -o $(TARGET) $(LDFLAGS)
endif

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

dist:
	rm -rf dist
	mkdir -p dist/include/tge
	rsync -a --include='*/' --include='*.h' --exclude='*' tge/ dist/include/tge/
ifneq ($(TGE_SRCS),)
	mkdir -p dist/lib
	$(MAKE) libtge.so
	cp libtge.so dist/lib/
endif
	@echo "Done → dist/"

clean:
	rm -f $(TARGET) $(GAME_OBJS) $(TGE_OBJS) libtge.so
