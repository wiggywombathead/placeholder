CXX = g++
CXXFLAGS = -g -std=gnu++11 

LDLIBS = -lGL -lGLU -lGLEW -lglfw -lSOIL -lX11 -lm -lpng
LIBDIRS = -L/usr/X11R6/lib
LDFLAGS = $(CXXFLAGS) $(LIBDIRS)

SRCDIR = src
OBJDIR = objs

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

BIN = run

all: $(OBJS) 
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $(BIN) $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(LDFLAGS) $(LDLIBS) -c $< -o $@

tidy:
	@rm -rf objs

clean:
	@rm -rf objs
	@rm -f $(BIN)
