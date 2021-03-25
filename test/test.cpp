// Trent Julich ~ 23 March 2021

//#define CATCH_CONFIG_MAIN
//#include "catch.hpp"

//#include "register_test.cpp"

#include <array>
#include <iostream>

#include "../src/emu.hpp"

int main(int argc, char* args[]) {
  Emu test_emu;
  std::array<bool, 8> test_instruction{
    0, 1, 0, 1,
    0, 1, 0, 1
  };
  test_emu.LoadInstruction(100, test_instruction);
  test_emu.PrintMemory(100);
  return 0;
}