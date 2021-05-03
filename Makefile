OBJS = objects/font_atlas.o objects/display.o objects/ram.o objects/emu.o objects/emu_arith.o 
OBJS += objects/emu_reg.o objects/keyboard.o objects/emu_panel.o objects/reg_panel.o 
OBJS += objects/main.o 

INCLUDE_PATH = -Iinclude/SDL2

LIB_PATH = -Llib/SDL2

OPTIONS = -g

TEST_LINKER_FLAGS = -lmingw32 -lSDL2 -lSDL_ttf

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

OBJ_NAME = chip-8

OBJ_DIR = objects

all: $(OBJ_NAME)

$(OBJ_NAME): $(OBJS)
	g++ $(OBJS) $(OPTIONS) $(LIB_PATH) $(LINKER_FLAGS)  -o build/$(OBJ_NAME)

$(OBJ_DIR)/main.o: src/main.cpp
	g++ -c src/main.cpp $(INCLUDE_PATH)  -o $(OBJ_DIR)/main.o 

$(OBJ_DIR)/display.o: src/display.cpp
	g++ -c src/display.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/display.o
	
$(OBJ_DIR)/ram.o: src/ram.cpp
	g++ -c src/ram.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/ram.o

$(OBJ_DIR)/emu.o: src/emu.cpp
	g++ -c src/emu.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/emu.o

$(OBJ_DIR)/emu_reg.o: src/emu_register_ops.cpp
	g++ -c src/emu_register_ops.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/emu_reg.o

$(OBJ_DIR)/objects/emu_arith.o: src/emu_arithmetic.cpp 
	g++ -c src/emu_arithmetic.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/emu_arith.o

$(OBJ_DIR)/keyboard.o: src/keyboard_input.cpp
	g++ -c src/keyboard_input.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/keyboard.o 

$(OBJ_DIR)/emu_panel.o: src/emulator_panel.cpp
	g++ -c src/emulator_panel.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/emu_panel.o

$(OBJ_DIR)/reg_panel.o: src/var_register_panel.cpp
	g++ -c src/var_register_panel.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/reg_panel.o

$(OBJ_DIR)/font_atlas.o: src/font_atlas.cpp
	g++ -c src/font_atlas.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/font_atlas.o

test: $(OBJ_DIR)/*.o
	g++ -o build/test $(OBJ_DIR)/*.o $(LIB_PATH) $(TEST_LINKER_FLAGS) 

$(OBJ_DIR)/test.o: test/test.cpp
	g++ -c test/test.cpp $(INCLUDE_PATH) -o $(OBJ_DIR)/test.o 

clean: 
	del build\chip-8.exe