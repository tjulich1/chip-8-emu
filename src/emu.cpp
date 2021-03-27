#include "emu.hpp"

#include <iostream>

Emu::Emu() {
  InitializeFonts();
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
  std::cout << "Mem[" << std::dec << p_address << "]: ";
  std::bitset<8> read_value = memory_.Read(p_address);
  std::bitset<4> first_nibble(read_value.to_string().substr(0, 4));
  std::bitset<4> second_nibble(read_value.to_string().substr(4, 4));  
  std::cout << std::hex << first_nibble.to_ulong() << second_nibble.to_ulong() << std::endl;
}

void Emu::Step() {

  std::cout << "Instruction at PC " << program_counter_ << ": ";

  std::bitset<16> current_instruction = Fetch();
  std::cout << std::hex << std::bitset<4>(current_instruction.to_string().substr(0, 4)).to_ulong();
  std::cout << std::hex << std::bitset<4>(current_instruction.to_string().substr(4, 4)).to_ulong();
  std::cout << std::hex << std::bitset<4>(current_instruction.to_string().substr(8, 4)).to_ulong();
  std::cout << std::hex << std::bitset<4>(current_instruction.to_string().substr(12, 4)).to_ulong();
  std::cout << std::endl;
}

std::bitset<16> Emu::Fetch() {
  
  std::bitset<8> first_byte = memory_.Read(program_counter_);
  std::bitset<8> second_byte = memory_.Read(program_counter_ + 1);

  program_counter_ += 2;

  std::bitset<16> current_instruction(first_byte.to_string() + second_byte.to_string());
  return current_instruction;
}

void Emu::PrintDisplay() {
  main_display_.Print();
}

void Emu::PrintRegisters() {
  for (int i = 0; i < variable_registers_.size(); i++) {
    std::cout << "V" << std::hex << i << ": " << variable_registers_[i].Read().to_ulong() 
      << std::endl;
  }
}

std::bitset<8> Emu::GetRegister(int p_register) {
  std::bitset<8> value;
  if (p_register > -1 && p_register < 16) {
    value = variable_registers_[p_register].Read();
  }
  return value;
}

void Emu::InitializeFonts() {
  std::vector<int> font_bytes{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F  
  };
  for (int i = 0; i < font_bytes.size(); i++) {
    std::bitset<8> current_byte(font_bytes[i]);
    memory_.Write(0x50 + i, current_byte);
  }
}

void Emu::ClearScreen() {
  main_display_.Clear();
}

void Emu::Jump(int p_new_program_counter) {
  program_counter_ = p_new_program_counter;
}

void Emu::SetRegister(int p_register, int p_new_value) {
  if (p_register > -1 && p_register < 16) {
    variable_registers_[p_register].Write(std::bitset<8>(p_new_value));
  }
}

void Emu::AddValToRegister(int p_register, int p_val) {
  if (p_register > -1 && p_register < 16) {
    int new_value = variable_registers_[p_register].Read().to_ulong() + p_val;
    variable_registers_[p_register].Write(std::bitset<8>(new_value));
  }
} 

void Emu::SetIndexRegister(std::bitset<16> p_new_index_register) {
  index_register_.Write(p_new_index_register);
}

void Emu::Display(int p_rows, int p_x_coord, int p_y_coord) {
  
}