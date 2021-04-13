#include "emu.hpp"

#include <iostream>

void Emu::DecodeRegisterOps(int p_register_number, std::bitset<8> p_instruction) {
  switch(p_instruction.to_ulong()) {
    case 0x7:
      variable_registers_[p_register_number].Write(delay_timer_);
      break;
    case 0xA:
      WaitForKeyPress(p_register_number);
      break;
    case 0x15:
      SetDelayTimer(p_register_number);
    case 0x18:
      SetSoundTimer(p_register_number);
      break;
    case 0x1E:
      AddRegisterToIndex(p_register_number);
      break;
    case 0x29:
      SetSpriteMemoryAddress(p_register_number);
      break;
    case 0x33: 
      StoreBinaryCodedDecimal(p_register_number);
      break;
    case 0x55:
      StoreRegistersToMem(p_register_number);
      break; 
    case 0x65:
      ReadMemToRegisters(p_register_number);
      break;
    default:
      std::cout << "Unknown instruction: 0xF" << std::hex << p_register_number << std::hex 
        << p_instruction.to_ulong() << std::endl; 
  }
}