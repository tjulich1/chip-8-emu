#include "emu.hpp"

#include <bitset>

void Emu::DecodeRegisterArithmetic(std::bitset<16> p_instruction) {
  std::bitset<16> second_byte_mask = std::bitset<16>(0x0F00);
  std::bitset<16> third_byte_mask = std::bitset<16>(0x00F0);
  std::bitset<16> fourth_byte_mask = std::bitset<16>(0x000F);

  int first_register = ((p_instruction & third_byte_mask) >> 4).to_ulong();
  int second_register = ((p_instruction & second_byte_mask) >> 8).to_ulong();

  switch((p_instruction & fourth_byte_mask).to_ulong()) {
    case 0: {
      StoreRegisterXInY(first_register, second_register);
      break;
    }
    case 1: {
      OrRegisters(first_register, second_register);
      break;
    }
    case 2: {
      AndRegisters(first_register, second_register);
      break;
    }
    case 3: {
      XorRegisters(first_register, second_register);
      break;
    }
    case 4: {
      AddRegisters(first_register, second_register);
      break;
    }
    case 5: { 
      SubtractRegisters(second_register, first_register, second_register);
      break;
    }
    case 6: { 
      ShiftRegisterRight(first_register, second_register);
      break;
    }
    case 7: {
      SubtractRegisters(first_register, second_register, second_register);
      break;
    }
    case 0xE: {
      ShiftRegisterLeft(first_register, second_register); 
      break;
    }
    default: {
      std::cout << "Instruction unknown " << p_instruction << std::endl;
      std::cout << "Last byte: " << std::hex << (p_instruction & fourth_byte_mask).to_string() 
        << std::endl;
    }
  }
}

void Emu::StoreRegisterXInY(int p_src_register, int p_dest_register) {
  set_register(p_dest_register, get_register(p_src_register));
}

void Emu::OrRegisters(int p_first_register, int p_second_register) {
  int first_value = get_register(p_first_register);
  int second_value = get_register(p_second_register);
  variable_registers_[p_second_register].Write(first_value | second_value);
}

void Emu::AndRegisters(int p_first_register, int p_second_register) {
  int first_value = get_register(p_first_register);
  int second_value = get_register(p_second_register);
  set_register(p_second_register, first_value & second_value);
}

void Emu::XorRegisters(int p_first_register, int p_second_register) {
  int first_value = get_register(p_first_register);
  int second_value = get_register(p_second_register);
  set_register(p_second_register, first_value ^ second_value);
}

void Emu::AddRegisters(int p_first_register, int p_second_register) {
  int first_value = get_register(p_first_register);
  int second_value = get_register(p_second_register);

  if (first_value + second_value > 0xFF) {
    set_register(0xF, 1);
  } else {
    set_register(0xF, 0);
  }

  set_register(p_second_register, first_value + second_value);
}

void Emu::SubtractRegisters(int p_first_register, int p_second_register, int p_destination_register) {
  int first_value = get_register(p_first_register);
  int second_value = get_register(p_second_register);

  set_register(0xF, 1);
  
  if (second_value > first_value) {
    set_register(0xF, 0);
  }

  if (p_destination_register != 0xF) {
    set_register(p_destination_register, first_value - second_value);
  }
}

void Emu::ShiftRegisterRight(int p_source_register, int p_destination_register) {
  int value = get_register(p_source_register);
  
  // Mask and write the least significant bit to VF
  set_register(0xF, value & 1);

  // Shift value to right one bit and store in destination register.
  set_register(p_destination_register, value >> 1);
}

void Emu::ShiftRegisterLeft(int p_source_register, int p_destination_register) {
  int value = get_register(p_source_register);
  
  // Write the least significant bit before shift to VF.
  set_register(0xF, value & 1);

  // Shift value to left 1 bit and store to destination register.
  set_register(p_destination_register, value << 1);
}