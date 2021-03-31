// Trent Julich ~ 23 March 2021

#include "emu.hpp"

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

    std::cout << "Renderer = " << renderer << std::endl;

    Emu emu = Emu(renderer);
    emu.set_program_counter(0x200);
    emu.set_index_register(0);

    std::vector<int> random_bytes;
    for (int i = 0; i <= 0xF; i++) {
      random_bytes.emplace_back(rand() % 0xFF);
      emu.set_memory(i, random_bytes[i]);
    }

    emu.LoadInstruction(0x200, std::bitset<16>(0xFF65));
    emu.Step();
    emu.PrintRegisters();

    // std::ifstream input("C:\\Users\\tjulich\\Desktop\\chip-8_roms\\ibm_logo.ch8");
    // std::vector<char> bytes(
    //   (std::istreambuf_iterator<char>(input)),
    //   (std::istreambuf_iterator<char>())
    // );

    // input.close();

    // for (int i = 0; i < bytes.size(); i += 2) {
    //   char first_byte = bytes[i];
    //   char second_byte = bytes[i+1];
    //   std::bitset<8> first_bits((int)first_byte);
    //   std::bitset<8> second_bits((int)second_byte);
    //   std::bitset<16> instruction(first_bits.to_string() + second_bits.to_string());
    //   emu.LoadInstruction(0x200 + i, instruction);
    // }
    
    // emu.Start();

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