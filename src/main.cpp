// Trent Julich ~ 23 March 2021

#include "emu.hpp"
#include "keyboard_input.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <SDL.h>

const int PIXEL_SIZE = 10;

bool init_sdl();

int main(int argc, char* argv[]) {

  if (init_sdl()) {
    SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      64*PIXEL_SIZE, 32*PIXEL_SIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Emu* emu = new Emu(renderer);

    std::ifstream input("../roms/tetris.rom", std::ifstream::binary);

    input.seekg(0, std::ios::end);
    int length = input.tellg();
    input.seekg(0, std::ios::beg);

    std::cout << "Size of input buffer: " << length << std::endl;

    int address_start = 0x200;

    for (int i = 0; i < length; i++) {
      emu->set_memory(address_start + i, input.get());
    }

    input.close();

    emu->Start();

    delete emu;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
  }
  SDL_Quit();
  return 0;
}

bool init_sdl() {
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    success = false;
    std::cout << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
  }
  return success;
}