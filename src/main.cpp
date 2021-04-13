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

    // emu->set_register(0, 1);
    // emu->LoadInstruction(0x202, 0xE0A1);

    // emu->Step();
    // SDL_Delay(1000);
    // emu->Step();

    std::vector<int> random_bytes;
    int MAX_BYTE = 0xFF;

    for (int i = 0; i <= 0xF; i++) {
      random_bytes.emplace_back(rand() % MAX_BYTE);
      emu->set_memory(i, random_bytes[i]);
    }

    std::ifstream input("../roms/Maze.ch8");
    std::vector<char> bytes(
      (std::istreambuf_iterator<char>(input)),
      (std::istreambuf_iterator<char>())
    );

    input.close();

    for (int i = 0; i < bytes.size(); i += 2) {
      char first_byte = bytes[i];
      char second_byte = bytes[i+1];
      std::bitset<8> first_bits((int)first_byte);
      std::bitset<8> second_bits((int)second_byte);
      std::bitset<16> instruction(first_bits.to_string() + second_bits.to_string());
      emu->LoadInstruction(0x200 + i, instruction);
    }

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