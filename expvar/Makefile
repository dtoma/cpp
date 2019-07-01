# From https://www.gnu.org/software/make/manual/html_node/Catalogue-of-Rules.html#Catalogue-of-Rules
# C:    n.o is made automatically from n.c with a recipe of the form ‘$(CC) $(CPPFLAGS) $(CFLAGS) -c’
# C++:  n.o is made automatically from n.cc, n.cpp, or n.C with a recipe of the form ‘$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c’
# Link: n is made automatically from n.o by running the linker (usually called ld) via the C compiler.
#       The precise recipe used is ‘$(CC) $(LDFLAGS) n.o $(LOADLIBES) $(LDLIBS)’

CXX ?= g++
CC := $(CXX)

EXECUTABLE := expvar-cpp.exe

LDFLAGS := -L ./leveldb
LDLIBS  := -lleveldb -lpthread

OBJECTS := picohttpparser/picohttpparser.o \
           lib/http.o \
	   lib/network.o \
	   lib/db.o \
	   main.o

CXXFLAGS := -std=c++14 \
            -Wall \
            -Wextra \
            -Wshadow \
            -Wnon-virtual-dtor \
            -pedantic \
            -Wold-style-cast \
            -Wcast-align \
            -Wunused \
            -Woverloaded-virtual \
            -Weffc++ \
            -Wconversion \
            -O3 \
            -march=native \
	    -isystem ./picohttpparser -isystem ./leveldb -I ./lib

CFLAGS := $(CXXFLAGS)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)

clean:
	@rm -f $(OBJECTS) $(EXECUTABLE)

re: clean all

.PHONY: all $(EXECUTABLE) clean re
