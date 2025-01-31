CC = gcc
COMMON_FLAGS = -std=c11 -O2 -D_POSIX_C_SOURCE=199309L
STRICT_FLAGS = -Wall -Wextra -Werror
CPPFLAGS = -I/usr/include/AL
LDFLAGS = -lopenal -lsqlite3


MAIN_SRC = $(wildcard src/*.c)
MAIN_OBJ = $(SRC:.c=.o)

LIB_SRC = src/lib/dotenv.c
LIB_OBJ = $(LIB_SRC:.c=.o)

OBJ = $(MAIN_OBJ) $(LIB_OBJ)
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(COMMON_FLAGS) $(OBJ) -o $(EXEC) $(LDFLAGS)
src/main.o: src/main.c
	$(CC) $(COMMON_FLAGS) $(COMMON_FLAGS) $(CPPFLAGS) -c $< -o $@

src/lib/%.o: src/lib/%.c
	$(CC) $(COMMON_FLAGS) $(CPPFLAGS) -c $< - o $@
clean:
	rm -f $(OBJ) $(EXEC)
