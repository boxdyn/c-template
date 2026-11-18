# John's "drop-in" makefile
# (C) 2019-2026 John "Boxdyn" (j@soft.fish)
# See License.md

# ------- Build Settings ------- #

# Target directory
BUILD = build

# Executable
OUT := $(BUILD)/a.out

# Paths to source, include, dependency, and object files
EXT = .c
HXT = .h

SRC = src
INC = $(shell dirname $(HEADERS) | sort | uniq)
DEP = $(BUILD)/deps
OBJ = $(BUILD)/objects

# compiler and compiler flags
CC = clang
CPP = clang++

FLAGS = $(addprefix -I,$(INC)) -O3 -flto -fpic -fdefer-ts -Wall -Wextra
CFLAGS = -std=c2y $(FLAGS) $(EXTFLAGS) $(EXTCFLAGS)
CPPFLAGS = -std=c++26 $(FLAGS) $(EXTFLAGS) $(EXTCPPFLAGS)

# list of static object files
SOURCES = $(shell find $(SRC) -type f '(' -name '*'$(EXT) -o -name '*'$(EXT)pp ')' 2>/dev/null)
HEADERS = $(shell find $(SRC) -type f -name '*'$(HXT) 2>/dev/null)
OBJECTS = $(patsubst $(SRC)/%$(EXT),$(OBJ)/%.o,$(patsubst $(SRC)/%$(EXT)pp,$(OBJ)/%.opp,$(SOURCES)))
DEPENDS = $(shell find $(DEP) -type f -name '*'.d 2>/dev/null) $(shell find $(DEP) -type f -name '*'.dpp 2>/dev/null)
DIRS =  $(dir $(OUT))

# -------- Build Targets ------- #
# Some targets aren't real
.PHONY: all dump clean clear run bear
# Don't autodelete object files:
.PRECIOUS: $(OBJ)/%.o $(OBJ)/%.opp

all: $(OUT)

dump:
	@echo "    SRC: $(SRC)"
	@echo "    INC: $(INC)"
	@echo "    DEP: $(DEP)"
	@echo "    OBJ: $(OBJ)"
	@echo " CFLAGS: $(CFLAGS)"
	@echo "SOURCES: $(SOURCES)"
	@echo "HEADERS: $(HEADERS)"
	@echo "DEPENDS: $(DEPENDS)"
	@echo "OBJECTS: $(OBJECTS)"

clean:
	-rm -r $(BUILD)

clear:
	@clear

run: $(OUT)
	-$(addprefix ./,$(addsuffix ;,$(OUT)))

bear: clean
	@rm compile_commands.json
	-$(MAKE) compile_commands.json

docs: $(BUILD)/docs/html/index.html
	@xdg-open $<

compile_commands.json: $(SOURCES) $(HEADERS)
	bear -o $@ -- $(MAKE) all

$(BUILD)/docs/html/index.html: $(SOURCES) $(HEADERS) README.md Doxyfile compile_commands.json
	@mkdir -p "$(dir $@)"
	doxygen

$(DIRS):
	@mkdir -p "$@"

# Make the executable
$(BUILD)/%.out: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Make the object and dependency files
$(OBJ)/%.o: $(SRC)/%$(EXT)
	@mkdir -p "$(dir $@)" "$(patsubst $(OBJ)/%,$(DEP)/%,$(dir $@))"
	$(CC) $(CFLAGS) -MMD -MF "$(patsubst $(OBJ)/%.o,$(DEP)/%.d,$@)" -o "$@" -c "$<"

$(OBJ)/%.opp: $(SRC)/%$(EXT)pp
	@mkdir -p "$(dir $@)" "$(patsubst $(OBJ)/%,$(DEP)/%,$(dir $@))"
	$(CPP) $(CPPFLAGS) -MMD -MF "$(patsubst $(OBJ)/%.opp,$(DEP)/%.dpp,$@)" -o "$@" -c "$<"


# --------- Inclusions --------- #
# use dependencies when rebuilding
-include $(DEPENDS)
