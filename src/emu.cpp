// Trent Julich ~ 27 March 2021

#include "emu.hpp"
#include "keyboard_input.hpp"

#include <iostream>

Emu::Emu() {
  
  InitializeFonts();
  program_counter_ = PROGRAM_START;
  set_index_register(0);
}

Emu::Emu(SDL_Renderer* p_renderer) {
  renderer_ = p_renderer;
  main_display_ = Display(renderer_);
  InitializeFonts();
  set_index_register(0);
  sound_timer_ = 0;
  delay_timer_ = 0;
  program_counter_ = PROGRAM_START;
}

void Emu::Start() {
  SDL_AddTimer(1000, delay_timer_callback, this);
  SDL_AddTimer(1000, sound_timer_callback, this);

  while (!Step()) { }
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

bool Emu::Step() {

  bool quit = false;

  SDL_Event e;
  while(SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT: {
        quit = true;
        break;
      }
      case SDL_USEREVENT:
        HandleUserEvent(e);
        break;
      case SDL_KEYDOWN:
        keyboard_.HandleKeyDown(e.key.keysym.scancode);
        break;
      case SDL_KEYUP:
        keyboard_.HandleKeyUp(e.key.keysym.scancode);
        break;
    }
    
  }

  // Grab the next instruction.
  std::bitset<16> current_instruction = Fetch();

  // Pass the instruction to get decoded.
  Decode(current_instruction);

  Render();
  return quit;
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

int Emu::get_sound_timer() {
  return sound_timer_;
}

void Emu::set_delay_timer(int p_new_timer_value) {
  delay_timer_ = p_new_timer_value;
}

int Emu::get_delay_timer() {
  return delay_timer_;
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
      switch ((p_instruction & (third_mask | fourth_mask)).to_ulong()) {
        case 0xE0:
          ClearScreen();
          break;
        case 0xEE: 
          ReturnFromSubroutine();
          break;
      }
      break;
    }
    case 0x1: {
      int new_program_counter = (p_instruction & std::bitset<16>(0x0FFF)).to_ullong();
      Jump(new_program_counter);
      break;
    }
    case 0x2: {
      int routine_address = (p_instruction & (second_mask | third_mask | fourth_mask)).to_ulong();
      ExecuteSubroutine(routine_address);
      break;
    }
    case 0x3: {
      int value_to_check = (p_instruction & (third_mask | fourth_mask)).to_ulong();
      SkipIfEqual(second.to_ulong(), value_to_check);
      break;
    }
    case 0x4: {
      int value_to_check = (p_instruction & (third_mask | fourth_mask)).to_ulong();
      SkipIfNotEqual(second.to_ulong(), value_to_check);
      break;
    }
    case 0xB: {
      int value = (p_instruction & (second_mask | third_mask | fourth_mask)).to_ulong();
      Jump(variable_registers_[0].Read().to_ulong() + value);
      break;
    }
    case 0xC: {
      int value = (p_instruction & (third_mask | fourth_mask)).to_ulong();
      int destination = second.to_ulong();
      GenerateRandom(destination, value);
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
    case 0x8: {
      DecodeRegisterArithmetic(p_instruction);
      break;
    }
    case 0x9: {
      int register_one = second.to_ulong();
      int register_two = third.to_ulong();
      SkipIfRegistersNotEqual(register_one, register_two);
      break;
    }
    case 0xE: {
      int register_num = second.to_ulong();
      std::bitset<8> rest_of_instruction = (third.to_ulong() << 4) + fourth.to_ulong();
      DecodeKeyInstructions(register_num, rest_of_instruction);
      break;
    }
    case 0xF: {
      int register_number = second.to_ulong();
      std::bitset<8> last_bytes((p_instruction & (third_mask | fourth_mask)).to_ulong());
      DecodeRegisterOps(register_number, last_bytes);
      break;
    }
    default: {
      std::cout << "Instruction unknown: " << p_instruction.to_string() << std::endl;
    }
  }
}

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
      std::cout << "Unknown instruction: 0xF" << std::hex << p_register_number << std::hex << p_instruction.to_ulong() << std::endl; 
  }
}

void Emu::DecodeRegisterArithmetic(std::bitset<16> p_instruction) {
  std::bitset<16> second_byte_mask = std::bitset<16>(0x0F00);
  std::bitset<16> third_byte_mask = std::bitset<16>(0x00F0);
  std::bitset<16> fourth_byte_mask = std::bitset<16>(0x000F);

  int first_register = ((p_instruction & third_byte_mask) >> 4).to_ulong();
  int second_register = ((p_instruction & second_byte_mask) >> 8).to_ulong();

  switch((p_instruction & fourth_byte_mask).to_ulong()) {
    case 0x0: 
      StoreRegisterXInY(first_register, second_register);
      break;
    case 0x5: {
      SubtractRegisters(second_register, first_register, second_register);
      break;
    }
    case 0x6: {
      ShiftRegisterRight(first_register, second_register);
      break;
    }
    case 0x7: {
      SubtractRegisters(first_register, second_register, second_register);
      break;
    }
    case 0xE: {
      ShiftRegisterLeft(first_register, second_register); 
      break;
    }
    default: 
      std::cout << "Instruction unknown" << p_instruction << std::endl;
  }
}

