rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard, src/, *.c) #$(wildcard src/*.cpp) $(wildcard src/engine/*.cpp)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
CFLAGS = --std=c11 -Wl,-allow-multiple-definition -Wno-incompatible-pointer-types -Wno-int-conversion -Isrc/
OUT = hacking

LDFLAGS = -lraylib -lglfw3 -lGL -lXxf86vm -lXext -lX11 -lXrandr -lXi -lXinerama -lXcursor -lm -lpthread -ldl
LDFLAGS = -lraylib -lOpenAL32 -lglfw3 -lopengl32 -lgdi32 -lws2_32
# LDFLAGS = -lmingw32 -lraylib -mwindows -Wl,--no-undefined -lws2_32 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc
LIBS = 
CC = gcc

$(OUT): $(OBJ)
	@ mkdir -p bin
	$(CC) -o bin/$(OUT) $(CFLAGS) $(OBJ) $(LDFLAGS)

obj/%.o: src/%.c
	@ mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -r obj/*
	rm bin/$(OUT)
