MAKE := make
CC := gcc
CFLAGS := -O3 -Wall -fopenmp
CXXFLAGS := -std=c++0x
LIBS := -lc -lstdc++ -lm -lgsl -lgslcblas -lboost_program_options

TARGETS := estimate csv2dump
CLASSDIR := src/class
CLASSSRCS := $(wildcard $(CLASSDIR)/*.cpp)
CLASSOBJS := $(patsubst %.cpp,%.o,$(CLASSSRCS))

.PHONY: all clean

all: $(TARGETS)

$(TARGETS): %: bin/%

bin/%: src/%.o $(CLASSOBJS) src/lib/util.o
	@if [ ! -e bin ]; then mkdir -p bin; fi
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

src/lib/util.o: src/lib/util.c
	$(CC) $(CFLAGS) -o $@ -c $<

src/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) -o $@ -c $<

# cleaning
clean:
	rm -f src/*.o src/lib/*.o $(CLASSOBJS)
