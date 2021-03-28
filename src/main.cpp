// Trent Julich ~ 23 March 2021

#include "emu.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
  Emu emu;
  
  Display &emu_display = emu.get_display();

  emu.PrintDisplay();
  emu.get_display().SetPixel(0, 0, 1);
  emu.PrintDisplay();

  std::cout << std::endl << std::endl;

  emu_display.Print();

  

  return 0;
}