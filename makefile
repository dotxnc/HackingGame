rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard, src/, *.c) #$(wildcard src/*.cpp) $(wildcard src/engine/*.cpp)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
CFLAGS = --std=c11 -Wl,-allow-multiple-definition -Isrc/
OUT = hacking

LDFLAGS = -lraylib_new -lOpenAL32 -lglfw3 -lgdi32 -lopengl32 -lws2_32
LIBS = 
CC = gcc

$(OUT): $(OBJ)
	@ mkdir -p bin
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o bin/$(OUT)

obj/%.o: src/%.c
	@ mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -r obj/*
	rm bin/$(OUT)

