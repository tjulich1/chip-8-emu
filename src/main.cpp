// Trent Julich ~ 23 March 2021

#include "emu.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
  Emu emu;

  // emu.LoadInstruction(0, std::bitset<16>("0000000011100000"));
  // emu.LoadInstruction(2, std::bitset<16>("1111000011001111"));

  // emu.PrintMemory(0);
  // emu.PrintMemory(1);

  // emu.PrintMemory(2);
  // emu.PrintMemory(3);
  
  // emu.SetProgramCounter(0);

  // emu.Step();

  // emu.PrintDisplay();
  
  emu.PrintMemory(0x50);
  emu.PrintMemory(0x51);
  emu.PrintMemory(0x52);
  emu.PrintMemory(0x53);
  emu.PrintMemory(0x54);

  return 0;
}