void Emu::DecodeKeyInstructions(int p_register_num, std::bitset<8> p_rest_of_instruction) {
  switch(p_rest_of_instruction.to_ulong()) {
    case 0x9E:
      SkipIfKeyPressed(p_register_num);
      break;
    case 0xA1:
      SkipIfKeyNotPressed(p_register_num);
      break;
    default:
      std::cout << "Instruction unknown E" << std::hex << p_register_num << p_rest_of_instruction.to_ulong() << std::endl;
  }
}

void Emu::HandleUserEvent(SDL_Event e) {
  switch (e.user.code) {
    case DELAY_TICK:
      if (delay_timer_ > 0) {
        delay_timer_--;
      }
      break;
    case SOUND_TICK:
      if (sound_timer_ > 0) {
        sound_timer_--;
      }
      break;
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

void Emu::SetSoundTimer(int p_register_number) {
  sound_timer_ = variable_registers_[p_register_number].Read().to_ulong();
}

void Emu::SetDelayTimer(int p_register_number) {
  delay_timer_ = variable_registers_[p_register_number].Read().to_ulong();
}

void Emu::SkipIfEqual(int p_register_number, int p_value) {
  if (variable_registers_[p_register_number].Read().to_ulong() == p_value) {
    program_counter_ += 2;
  }
}

void Emu::SkipIfNotEqual(int p_register_number, int p_value) {
  if (variable_registers_[p_register_number].Read().to_ulong() != p_value) {
    program_counter_ += 2;
  }
}

void Emu::SkipIfRegistersNotEqual(int p_register_one, int p_register_two) {
  int value_one = variable_registers_[p_register_one].Read().to_ulong();
  int value_two = variable_registers_[p_register_two].Read().to_ulong();
  if (value_one != value_two) {
    program_counter_ += 2;
  }
}

void Emu::SkipIfKeyPressed(int p_register) {
  // Find the hex value corresponding to the key to check from variable register.
  int key_to_check = variable_registers_[p_register].Read().to_ulong();

  // Check to see if the given key is currently pressed
  if (key_to_check >= 0 && key_to_check <= 0xF && keyboard_.IsKeyDown(key_to_check)) {
    // If so, skip next instruction.
    program_counter_ += 2;
  }
}

void Emu::SkipIfKeyNotPressed(int p_register) {
  std::cout << "Skip if key not pressed" << std::endl;
  int key_to_check = variable_registers_[p_register].Read().to_ulong();
  std::cout << "Key to check: " << key_to_check << std::endl;
  std::cout << "Pressed? " << keyboard_.IsKeyDown(key_to_check) << std::endl;
  if (key_to_check >= 0 && key_to_check <= 0xF && !keyboard_.IsKeyDown(key_to_check)) {
    std::cout << "Key not pressed" << std::endl;
    program_counter_ += 2;
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

void Emu::StoreRegisterXInY(int p_src_register, int p_dest_register) {
  variable_registers_[p_dest_register].Write(variable_registers_[p_src_register].Read());
}

void Emu::ShiftRegisterLeft(int p_source_register, int p_destination_register) {
  std::bitset<8> value = variable_registers_[p_source_register].Read();
  std::bitset<8> least_sig_bit = value & std::bitset<8>(1);
  value = value << 1;
  variable_registers_[p_destination_register].Write(value);
  variable_registers_[0xF].Write(least_sig_bit);
}

void Emu::ShiftRegisterRight(int p_source_register, int p_destination_register) {
  int value = variable_registers_[p_source_register].Read().to_ulong();
  
  // Mask and write the least significant bit to VF
  variable_registers_[0xF].Write(value & 1);

  // Shift value to right one bit and store in destination register.
  variable_registers_[p_destination_register].Write(value >> 1);
}

void Emu::SubtractRegisters(int p_first_register, int p_second_register, int p_destination_register) {
  int first_value = variable_registers_[p_first_register].Read().to_ulong();
  int second_value = variable_registers_[p_second_register].Read().to_ulong();

  variable_registers_[0xF].Write(1);
  if (second_value > first_value) {
    variable_registers_[0xF].Write(0);
  }

  int result = first_value - second_value;
  if (p_destination_register != 0xF) {
    variable_registers_[p_destination_register].Write(result);
  }
}

void Emu::ExecuteSubroutine(int p_address) {
  // Ensure that the address is even, so its aligned with instruction boundaries.
  if (p_address % 2 == 0) {
    // Store current program counter on stack.
    ret_address_stack_.emplace_back(std::bitset<16>(program_counter_));

    // Set program counter to new address. 
    program_counter_ = p_address;
  }
}

void Emu::ReturnFromSubroutine() {
  if (ret_address_stack_.size() != 0) {
    program_counter_ = ret_address_stack_.back().to_ulong();
    ret_address_stack_.pop_back();
  }
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

void Emu::SetSpriteMemoryAddress(int p_register_number) {
  int sprite_value = variable_registers_[p_register_number].Read().to_ulong();
  if (sprite_value >= 0 && sprite_value <= 0xF) {
    index_register_.Write(std::bitset<16>(0x50 + sprite_value));
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

void Emu::GenerateRandom(int p_register, int p_mask) {
  int start_value = rand() % 0x100;
  start_value &= p_mask;
  variable_registers_[p_register].Write(start_value);
}

void Emu::DelayTick() {
  SDL_Event event;
  event.user.code = DELAY_TICK;
  event.type = SDL_USEREVENT;
  SDL_PushEvent(&event);
}

void Emu::SoundTick() {
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