CXX = g++
CXXFLAGS = -Wall -O2 -std=c++11
LIBS = -lpthread -lboost_system -lboost_filesystem -lboost_date_time

SOURCES = sit.cc Core.cc Diff.cc FileSystem.cc Index.cc Objects.cc Util.cc
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
	