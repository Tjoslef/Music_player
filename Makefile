CC = gcc
COMMON_FLAGS = -std=c11 -O2 -D_GNU_SOURCE -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700
STRICT_FLAGS = -Wall -Wextra -Werror
CPPFLAGS = -I/usr/include/AL
LDFLAGS = -lopenal -lsqlite3

SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = $(SRC_DIR)/lib

MAIN_SRC = $(wildcard $(SRC_DIR)/*.c)
MAIN_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(MAIN_SRC))

LIB_SRC = $(wildcard $(LIB_DIR)/*.c)
LIB_OBJ = $(patsubst $(LIB_DIR)/%.c,$(OBJ_DIR)/lib/%.o,$(LIB_SRC))

OBJ = $(MAIN_OBJ) $(LIB_OBJ)
EXEC = main
EXEC_DIR = exec
EXECUTABLE = $(EXEC_DIR)/$(EXEC)

$(shell mkdir -p $(OBJ_DIR) $(OBJ_DIR)/lib $(EXEC_DIR))
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(COMMON_FLAGS) $^ -o $(EXECUTABLE) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(COMMON_FLAGS) $(STRICT_FLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/lib/%.o: $(LIB_DIR)/%.c
	$(CC) $(COMMON_FLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(EXECUTABLE)
.PHONY: all clean
