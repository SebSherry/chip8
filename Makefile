SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
ROM_DIR := tests
BIN_ROM_DIR := roms

EXE := $(BIN_DIR)/chip8
SRC := $(wildcard $(SRC_DIR)/*.c)
TESTS := $(wildcard $(ROM_DIR)/*.8o)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CH8 := $(TESTS:$(ROM_DIR)/%.8o=$(BIN_ROM_DIR)/%.ch8)

CPPFLAGS := -MMD -MP
CFLAGS 	 := -Wall
LDFLAGS  := -Llib
LDLIBS   := -lSDL2

OCTO 	 := octo

.PHONY: all clean tests

all: $(EXE)

tests: $(CH8)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_ROM_DIR)/%.ch8: $(ROM_DIR)/%.8o | $(BIN_ROM_DIR)
	$(OCTO) $< $@

$(BIN_DIR) $(OBJ_DIR) $(BIN_ROM_DIR):
	mkdir -p $@

clean:    
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) $(BIN_ROM_DIR)

-include $(OBJ:.o=.d)
