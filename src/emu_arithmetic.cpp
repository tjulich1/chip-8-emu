#include "emu.hpp"

#include <bitset>

void Emu::DecodeRegisterArithmetic(std::bitset<16> p_instruction) {
  std::bitset<16> second_byte_mask = std::bitset<16>(0x0F00);
  std::bitset<16> third_byte_mask = std::bitset<16>(0x00F0);
  std::bitset<16> fourth_byte_mask = std::bitset<16>(0x000F);

  int first_register = ((p_instruction & third_byte_mask) >> 4).to_ulong();
  int second_register = ((p_instruction & second_byte_mask) >> 8).to_ulong();

  int first_value = get_register(first_register);
  int second_value = get_register(second_register);

  int last_nibble = (p_instruction & fourth_byte_mask).to_ulong();

  switch(last_nibble) {
    case 0: {
      set_register(second_register, first_value);
      break;
    }
    case 1: {
      set_register(second_register, first_value | second_value);
      break;
    }
    case 2: {
      set_register(second_register, first_value & second_value);
      break;
    }
    case 3: {
      set_register(second_register, first_value ^ second_value);
      break;
    }
    case 4: {
      set_register(second_register, first_value + second_value);
      if (first_value + second_value > 0xFF) {
        set_register(0xF, 1);
      } else {
        set_register(0xF, 0);
      }
      break;
    }  
    case 5: {
      if (second_value < first_value) {
        set_register(0xF, 0);
      } else {
        set_register(0xF, 1);
      }
      set_register(second_register, second_value - first_value);
      break;
    }
    case 6: { 
      set_register(0xF, first_value & 1);
      set_register(second_register, first_value >> 1);
      break;
    }
    case 7: {
      if (first_value < second_value) {
        set_register(0xF, 0);
      } else {
        set_register(0xF, 1);
      }
      set_register(second_register, first_value - second_value);
      break;
    }
    case 0xE: {
      set_register(0xF, first_value & 1); 
      set_register(second_register, first_value << 1);
      break;
    }
    default: {
      std::cout << "Instruction unknown " << p_instruction << std::endl;
      std::cout << "Last byte: " << std::hex << (p_instruction & fourth_byte_mask).to_string() 
        << std::endl;
    }
  }
}