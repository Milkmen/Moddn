# Compiler and tools
CC      	= i686-w64-mingw32-gcc

# Directories
SRC_DIR		= src
BUILD_DIR	= build
BIN_DIR 	= bin
LUAJIT_DIR  = lib/luajit/src

OUT     	= $(BIN_DIR)/moddn.dll

CFLAGS  	= -std=c99 -O2 -Wall -I$(BUILD_DIR) -I$(LUAJIT_DIR)
LDFLAGS 	= -shared -Wl,--kill-at
LIBS    	= $(BUILD_DIR)/libluajit.a -lwinmm

# Recursively find all .c files
SRC     	= $(shell find $(SRC_DIR) -name '*.c')
OBJ     	= $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRC:.c=.o))

all: setup $(OUT)

# Link DLL with proper CRT startup
$(OUT): $(OBJ)
	@echo -e "LINK\t$@"
	$(CC) -m32 $(LDFLAGS) -o $@ $^ $(LIBS)

# Compile .c files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo -e "CC\t$<"
	$(CC) -m32 $(CFLAGS) -c $< -o $@

# Prepare build directory and LuaJIT static lib
setup: $(LUAJIT_DIR)/libluajit.a
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@cp $(LUAJIT_DIR)/libluajit.a $(BUILD_DIR)/

# Clean build
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean setup
