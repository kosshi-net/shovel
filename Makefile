# -*- MakeFile -*-

SRC= src/
BIN= bin/
BUILD= build/
CC= g++
CFLAGS= -O3 -ftree-vectorize -msse2 -static -fopenmp -Wall -static-libgcc -static-libstdc++ -std=c++11 -m32
# CFLAGS= -g -fopenmp -Wall -std=c++11
#-Wl,-subsystem,windows
# -Wl,-subsystem,windows -fopt-info-vec-missed 
# CFLAGS=-fopenmp -static  -Wall -static-libgcc -static-libstdc++
# CFLAGS=-fopenmp -Wall -O0 -std=c++11
EXT = C:/ext/
GLFW = $(EXT)glfw-3.2.1/
GLEW = $(EXT)glew-2.0.0/
SIMPLEX = $(EXT)simplex-1.0/
GLM = $(EXT)glm-0.9.8.4/
FT = $(EXT)freetype-2.8/

INC = -I $(GLFW)include -I $(GLEW)include -I $(SIMPLEX)include -I $(GLM)include -I $(FT)include -I include -I src
LIB =   $(GLEW)lib/glew.o -L $(GLFW)lib $(SIMPLEX)lib/simplex.o $(FT)lib/libfreetype.a
HEAD = include/

$(BIN)game: $(BUILD)game.o $(BUILD)renderer.o $(BUILD)input.o $(BUILD)terrain.o $(BUILD)terrainmesher.o $(BUILD)log.o;
	$(CC) $(BUILD)game.o $(BUILD)log.o $(BUILD)renderer.o $(BUILD)input.o $(BUILD)terrain.o $(BUILD)terrainmesher.o $(LIB) -lglfw3 -lgdi32 -lopengl32 $(CFLAGS) -o $(BIN)game.exe 

$(BUILD)game.o: $(SRC)game.cpp $(SRC)other/defineterrain.hpp
	$(CC) -c $(SRC)game.cpp $(INC) $(CFLAGS) -o $(BUILD)game.o

$(BUILD)renderer.o: $(SRC)renderer/renderer.cpp $(BUILD)res.h $(SRC)renderer/textrenderer.hpp
	$(CC) -c $(SRC)renderer/renderer.cpp $(INC) $(CFLAGS) -o $(BUILD)renderer.o

$(BUILD)input.o: $(SRC)input/input.cpp
	$(CC) -c $(SRC)input/input.cpp $(INC) $(CFLAGS) -o $(BUILD)input.o

$(BUILD)terrain.o: $(SRC)terrain/terrain.cpp
	$(CC) -c $(SRC)terrain/terrain.cpp $(INC) $(CFLAGS) -o $(BUILD)terrain.o

$(BUILD)terrainmesher.o: $(SRC)mesher/terrainmesher.cpp
	$(CC) -c $(SRC)mesher/terrainmesher.cpp $(INC) -fopenmp $(CFLAGS) -o $(BUILD)terrainmesher.o

$(BUILD)log.o: $(SRC)log/log.cpp
	$(CC) -c $(SRC)log/log.cpp $(INC) $(CFLAGS) -o $(BUILD)log.o

$(BUILD)entitysys.o: $(SRC)entitysys/entitysys.cpp
	$(CC) -c $(SRC)entitysys/entitysys.cpp $(INC) $(CFLAGS) -o $(BUILD)entitysys.o

$(BUILD)res.h: $(SRC)renderer/shader.glsl
	node scripts/res.js

clean:
	del build\*
	del bin\game.exe

test: $(BIN)game
	bin/game

phystest:
	$(CC) $(SRC)tests/aabbtest.cpp $(INC) $(CFLAGS) -o $(BUILD)phystest.exe
	./build/phystest
test_entitysys:
	$(CC) $(SRC)tests/test_entitysys.cpp $(INC) $(CFLAGS) -o $(BUILD)test_entitysys.exe
	./build/test_entitysys