# -*- MakeFile -*-
SRC= src/
BIN= bin/
BUILD= build/
CC= g++
CFLAGS= \
	-ggdb \
	-O3 -ftree-vectorize -msse3 -m32 -Wall \
	-static -fopenmp -static-libgcc -static-libstdc++ -std=c++11 \
	-Wl,-subsystem,windows

#-Wl,-subsystem,windows -fopt-info-vec-missed 
EXT = lib/
GLFW = $(EXT)glfw-3.2.1/
GLEW = $(EXT)glew-2.0.0/
SIMPLEX = $(EXT)simplex-1.0/
GLM = $(EXT)glm-0.9.8.4/
FT = $(EXT)freetype-2.8/

INC = \
	-I $(GLFW)include \
	-I $(GLEW)include \
	-I $(SIMPLEX)include \
	-I $(GLM)include \
	-I $(FT)include \
	-I include \
	-I src

LIB = \
	$(GLEW)lib/glew.o \
	-L $(GLFW)lib \
	$(SIMPLEX)lib/simplex.o \
	$(FT)lib/libfreetype.a

OBJECTS = \
	$(SRC)game.o \
	$(BUILD)core.o \
	$(SRC)graphics/voxel.o \
	$(SRC)graphics/cull.o \
	$(SRC)graphics/text.o \
	$(SRC)input/input.o \
	$(SRC)terrain/terrain.o \
	$(SRC)mesher/terrainmesher.o \
	$(SRC)entity/entity.o \
	$(SRC)entity/entityfactory.o \
	$(SRC)system/localinput.o \
	$(SRC)system/physics.o \
	$(SRC)system/camera.o \
	$(SRC)system/debug.o \
	$(SRC)physics/saabb.o \
	$(SRC)event/event.o \
	$(SRC)core/memory.o \
	$(SRC)core/util.o \
	$(SRC)core/log.o \


$(BIN)game: $(OBJECTS)
	@echo Linking $@
	@$(CC) $^ $(LIB) -lglfw3 -lgdi32 -lopengl32 $(CFLAGS) -o $(BIN)game.exe 

$(BUILD)core.o: \
$(SRC)graphics/core.cpp $(BUILD)res.h
	@echo Building $@
	@$(CC) -c $< $(INC) $(CFLAGS) -o $@

$(SRC)%.o : $(SRC)%.cpp
	@echo Building $@
	@$(CC) -c $< $(INC) $(CFLAGS) -o $@

$(BUILD)res.h: $(SRC)graphics/shader.glsl
	@node scripts/res.js

run: bin/game
	@bin/game

.PHONY: clean git

clean:
	@del build\* /Q
	@del src\*.o /Q /S
	@del bin\game.exe /Q

git:
	git add *
	git commit
	git push