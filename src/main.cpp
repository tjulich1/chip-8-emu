// Trent Julich ~ 23 March 2021

#include "emu.hpp"

#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* argv[]) {
  Emu emu;
  emu.set_program_counter(0x200);
  std::ifstream input("C:\\Users\\tjulich\\Desktop\\chip-8_roms\\ibm_logo.ch8");
  std::vector<char> bytes(
    (std::istreambuf_iterator<char>(input)),
    (std::istreambuf_iterator<char>())
  );

  input.close();

  std::cout << "Size: " << bytes.size() << std::endl;

  for (int i = 0; i < bytes.size(); i += 2) {
    char first_byte = bytes[i];
    char second_byte = bytes[i+1];
    std::bitset<8> first_bits((int)first_byte);
    std::bitset<8> second_bits((int)second_byte);
    std::bitset<16> instruction(first_bits.to_string() + second_bits.to_string());
    emu.LoadInstruction(0x200 + i, instruction);
  }

  for (int i = 0; i < bytes.size()/2; i++) {
    emu.Step();
    emu.PrintDisplay(); 
  }

  return 0;
}