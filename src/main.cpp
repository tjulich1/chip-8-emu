// Trent Julich ~ 23 March 2021

#include "emu.hpp"
#include "keyboard_input.hpp"
#include "emulator_panel.hpp"
#include "var_register_panel.hpp"

#include <fstream>
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

const int PIXEL_SIZE = 10;

const int REGISTER_WIDTH = 100;
const int EMULATOR_WIDTH = 64*PIXEL_SIZE;
const int EMULATOR_HEIGHT = 32*PIXEL_SIZE;

const int WINDOW_HEIGHT = EMULATOR_HEIGHT;
const int WINDOW_WIDTH = EMULATOR_WIDTH + REGISTER_WIDTH; 

static Uint32 sound_timer_callback(Uint32 p_interval, void* p_param);
static Uint32 delay_timer_callback(Uint32 p_interval, void* p_param);
static Uint32 step_timer_callback(Uint32 p_interval, void* p_param);
bool init_sdl();

/**
 * Looks for input file flag in argv, and if found, stores the input file path in the input_file 
 * string.
 */
bool parse_args(int p_argc, char* p_argv[], std::string& p_input_file) {
  bool rom_found = true;

  // Check for input file flag ("-i") from command line
  for (int i = 0; i < p_argc; i++) {
    if ((std::string(p_argv[i]) == "-i") && (i + 1 < p_argc)) {
      p_input_file = p_argv[i+1];
      break;
    }
  }

  if (p_input_file.empty()) {
    std::cout << "chip-8 -i <filename>" << std::endl;
    rom_found = false;
  }

  return rom_found;
}

/**
 * Takes the given ifstream and returns the length in bytes.
 */
int find_rom_length(std::ifstream& p_rom_buffer) {
  // Set cursor to end of file
  p_rom_buffer.seekg(0, std::ios::end);

  // Get the current cursor position which tells length of file (number of bytes)
  int length = p_rom_buffer.tellg();

  // Put cursor back at beginning of file
  p_rom_buffer.seekg(0, std::ios::beg);

  return length;
}

/**
 * Loads each byte from p_rom_buffer into the virtual memory of p_emu.
 */
void load_rom(Emu* p_emu, std::ifstream& p_rom_buffer, int p_rom_length) {
  int address_start = 0x200;

  for (int i = 0; i < p_rom_length; i++) {
    p_emu->set_memory(address_start + i, p_rom_buffer.get());
  }

  p_rom_buffer.close();
}

/**
 * Called to start emulation of the selected rom. Also registers emulator for timing events for 
 * the delay counter, sound timer, and main execution loop timing. 
 */
void start_emulator(Emu* p_emu, SDL_Renderer* p_renderer) {
  SDL_AddTimer(1000 / 60, delay_timer_callback, p_emu);
  SDL_AddTimer(1000 / 60, sound_timer_callback, p_emu);
  SDL_AddTimer(2, step_timer_callback, p_emu);

  std::vector<Panel*> components;
  
  components.emplace_back(new EmulatorPanel(0, 0, EMULATOR_WIDTH, EMULATOR_HEIGHT, p_emu));
  components.emplace_back(new VariableRegisterPanel(EMULATOR_WIDTH, 0, REGISTER_WIDTH, WINDOW_HEIGHT, p_emu->get_variable_registers()));

  bool running = true;

  while (running) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT: {
          running = false;
          break;
        }
        case SDL_USEREVENT: {
          p_emu->HandleUserEvent(e);
          SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
          SDL_RenderClear(p_renderer);
          for (int i = 0; i < components.size(); i++) {
            components[i]->Render(p_renderer);
          }
          p_emu->Render();
          SDL_RenderPresent(p_renderer);
          break;
        }
        case SDL_KEYDOWN: {
          p_emu->KeyDown(e.key.keysym.scancode);
          break;
        }
        case SDL_KEYUP: {
          p_emu->KeyUp(e.key.keysym.scancode);
          break;
        }
      }
    }
  }
}

int main(int argc, char* argv[]) {

  std::string input_file;

  // If input file was provided
  if (parse_args(argc, argv, input_file)) {
    // Attempt to open file stream.
    std::ifstream input(input_file, std::ifstream::binary);

    int length = find_rom_length(input);

    if (length > 0) {
      if (init_sdl()) {
        SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, 
          SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

        if (window) {
          SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

          if (renderer) {
            Emu* emu = new Emu(renderer);
          
            load_rom(emu, input, length);
            start_emulator(emu, renderer);
      
            delete emu;
          } else {
            std::cout << "Unable to create renderer: " << SDL_GetError() << std::endl;
          }
          SDL_DestroyRenderer(renderer);  
        } else {
          std::cout << "Unable to create window: " << SDL_GetError() << std::endl;  
        }
        SDL_DestroyWindow(window);
      }
      SDL_Quit();
    } else {
      std::cout << "chip-8 -i <filename>" << std::endl;
    }
  }
  return 0;
}

bool init_sdl() {
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    success = false;
    std::cout << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
  }

  if (TTF_Init() == -1) {
    success = false;
    std::cout << "Unable to initialize SDL_ttf: " << TTF_GetError() << std::endl;
  }

  return success;
}

Uint32 delay_timer_callback(Uint32 p_interval, void* param) {
  // Cast the void pointer to a pointer to the emulator we are wanting to call DelayTick on.
  Emu* emu = static_cast<Emu*>(param);
  emu->DelayTick();
  return p_interval;
}

Uint32 sound_timer_callback(Uint32 p_interval, void* param) {
  // Cast the void pointer to a pointer to the emulator we are wanting to call DelayTick on.
  Emu* emu = static_cast<Emu*>(param);
  emu->SoundTick();
  return p_interval;
}

Uint32 step_timer_callback(Uint32 p_interval, void* p_param) {
  // Cast void pointer to a pointer to the emulator we are wanting to call Step on.
  Emu* emu = static_cast<Emu*>(p_param);
  emu->StepTick();
  return p_interval;
}