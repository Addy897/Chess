RAYLIB_SRC_PATH=./raylib/src/
RAYLIB_LIB_PATH=./raylib/src/


CC=gcc
CFLAGS=-I$(RAYLIB_SRC_PATH) -I./includes -L$(RAYLIB_LIB_PATH) -lraylib -lgdi32 -lwinmm -lm -Ofast


OBJECTS_DIR=obj/



SRCS=$(wildcard ./src/*.c)



OBJS=$(patsubst ./src/%.c,$(OBJECTS_DIR)/%.o,$(SRCS))


EXECUTABLE=main


all: $(EXECUTABLE)



$(OBJECTS_DIR):
	mkdir ${subst /,\,$(OBJECTS_DIR)}


$(EXECUTABLE): $(OBJS)
	$(CC) $^ $(CFLAGS) -o $@



$(OBJECTS_DIR)%.o: ./src/%.c | $(OBJECTS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	if exist ${subst /,\,$(OBJECTS_DIR)} rmdir /s /q ${subst /,\,$(OBJECTS_DIR)}
	if exist main.exe del /f /q main.exe
	if exist main del /f /q main
.PHONY: all clean
