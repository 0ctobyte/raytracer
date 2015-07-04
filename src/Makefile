SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
LDFLAGS = $(shell pkg-config --libs lua5.1) $(shell pkg-config --libs libpng12) 
CPPFLAGS = $(shell pkg-config --cflags lua5.1) $(shell pkg-config --cflags libpng12)
CXXFLAGS = $(CPPFLAGS) -std=c++11 -W -Wall -Wno-unused-parameter -g
CXX = g++
MAIN = rt

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
