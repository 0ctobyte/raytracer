SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
LDFLAGS = $(shell pkg-config --libs lua5.1) -llua5.1 -lpng
CPPFLAGS = $(shell pkg-config --cflags lua5.1)
CXXFLAGS = $(CPPFLAGS) -std=c++11 -Wno-c++0x-compat -W -Wall -g
CXX = g++
MAIN = rt

ifeq ($(shell uname), Darwin)
LDFLAGS = -L/usr/local/opt/libpng12/lib $(shell pkg-config --libs lua5.1) -lpng
CPPFLAGS = -I/usr/local/opt/libpng12/include $(shell pkg-config --cflags lua5.1)
CXXFLAGS += -Wno-c++11-extensions
endif

all: $(MAIN)

depend: $(DEPENDS)

clean:
	rm -f *.o *.d $(MAIN)

$(MAIN): $(OBJECTS)
	@echo Creating $@...
	@$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) -o $@ -c $(CXXFLAGS) $<

%.d: %.cpp
	@echo Building $@...
	@set -e; $(CC) -M $(CPPFLAGS) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@

include $(DEPENDS)
