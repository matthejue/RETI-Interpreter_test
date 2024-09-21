# based on source: https://stackoverflow.com/a/30602701

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := unit_test
OBJ_TEST_DIR := obj_test
INCLUDE_DIR := include
LIB_DIR := lib

BIN_SRC := $(BIN_DIR)/$(basename $(notdir $(wildcard $(SRC_DIR)/*_main.c)))
BIN_TEST := $(BIN_DIR)/$(basename $(notdir $(wildcard $(TEST_DIR)/*_test.c)))
SRC := $(filter-out %_main.c %_test.c, $(wildcard $(SRC_DIR)/*.c))
OBJ_SRC := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -I$(INCLUDE_DIR) -MMD -MP
CFLAGS   := -Wall -g -O2
LDFLAGS  := -L$(LIB_DIR)
LDLIBS   := -lm

.PRECIOUS: $(OBJ_DIR)/%.o $(OBJ_TEST_DIR)/%.o
.PHONY: all test test-full clean debug

all: $(BIN_SRC)

unit-test: $(BIN_TEST)
		for T in $(BIN_TEST); do ./$$T; done

$(BIN_DIR)/%_main: $(OBJ_DIR)/%_main.o $(OBJ_SRC) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR)/%_test: $(OBJ_TEST_DIR)/%_test.o $(OBJ_SRC) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_TEST_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_TEST_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR) $(OBJ_TEST_DIR):
	mkdir -p $@

sys-test:
	./run_tests.sh $${COLUMNS} $(TESTCLASS_BASE) $(VERBOSE) $(DEBUG);

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) $(OBJ_TEST_DIR)

debug:
	# p/x $pc with # break *0x555555556543 or break src/interpret.c:234 or break exit
	make
	gdb --tui -n -x ./.gdbinit --args ./bin/interpret_main -f /tmp ./tests/all_operations.reti

-include $(OBJ:.o=.d)
