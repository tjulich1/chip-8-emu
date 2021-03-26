// Trent Julich ~ 23 March 2021

#include "emu.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
  Emu emu;
  emu.LoadInstruction(0, std::bitset<16>("0000000011110000"));
  emu.LoadInstruction(2, std::bitset<16>("1111000011000011"));

  emu.PrintMemory(0);
  emu.PrintMemory(1);
  emu.PrintMemory(2);

  emu.SetProgramCounter(0);

  return 0;
}