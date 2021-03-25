#include "emu.hpp"

#include <iostream>

void Emu::LoadInstruction(int p_address, std::array<bool, 8> p_instruction) {
  memory_.Write(p_address, p_instruction);
}

void Emu::SetProgramCounter(std::array<bool, 16> p_pc_value) {
  program_counter_.Write(p_pc_value);
}

void Emu::Step() { }

void Emu::PrintMemory(int p_address) {
  std::cout << "Mem[" << p_address << "]: ";
  std::array<bool, 8> read_value = memory_.Read(p_address);
  for (int i = 0; i < read_value.size(); i++) {
    std::cout << read_value[i];
    if (i % 4 == 3) {
      std::cout << ' ';
    }
  }
  std::cout << std::endl;
}