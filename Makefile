CXX = g++
CXXFLAGS = -g -std=gnu++11 

LDLIBS = -lGL -lGLU -lGLEW -lglfw -lSOIL -lX11 -lm -lpng
LIBDIRS = -L/usr/X11R6/lib
LDFLAGS = $(CXXFLAGS) $(LIBDIRS)

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)

BIN = run

%: %.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $< -o $@

%.o: %.cpp
	$(CXX) $(LDFLAGS) $(LDLIBS) -c $<

all: $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $(BIN) $(OBJS)

clean:
	@rm *.o
