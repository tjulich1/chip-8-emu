// Trent Julich ~ 23 March 2021

#include "emu.hpp"
#include "keyboard_input.hpp"

#include <fstream>
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

bool init_sdl();

int main(int argc, char* argv[]) {

  std::string input_file;

  // Check for input file flag ("-i") from command line
  for (int i = 0; i < argc; i++) {
    if ((std::string(argv[i]) == "-i") && (i + 1 < argc)) {
      input_file = argv[i+1];
    }
  }

  // If input file was provided
  if (!input_file.empty()) {
    // Attempt to open file stream.
    std::ifstream input(input_file, std::ifstream::binary);

    // Set cursor to end of file
    input.seekg(0, std::ios::end);

    // Get the current cursor position which tells length of file (number of bytes)
    int length = input.tellg();

    // Put cursor back at beginning of file
    input.seekg(0, std::ios::beg);

    if (length > 0) {
      if (init_sdl()) {
        const int PIXEL_SIZE = 10;
        SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, 
          SDL_WINDOWPOS_UNDEFINED, 64*PIXEL_SIZE, 32*PIXEL_SIZE, SDL_WINDOW_SHOWN);

        if (window) {
          SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

          if (renderer) {
            Emu* emu = new Emu(renderer);
            int address_start = 0x200;

            for (int i = 0; i < length; i++) {
              emu->set_memory(address_start + i, input.get());
            }

            input.close();
            emu->Start();
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
  } else {
    std::cout << "chip-8 -i <filename>" << std::endl;
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