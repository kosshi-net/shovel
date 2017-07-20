# -*- MakeFile -*-

SRC= src/
BIN= bin/
BUILD= build/
CC= g++
CFLAGS= -O3 -ftree-vectorize -msse2 -static -fopenmp -Wall -static-libgcc -static-libstdc++ -std=c++11
# -Wl,-subsystem,windows -fopt-info-vec-missed 
# CFLAGS=-fopenmp -static  -Wall -static-libgcc -static-libstdc++
# CFLAGS=-fopenmp -Wall
EXT = C:/ext/
GLFW = $(EXT)glfw-3.2.1/
GLEW = $(EXT)glew-2.0.0/
SIMPLEX = $(EXT)simplex-1.0/
GLM = $(EXT)glm-0.9.8.4/

INC = -I $(GLFW)include -I $(GLEW)include -I $(SIMPLEX)include -I $(GLM)include -I include
LIB =    $(GLEW)lib/glew.o -L $(GLFW)lib $(SIMPLEX)lib/simplex.o

HEAD = include/

$(BIN)game: $(BUILD)game.o $(BUILD)renderer.o $(BUILD)terrain.o $(BUILD)terrainmesher.o;
	$(CC) $(BUILD)game.o $(BUILD)renderer.o $(BUILD)terrain.o $(BUILD)terrainmesher.o $(LIB) -lglfw3 -lgdi32 -lopengl32 $(CFLAGS) -o $(BIN)game.exe 

$(BUILD)game.o: $(SRC)game.cpp $(HEAD)defineterrain.hpp
	$(CC) -c $(SRC)game.cpp $(INC) $(CFLAGS) -o $(BUILD)game.o

$(BUILD)renderer.o: $(SRC)renderer.cpp $(BUILD)res.h
	$(CC) -c $(SRC)renderer.cpp $(INC) $(CFLAGS) -o $(BUILD)renderer.o

$(BUILD)terrain.o: $(SRC)terrain.cpp
	$(CC) -c $(SRC)terrain.cpp $(INC) $(CFLAGS) -o $(BUILD)terrain.o

$(BUILD)terrainmesher.o: $(SRC)terrainmesher.cpp
	$(CC) -c $(SRC)terrainmesher.cpp $(INC) $(CFLAGS) -o $(BUILD)terrainmesher.o



$(BUILD)res.h: $(SRC)shader.glsl
	node scripts/res.js

clean:
	del build\*
	del bin\game.exe

test: $(BIN)game
	bin/game