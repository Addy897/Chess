RAYLIB_SRC_PATH=./raylib/src/
CFLAGS=-I$(RAYLIB_SRC_PATH) -L$(RAYLIB_SRC_PATH) -lraylib -lgdi32 -lwinmm
CC=gcc
FILE=main
main: $(FILE).c
	$(CC) -Ofast $< $(CFLAGS) -o $(FILE) -pg