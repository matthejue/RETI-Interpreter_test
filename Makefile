TESTCLASS_BASE = $(shell basename --suffix=.picoc $(TESTCLASS))
CXX = gcc -g -Wall
MAIN_BINARIES = $(basename $(wildcard ./src/*_main.c))
TEST_BINARIES = $(basename $(wildcard ./src/*_test.c))
HEADERS = $(wildcard ./src/*.h)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %_main.c %_test.c, $(wildcard ./src/*.c))))
LIBRARIES =

.PRECIOUS: %.o
.SUFFIXES:
.PHONY: clean compile test

all: compile test

compile: $(MAIN_BINARIES) $(TEST_BINARIES)

test: $(TEST_BINARIES)
	# for T in $(TEST_BINARIES); do ./$$T; done
	# start with 'make test-arg ARG=file_basename'
	# DEBUG=-d for debugging
	./run_tests.sh $${COLUMNS} $(TESTCLASS_BASE) $(VERBOSE) $(DEBUG);

test-clean: test clean

clean:
	rm -f ./src/*.o
	rm -f $(MAIN_BINARIES)
	rm -f $(TEST_BINARIES)

%_main: ./src/%_main.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBRARIES)

%_test: ./src/%_test.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBRARIES)

%.o: ./src/%.c $(HEADERS)
	$(CXX) -c $<

print-main-binaries:
	@echo "MAIN_BINARIES: $(MAIN_BINARIES)"
