CC = gcc
COMMON_FLAGS = -std=c11 -O2 -D_GNU_SOURCE -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700
STRICT_FLAGS = -Wall -Wextra -Werror
CPPFLAGS = -I/usr/include/AL
LDFLAGS = -lopenal -lsqlite3

MAIN_SRC = $(wildcard src/*.c)
MAIN_OBJ = $(MAIN_SRC:.c=.o)

LIB_SRC = $(wildcard src/lib/*.c)
LIB_OBJ = $(LIB_SRC:.c=.o)

OBJ = $(MAIN_OBJ) $(LIB_OBJ)
EXEC = main
EXEC_DIR = exec
EXECUTABLE = $(EXEC_DIR)/$(EXEC)
$(shell mkdir -p $(EXEC_DIR))
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(COMMON_FLAGS) $^ -o $@ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(COMMON_FLAGS) $(STRICT_FLAGS) $(CPPFLAGS) -c $< -o $@

src/lib/%.o: src/lib/%.c
	$(CC) $(COMMON_FLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

