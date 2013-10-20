INCLUDE_DIR=./include
INCLUDE_SUBDIRS=. codec container quantize mesh debug compress
HEADERS=$(wildcard $(patsubst %,$(INCLUDE_DIR)/onika/%/*.h,$(INCLUDE_SUBDIRS)) )
TESTS=$(patsubst $(INCLUDE_DIR)/%.h,bin/%_TEST,$(HEADERS))
#EXE=bin/meshcompress
VTK=

#CXX=colorgcc
CXX=g++
#CXX=clang++
#PROF_FLAGS= -DPROFILING=1 
OPT_FLAGS=-g3 -DDEBUG=1 -D_DEBUG=1
#OPT_FLAGS=-march=athlon64 -O3
#OPT_FLAGS= -O3
CFLAGS= -std=c++11 -Iinclude $(OPT_FLAGS) $(PROF_FLAGS)
#CFLAGS= -Iinclude $(OPT_FLAGS) $(PROF_FLAGS)
#RM=syncrm
RM=rm

#-march=athlon64 -O3 -finline-limit=1000000

all: verbose test exe

verbose:
	@echo "INCLUDE_DIR = $(INCLUDE_DIR)"
	@echo "INCLUDE_SUBDIRS = $(INCLUDE_SUBDIRS=.)"
	@echo "HEADERS = $(HEADERS)"
	@echo "TESTS = $(TESTS)"
	@echo "EXE = $(EXE)"

exe: $(EXE)

test: $(TESTS)

bin/%: src/%.cpp
	$(CXX) $(CFLAGS) $< -o $@

bin/%_TEST: include/%.h
	mkdir -p `dirname $@`
	$(CXX) -xc++ $(CFLAGS) -D`echo onika_$<_TEST | sed 's/include\/onika\///g' | sed 's/\.h//g' | sed 's/\.\///g' | tr "./" "_"`=1 $< -o $@

clean:
	$(RM) -f $(TESTS) $(EXE)
	find . -name "*~*" -exec $(RM) -f {} \;
# DO NOT DELETE
