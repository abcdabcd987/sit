CXX = g++
CXXFLAGS = -Wall -O2 -std=c++11
LIBS = -lpthread -lboost_system -lboost_filesystem

SOURCES = sit.cc sit-add.cc sit-init.cc sit-sha1.cc
OBJECTS = $(SOURCES:.cc=.o)
EXECUTABLE = sit

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -o $@ $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	