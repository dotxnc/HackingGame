rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard, src/, *.c) #$(wildcard src/*.cpp) $(wildcard src/engine/*.cpp)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
CFLAGS = --std=c11 -Wno-incompatible-pointer-types -Wno-int-conversion -Isrc/
NAME = hacking
OUT =

LDFLAGS = -Wl,-allow-multiple-definition
LIBS = 
CC = 
ifeq ($(OS),Windows_NT)
	OUT = $(NAME).exe
	CC += clang
	LIBS = -lraylib -lglfw3 -lopengl32 -lgdi32 -lws2_32
else
	OUT = $(NAME)
	CC += gcc
	LIBS = -lraylib -lglfw3 -lGL -lXxf86vm -lXext -lX11 -lXrandr -lXi -lXinerama -lXcursor -lm -lpthread -ldl 
endif

$(OUT): $(OBJ)
	@ mkdir -p bin
	$(CC) -o bin/$(OUT) $(CFLAGS) $(OBJ) $(LIBS) $(LDFLAGS)

obj/%.o: src/%.c
	@ mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -r obj/*
	rm bin/$(OUT)
