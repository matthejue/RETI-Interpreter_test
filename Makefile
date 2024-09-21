# source: https://stackoverflow.com/a/30602701

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INCLUDE_DIR := include
LIB_DIR := lib

BIN := $(BIN_DIR)/$(basename $(notdir $(wildcard $(SRC_DIR)/*_main.c)))
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -I$(INCLUDE_DIR) -MMD -MP
CFLAGS   := -Wall -g
LDFLAGS  := -L$(LIB_DIR)
LDLIBS   := -lm

.PHONY: all test clean debug

all: $(BIN)

$(BIN): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

test:
	./run_tests.sh $${COLUMNS} $(TESTCLASS_BASE) $(VERBOSE) $(DEBUG);

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

debug:
	# p/x $pc with # break *0x555555556543 or break src/interpret.c:234 or break exit
	make
	gdb --tui -n -x ./.gdbinit --args ./bin/interpret_main -f /tmp ./tests/all_operations.reti

-include $(OBJ:.o=.d)
