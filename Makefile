# -*- MakeFile -*-
SRC= src/
BIN= bin/
BUILD= build/
CC= g++
CFLAGS= -O3 -ftree-vectorize -msse3 -static -fopenmp -Wall -static-libgcc -static-libstdc++ -std=c++11 -m32
#-Wl,-subsystem,windows -fopt-info-vec-missed 
EXT = lib/
GLFW = $(EXT)glfw-3.2.1/
GLEW = $(EXT)glew-2.0.0/
SIMPLEX = $(EXT)simplex-1.0/
GLM = $(EXT)glm-0.9.8.4/
FT = $(EXT)freetype-2.8/

INC = -I $(GLFW)include -I $(GLEW)include -I $(SIMPLEX)include -I $(GLM)include -I $(FT)include -I include -I src
LIB =   $(GLEW)lib/glew.o -L $(GLFW)lib $(SIMPLEX)lib/simplex.o $(FT)lib/libfreetype.a
HEAD = include/

OBJECTS = \
$(SRC)game.o \
$(BUILD)renderer.o \
$(SRC)input/input.o \
$(SRC)terrain/terrain.o \
$(SRC)mesher/terrainmesher.o \
$(SRC)log/log.o \
$(SRC)entity/entity.o \
$(SRC)entity/entityfactory.o \
$(SRC)system/localcontrol.o \
$(SRC)system/physics.o \
$(SRC)system/camera.o \
$(SRC)other/util.o \


$(BIN)game: $(OBJECTS)
	@echo Linking $@
	@$(CC) $^ $(LIB) -lglfw3 -lgdi32 -lopengl32 $(CFLAGS) -o $(BIN)game.exe 

$(BUILD)renderer.o: $(SRC)renderer/renderer.cpp $(BUILD)res.h $(SRC)renderer/textrenderer.hpp
	@echo Building $@
	@$(CC) -c $< $(INC) $(CFLAGS) -o $@

$(SRC)%.o : $(SRC)%.cpp
	@echo Building $@
	@$(CC) -c $< $(INC) $(CFLAGS) -o $@

$(BUILD)res.h: $(SRC)renderer/shader.glsl
	@node scripts/res.js

run: bin/game
	@bin/game

.PHONY: clean
clean:
	@del build\* /Q
	@del src\*.o /Q /S
	@del bin\game.exe /Q

git:
	git add *
	git commit
	git push