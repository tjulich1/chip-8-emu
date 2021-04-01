// Trent Julich ~ 27 March 2021

#include "emu.hpp"

#include <iostream>

Emu::Emu() {
  InitializeFonts();
}

Emu::Emu(SDL_Renderer* p_renderer) {
  renderer_ = p_renderer;
  main_display_ = Display(renderer_);
  InitializeFonts();
  sound_timer_ = 0;
  delay_timer_ = 0;
}

void Emu::Start() {
  SDL_AddTimer(1000, delay_timer_callback, this);
  SDL_AddTimer(1000, sound_timer_callback, this);

  bool quit = false;
  while (!quit) {

    SDL_Event e;
    while(SDL_PollEvent(&e)) {

      switch (e.type) {
        case SDL_QUIT: {
          quit = true;
          break;
        }
        case SDL_USEREVENT:
          switch (e.user.code) {
            case DELAY_TICK:
              if (delay_timer_ > 0) {
                delay_timer_--;
                std::cout << "Delay tick" << std::endl;
              }
              break;
            case SOUND_TICK:
              if (sound_timer_ > 0) {
                sound_timer_--;
                std::cout << "Sound tick" << std::endl;
              }
              break;
          }
          break;
      }
    }

    std::bitset<16> current_instruction = Fetch();
    if (current_instruction != std::bitset<16>(0x000)) {
      Decode(current_instruction);
      Render();
    } else {
      quit = true;
    } 
  }
}

void Emu::LoadInstruction(int p_address, std::bitset<16> p_instruction) {
  // Grab the leftmost 8 bits of the instruction.
  std::bitset<8> first_byte(p_instruction.to_string().substr(0, 8));

  // Grab the rightmost 8 bits of the instruction.
  std::bitset<8> second_byte(p_instruction.to_string().substr(8, 8));

  // Write both bytes back to back in memory.
  memory_.Write(p_address, first_byte);
  memory_.Write(p_address + 1, second_byte);
}

void Emu::Step() {
  // Grab the next instruction.
  std::bitset<16> current_instruction = Fetch();

  // Pass the instruction to get decoded.
  Decode(current_instruction);
}

void Emu::Render() {

  // Fill with white
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
  SDL_RenderClear(renderer_);

  // Draw the display
  main_display_.Render();

  SDL_RenderPresent(renderer_);
}

void Emu::PrintMemory(int p_address) {
  std::cout << "Mem[" << std::hex << p_address << "]: ";

  // Read the byte from memory
  std::bitset<8> read_value = memory_.Read(p_address);

  // Split the byte into 4 bit nibbles.
  std::bitset<4> first_nibble(read_value.to_string().substr(0, 4));
  std::bitset<4> second_nibble(read_value.to_string().substr(4, 4));

  // Convert each value to hex and output to console.  
  std::cout << std::hex << first_nibble.to_ulong() << second_nibble.to_ulong() << std::endl;
}

void Emu::PrintDisplay() {
  main_display_.Print();
}

void Emu::PrintRegisters() {
  // Print program counter.
  std::cout << "PC: " << std::hex << program_counter_ << std::endl; 

  // Print value of index register.
  std::cout << "Index Register: " << std::hex << index_register_.Read().to_ulong() << std::endl;
  
  // Print each of the variable registers.
  for (int i = 0; i < variable_registers_.size(); i++) {
    std::cout << "V" << std::hex << i << ": " << variable_registers_[i].Read().to_ulong() 
      << std::endl;
  }
}

Display& Emu::get_display() {
  return main_display_;
}

void Emu::set_program_counter(int p_pc_value) {
  program_counter_ = p_pc_value;
}

int Emu::get_program_counter() {
  return program_counter_;
}

void Emu::set_index_register(int p_new_value) {
  index_register_.Write(std::bitset<16>(p_new_value));
}

std::bitset<16> Emu::get_index_register() {
  return index_register_.Read();
}

int Emu::get_register(int p_register) {
  // Initialize return value to zero.
  std::bitset<8> value(0);
  if (p_register > -1 && p_register < 16) {
    // Set return value to register value if p_register is valid.
    value = variable_registers_[p_register].Read();
  }
  return value.to_ulong();
}

void Emu::set_register(int p_register, int p_new_value) {
  // Make sure that 0 <= p_register < 16
  if (p_register > -1 && p_register < 16) {
    variable_registers_[p_register].Write(std::bitset<8>(p_new_value));
  }
}

int Emu::get_memory(int p_address) {
  return memory_.Read(p_address).to_ulong();
}

void Emu::set_memory(int p_address, int p_value) {
  memory_.Write(p_address, std::bitset<8>(p_value));
}

