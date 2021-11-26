TARGET = maze-gen

CC = clang

SRC = src
OBJ = obj
# INCLUDE = include

# LIBS = $(shell pkg-config --libs --cflags sdl2 sdl2_gfx SDL2_ttf) # libs to include

CFLAGS = -Wall -Wextra -std=c99 -Wno-unused-command-line-argument -pedantic $(LIBS)

SRCS = $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/**/*.c) # get all src files
# INCLUDES = $(wildcard $(INCLUDE)/*.c) # get all include files

# create all obj file names from src and include files
# OBJS = $(patsubst $(INCLUDE)/%.c, $(OBJ)/%.o, $(INCLUDES))
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

# if dir does not exist create it
TREE = $(dir $(OBJS))
$(foreach dir, $(TREE), $(shell if [ ! -d "${dir}" ]; then mkdir -p ${dir}; fi;))

all: $(TARGET)

# create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

debug:
	$(CC) -o $(TARGET) $(SRCS) $(INCLUDES) $(CFLAGS) -g

# $(OBJ)/%.o: $(INCLUDE)/%.c
# $(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ k

# create all object files
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET)
	$(RM) -r *.dSYM
	$(RM) -r $(OBJ)
