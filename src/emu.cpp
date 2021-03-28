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

  // Write second byte to memory
  for (int i = 0; i < 8; i++) {
    byte[i] = p_instruction[i];
  }
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
  Decode(current_instruction);
}

void Emu::Decode(std::bitset<16> p_instruction) {
  // Grab far left nibble
  std::bitset<4> first(p_instruction.to_string().substr(0, 4));
  // Grab second
  std::bitset<4> second(p_instruction.to_string().substr(4, 4));
  // Grab third
  std::bitset<4> third(p_instruction.to_string().substr(8, 4));
  // Grab far right nibble
  std::bitset<4> fourth(p_instruction.to_string().substr(12, 4));
  std::cout << std::hex << first.to_ulong() << second.to_ulong() << third.to_ulong() << fourth.to_ulong() << std::endl;

  if (p_instruction.to_ulong() == 0x00e0) {
    ClearScreen();
  }
  switch(first.to_ulong()) {
    case 0x1: {
      int new_program_counter = (p_instruction & std::bitset<16>(0x0FFF)).to_ullong();
      Jump(new_program_counter);
      break;
    }
    case 0xD: {
      int rows = fourth.to_ulong();
      int x_coord = variable_registers_[second.to_ulong()].Read().to_ulong();
      int y_coord = variable_registers_[third.to_ulong()].Read().to_ulong();
      Display(rows, x_coord, y_coord);
      break;
    }
    case 0x7: {
      int register_number = second.to_ulong();
      int value_to_add = std::bitset<8>(p_instruction.to_string().substr(8, 8)).to_ulong();
      AddValToRegister(register_number, value_to_add);
      break;
    }
    case 0x6: {
      int register_number = second.to_ulong();
      int new_value = std::bitset<8>(p_instruction.to_string().substr(8, 8)).to_ulong();
      SetRegister(register_number, new_value);
      break;
    }
    case 0xA: {
      SetIndexRegister(std::bitset<16>(p_instruction.to_string().substr(4, 12)).to_ulong());
      break;
    }
  }
}

std::bitset<16> Emu::Fetch() {
  
  std::bitset<8> first_byte = memory_.Read(program_counter_);
  std::bitset<8> second_byte = memory_.Read(program_counter_ + 1);

  program_counter_ += 2;

  std::bitset<16> current_instruction(first_byte.to_string() + second_byte.to_string());
  return current_instruction;
}

int Emu::GetProgramCounter() {
  return program_counter_;
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

int Emu::GetRegister(int p_register) {
  std::bitset<8> value;
  if (p_register > -1 && p_register < 16) {
    value = variable_registers_[p_register].Read();
  }
  return value.to_ulong();
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

void Emu::Display(int p_rows, int p_x_coord, int p_y_coord) {
  std::cout << "ROWS: " << p_rows << ", X: " << p_x_coord << ", Y:" << p_y_coord << std::endl;

  // For p_rows:
  for (int i = 0; i < p_rows; i++) { 
  //   grab byte of sprite from address in index_register_
    std::bitset<8> sprite = memory_.Read(index_register_.Read().to_ulong());
    //   For bit in current byte:
    for (int j = 7; j > -1; j--) {
      //     if current_bit = 1 && display[x][y] == 1
      if (main_display_.GetPixel(p_y_coord, p_x_coord) && sprite[j]) {
        //       VF = 1  
        variable_registers_[0xF].Write(std::bitset<8>(1)); 
      } else if (sprite[j]) {
        main_display_.SetPixel(p_y_coord, p_x_coord, 1);
      } 
      if (p_x_coord + (7-j) >= main_display_.get_pixel_width()) {
        break;
      }
    }
    if (p_y_coord + i >= main_display_.get_pixel_height()) {
      break;
    }
  }
}

Display& Emu::get_display() {
  return main_display_;
}

std::bitset<16> Emu::GetIndexRegister() {
  return index_register_.Read();
}

void Emu::SetIndexRegister(int p_new_value) {
  index_register_.Write(std::bitset<16>(p_new_value));
}