void Emu::Decode(std::bitset<16> p_instruction) {
  std::bitset<16> first_mask(0xF000);
  std::bitset<16> second_mask(0x0F00);
  std::bitset<16> third_mask(0x00F0);
  std::bitset<16> fourth_mask(0x000F);

  // Grab second nibble
  std::bitset<4> second(p_instruction.to_string().substr(4, 4));
  // Grab third nibble
  std::bitset<4> third(p_instruction.to_string().substr(8, 4));
  // Grab far right nibble
  std::bitset<4> fourth(p_instruction.to_string().substr(12, 4));

  switch(((p_instruction & first_mask) >> 12).to_ulong()) {
    case 0x0: {
      ClearScreen();
      break;
    }
    case 0x1: {
      int new_program_counter = (p_instruction & std::bitset<16>(0x0FFF)).to_ullong();
      Jump(new_program_counter);
      break;
    }
    case 0xD: {
      int rows = fourth.to_ulong();
      int x_coord = variable_registers_[second.to_ulong()].Read().to_ulong();
      int y_coord = variable_registers_[third.to_ulong()].Read().to_ulong();
      DisplaySprite(rows, x_coord, y_coord);
      break;
    }
    case 0x7: {
      int register_number = second.to_ulong();
      int value_to_add = (p_instruction & (third_mask | fourth_mask)).to_ulong();
      AddValToRegister(register_number, value_to_add);
      break;
    }
    case 0x6: {
      int register_number = second.to_ulong();
      int new_value = (p_instruction & (third_mask | fourth_mask)).to_ulong();
      set_register(register_number, new_value);
      break;
    }
    case 0xA: {
      set_index_register((p_instruction & (second_mask | third_mask | fourth_mask)).to_ulong());
      break;
    }
    case 0xF: {
      int register_number = second.to_ulong();
      std::bitset<8> last_bytes((p_instruction & (third_mask | fourth_mask)).to_ulong());
      DecodeRegisterOps(register_number, last_bytes);
    }
  }
}

std::bitset<16> Emu::Fetch() {
  // Grab leftmost byte of instruction
  std::bitset<8> first_byte = memory_.Read(program_counter_);

  // Grab rightmost byte of instruction.
  std::bitset<8> second_byte = memory_.Read(program_counter_ + 1);

  // Increment the program counter to point at start of next instruction.
  program_counter_ += 2;

  // Return bitstring concatenating both bytes together.
  return std::bitset<16>(first_byte.to_string() + second_byte.to_string());
}

void Emu::ClearScreen() {
  main_display_.Clear();
}

void Emu::Jump(int p_new_program_counter) {
  program_counter_ = p_new_program_counter;
}

void Emu::AddValToRegister(int p_register, int p_val) {
  if (p_register > -1 && p_register < 16) {
    int new_value = variable_registers_[p_register].Read().to_ulong() + p_val;
    variable_registers_[p_register].Write(std::bitset<8>(new_value));
  }
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

void Emu::AddRegisterToIndex(int p_register_number) {
  int index_value = index_register_.Read().to_ulong();
  int value_to_add = variable_registers_[p_register_number].Read().to_ulong();
  index_register_.Write(std::bitset<16>(index_value + value_to_add));
}

void Emu::DisplaySprite(int p_rows, int p_x_coord, int p_y_coord) {
  // For p_rows:
  for (int i = 0; i < p_rows; i++) { 
    // Grab the current byte of the sprite from address stored in index register + i
    std::bitset<8> sprite = memory_.Read(index_register_.Read().to_ulong() + i);
    for (int j = 7; j > -1; j--) { // Iterate over each bit in the byte left to right.
      // Check if a pixel would be drawn on an existing pixel
      if (main_display_.GetPixel(p_y_coord + i, p_x_coord + (7-j)) && sprite[j]) {
        // Write 1 to the flag register.
        variable_registers_[0xF].Write(std::bitset<8>(1)); 
      }
      // If there wasn't a collision, check if the pixel should be drawn. 
      else if (sprite[j]) {
        main_display_.SetPixel(p_y_coord + i, p_x_coord + (7-j), 1);
      } 
      // Stop drawing row if it would go off right edge of screen.
      if (p_x_coord + (7-j) >= main_display_.get_pixel_width()) {
        break;
      }
    }
    // Stop drawing entire sprite if the bottom of the sprite goes off screen.
    if (p_y_coord + i >= main_display_.get_pixel_height()) {
      break;
    }
  }
}

void Emu::DecodeRegisterOps(int p_register_number, std::bitset<8> p_instruction) {
  switch(p_instruction.to_ulong()) {
    case 0x1E:
      AddRegisterToIndex(p_register_number);
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
  }
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

void Emu::DelayTick() {
  std::cout << "Pushed delay event" << std::endl;
  SDL_Event event;
  event.user.code = DELAY_TICK;
  event.type = SDL_USEREVENT;
  SDL_PushEvent(&event);
}

void Emu::SoundTick() {
  std::cout << "Pushed sound event" << std::endl;
  SDL_Event event;
  event.user.code = SOUND_TICK;
  event.type = SDL_USEREVENT;
  SDL_PushEvent(&event);
}

Uint32 Emu::delay_timer_callback(Uint32 p_interval, void* param) {
  // Cast the void pointer to a pointer to the emulator we are wanting to call DelayTick on.
  Emu* emu = static_cast<Emu*>(param);
  emu->DelayTick();
  return p_interval;
}

Uint32 Emu::sound_timer_callback(Uint32 p_interval, void* param) {
  // Cast the void pointer to a pointer to the emulator we are wanting to call DelayTick on.
  Emu* emu = static_cast<Emu*>(param);
  emu->SoundTick();
  return p_interval;
}