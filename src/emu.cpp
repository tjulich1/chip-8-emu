#include "emu.hpp"

#include <iostream>

int BitsToInt(std::array<bool, 8> p_bits) {
  int value;

}

void Emu::LoadInstruction(int p_address, std::bitset<16> p_instruction) {
  
  // p_instruction = [15, ... , 0]
  // first_byte = [15, ... , 8]
  // second_byte = [7, ... , 0]
  // mem[p_address] = first_byte
  // mem[p_address + 1] = second_byte

  std::bitset<8> byte;

  // Write first byte to memory
  for (int i = 0; i < 8; i++) {
    byte[i] = p_instruction[i + 8];
  }
  memory_.Write(p_address, byte);

  std::cout << "Wrote " << std::hex << byte.to_ulong();

  // Write second byte to memory
  for (int i = 0; i < 8; i++) {
    byte[i] = p_instruction[i];
  }

  std::cout << std::hex << byte.to_ulong() << " to " << p_instruction << std::endl;

  memory_.Write(p_address + 1, byte);
}

void Emu::SetProgramCounter(int p_pc_value) {
  program_counter_ = p_pc_value;
}

void Emu::PrintMemory(int p_address) {
  std::cout << "Mem[" << p_address << "]: ";
  std::bitset<8> read_value = memory_.Read(p_address);
  std::bitset<4> first_nibble(read_value.to_string().substr(0, 4));
  std::bitset<4> second_nibble(read_value.to_string().substr(4, 4));  
  std::cout << std::hex << first_nibble.to_ulong() << second_nibble.to_ulong() << std::endl;
}

void Emu::Step() {

}

std::bitset<16> Emu::Fetch() {
  
}

void Emu::PrintDisplay() {
  main_display_.Print();
}