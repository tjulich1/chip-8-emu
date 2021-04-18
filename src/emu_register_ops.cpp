#include "emu.hpp"

#include <iostream>

void Emu::DecodeRegisterOps(int p_register_number, std::bitset<8> p_instruction) {
  switch(p_instruction.to_ulong()) {
    case 0x7: {
      variable_registers_[p_register_number].Write(delay_timer_);
      break;
    }
    case 0xA: {
      WaitForKeyPress(p_register_number);
      break;
    }
    case 0x15: {
      set_delay_timer(get_register(p_register_number));
      break;
    }
    case 0x18: {
      set_sound_timer(get_register(p_register_number));
      break;
    }
    case 0x1E: {
      AddRegisterToIndex(p_register_number);
      break;
    }
    case 0x29: {
      SetSpriteMemoryAddress(p_register_number);
      break;
    }
    case 0x33: {
      StoreBinaryCodedDecimal(p_register_number);
      break;
    }
    case 0x55: {
      StoreRegistersToMem(p_register_number);
      break; 
    }
    case 0x65: {
      ReadMemToRegisters(p_register_number);
      break;
    }
    default:{
      std::cout << "Unknown instruction: 0xF" << std::hex << p_register_number << std::hex 
        << p_instruction.to_ulong() << std::endl; 
    }
  }
}

void Emu::WaitForKeyPress(int p_register) {
  bool waiting = true;
  while (waiting) {
    SDL_Event e;
    while(SDL_PollEvent(&e) && waiting) {
      switch (e.type) {
        // If we get a quit event, push it back to the event queue and stop waiting for a key.
        case SDL_QUIT:
          SDL_PushEvent(&e);
          waiting = false;
          break;
        // If we get a key press, check if its a recognized key and if so, save to register.
        case SDL_KEYDOWN: {
          SDL_Scancode code = e.key.keysym.scancode;
          if (keyboard_.IsValidKey(code)) {
            variable_registers_[p_register].Write(keyboard_.GetKeyValue(code));
            waiting = false;
          }
        }
      }
    }
  }
}

void Emu::AddRegisterToIndex(int p_register_number) {
  int index_value = index_register_.Read().to_ulong();
  int value_to_add = variable_registers_[p_register_number].Read().to_ulong();
  index_register_.Write(std::bitset<16>(index_value + value_to_add));
}

void Emu::SetSpriteMemoryAddress(int p_register_number) {
  int sprite_value = get_register(p_register_number);
  if (sprite_value >= 0 && sprite_value <= 0xF) {
    // Must multiply sprite_value by 5 because each sprite has 5 rows of pixels.
    index_register_.Write(std::bitset<16>(0x50 + (sprite_value * 5)));
  }
}

void Emu::StoreBinaryCodedDecimal(int p_register_number) {
  int value_to_store = variable_registers_[p_register_number].Read().to_ulong();

  // Extract the values 
  int hundreds_place = value_to_store / 100;
  int tens_place = (value_to_store / 10) % 10;
  int ones_place = (value_to_store % 100) % 10;

  int mem_start = index_register_.Read().to_ulong();

  // Store each digit in its own byte in memory.
  memory_.Write(mem_start, std::bitset<8>(hundreds_place));
  memory_.Write(mem_start + 1, std::bitset<8>(tens_place));
  memory_.Write(mem_start + 2, std::bitset<8>(ones_place));
}

void Emu::StoreRegistersToMem(int p_register_number) {
  for (int i = 0; i <= p_register_number; i++) {
    int address_to_write = index_register_.Read().to_ulong() + i;
    std::bitset<8> value_to_write = variable_registers_[i].Read();
    memory_.Write(address_to_write, value_to_write);
  }
  index_register_.Write(std::bitset<16>(index_register_.Read().to_ulong() + p_register_number + 1));
}

void Emu::ReadMemToRegisters(int p_register_number) {
  int start_mem = index_register_.Read().to_ulong();
  for (int i = 0; i <= p_register_number; i++) {
    variable_registers_[i].Write(memory_.Read(start_mem + i));
  }
  index_register_.Write(start_mem + p_register_number + 1);
}