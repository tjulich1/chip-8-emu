OBJS = src/*.cpp

INCLUDE_PATH = -Iinclude/SDL2

LIB_PATH = -Llib/SDL2

OPTIONS = -g

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

OBJ_NAME = build/chip-8

all: $(OBJS)
	g++ $(OBJS) $(OPTIONS) $(INCLUDE_PATH) $(LIB_PATH) $(LINKER_FLAGS) -o $(OBJ_NAME)
