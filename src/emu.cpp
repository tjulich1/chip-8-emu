// Trent Julich ~ 27 March 2021

#include "emu.hpp"
#include "keyboard_input.hpp"

#include <bitset>
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

void Emu::LoadInstruction(int p_address, std::bitset<16> p_instruction) { 
  // Grab the leftmost 8 bits of the instruction.
  std::bitset<8> first_byte(p_instruction.to_string().substr(0, 8));

  // Grab the rightmost 8 bits of the instruction.
  std::bitset<8> second_byte(p_instruction.to_string().substr(8, 8));

  // Write both bytes back to back in memory.
  memory_.Write(p_address, first_byte);
  memory_.Write(p_address + 1, second_byte);
}

void PrintInstruction(std::bitset<16> p_instruction) {
  int first_nibble = (p_instruction.to_ulong() & 0xF000) >> 12;
  int second_nibble = (p_instruction.to_ulong() & 0xF00) >> 8;
  int third_nibble = (p_instruction.to_ulong() & 0xF0) >> 4;
  int fourth_nibble = (p_instruction.to_ulong() & 0xF);

  std::cout << "Instruction: " << std::hex << first_nibble << second_nibble << third_nibble 
    << fourth_nibble << std::endl;
}

void Emu::Step() {
  // Grab the next instruction.
  std::bitset<16> current_instruction = Fetch();

  // PrintInstruction(current_instruction);

  // Pass the instruction to get decoded.
  Decode(current_instruction);

  Render();
}

void Emu::Render() {
  // Draw the display
  main_display_.Render();
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
    std::cout << "V" << std::hex << i << ": " << get_register(i) 
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
  int value(0);
  if (p_register >= 0 && p_register < 16) {
    // Set return value to register value if p_register is valid.
    value = variable_registers_[p_register].Read().to_ulong();
  }
  return value;
}

void Emu::set_register(int p_register, int p_new_value) {
  if (p_register >= 0 && p_register < 16) {
    variable_registers_[p_register].Write(std::bitset<8>(p_new_value));
  }
}

int Emu::get_memory(int p_address) {
  return memory_.Read(p_address).to_ulong();
}

void Emu::set_memory(int p_address, int p_value) {
  memory_.Write(p_address, std::bitset<8>(p_value));
}

void Emu::set_sound_timer(int p_new_timer_value) {
  sound_timer_ = p_new_timer_value;
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

std::array<Register<8>, 16>* Emu::get_variable_registers() {
  return &variable_registers_;
}

void Emu::Decode(std::bitset<16> p_instruction) {
  std::bitset<16> first_mask(0xF000);
  std::bitset<16> second_mask(0x0F00);
  std::bitset<16> third_mask(0x00F0);
  std::bitset<16> fourth_mask(0x000F);

  int first_nibble = ((p_instruction & first_mask) >> 12).to_ulong();
  int second_nibble = ((p_instruction & second_mask) >> 8).to_ulong();
  int third_nibble = ((p_instruction & third_mask) >> 4).to_ulong();
  int fourth_nibble = ((p_instruction & fourth_mask)).to_ulong();

  int second_byte = (p_instruction & (third_mask | fourth_mask)).to_ulong();

  std::bitset<4> first(p_instruction.to_string().substr(0, 4));
  // Grab second nibble
  std::bitset<4> second(p_instruction.to_string().substr(4, 4));
  // Grab third nibble
  std::bitset<4> third(p_instruction.to_string().substr(8, 4));
  // Grab far right nibble
  std::bitset<4> fourth(p_instruction.to_string().substr(12, 4));

  switch(first_nibble) {
    case 0x0: {
      switch (second_byte) {
        case 0xE0: {
          ClearScreen();
          break;
        } 
        case 0xEE: {
          ReturnFromSubroutine();
          break;
        } 
        default: {
          std::cout << "-> Unknown instruction" << std::endl;
        }
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
      SkipIfEqual(second_nibble, second_byte);
      break;
    }
    case 0x4: {
      SkipIfNotEqual(second_nibble, second_byte);
      break;
    }
    case 0x5: {
      if (fourth_nibble == 0) {
        SkipIfRegistersEqual(second_nibble, third_nibble);
      } else {
        std::cout << "-> Unknown instruction 0x" << std::hex << p_instruction.to_ulong() 
          << std::endl;
      }      
      break;
    }
    case 0x6: {
      set_register(second_nibble, second_byte);
      break;
    }
    case 0x7: {
      AddValToRegister(second_nibble, second_byte);
      break;
    }
    case 0x8: {
      DecodeRegisterArithmetic(p_instruction);
      break;
    }
    case 0x9: {
      if (fourth_nibble == 0) {
        SkipIfRegistersNotEqual(second_nibble, third_nibble);
      } else {
        std::cout << "-> UNKNOWN INSTRUCTION: 0x" << std::hex << p_instruction.to_ulong() 
          << std::endl;
      }
      
      break;
    }
    case 0xA: {
      set_index_register((p_instruction & (second_mask | third_mask | fourth_mask)).to_ulong());
      break;
    }
    case 0xB: {
      int value = (p_instruction & (second_mask | third_mask | fourth_mask)).to_ulong();
      Jump(get_register(0) + value);
      break;
    }
    case 0xC: {
      GenerateRandom(second_nibble, second_byte);
      break;
    }
    case 0xD: {
      int x_coord = get_register(second_nibble);
      int y_coord = get_register(third_nibble);
      DisplaySprite(fourth_nibble, x_coord, y_coord);
      break;
    }
    case 0xE: {
      std::bitset<8> rest_of_instruction(second_byte);
      DecodeKeyInstructions(second_nibble, rest_of_instruction);
      break;
    }
    case 0xF: {
      std::bitset<8> last_bytes(second_byte);
      DecodeRegisterOps(second_nibble, last_bytes);
      break;
    }
    default: {
      std::cout << "-> Instruction unknown: " << p_instruction.to_string() << std::endl;
    }
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
      std::cout << "-> Instruction unknown E" << std::hex << p_register_num  
        << p_rest_of_instruction.to_ulong() << std::endl;
  }
}

void Emu::HandleUserEvent(SDL_Event e) {
  switch (e.user.code) {
    case DELAY_TICK: {
      if (delay_timer_ > 0) {
        delay_timer_--;
      }
      break;
    }
    case SOUND_TICK: {
      if (sound_timer_ > 0) {
        sound_timer_--;
      }
      break;
    }
    case STEP_TICK: {
      Step();
      break;
    }
    default:
      std::cout << "Unknown event: " << e.user.code << std::endl;
  }
}

void Emu::KeyDown(SDL_Scancode p_code) {
  keyboard_.HandleKeyDown(p_code); 
}

void Emu::KeyUp(SDL_Scancode p_code) {
  keyboard_.HandleKeyUp(p_code);
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
    int new_value = get_register(p_register) + p_val;
    set_register(p_register, new_value);
  }
} 

void Emu::SkipIfEqual(int p_register_number, int p_value) {
  if (get_register(p_register_number) == p_value) {
    program_counter_ += 2;
  }
}

void Emu::SkipIfRegistersEqual(int p_first_register, int p_second_register) {
  int first_value = get_register(p_first_register);
  int second_value = get_register(p_second_register);
  if (first_value == second_value) {
    program_counter_ += 2;
  }
}

void Emu::SkipIfNotEqual(int p_register_number, int p_value) {
  if (get_register(p_register_number) != p_value) {
    program_counter_ += 2;
  }
}

void Emu::SkipIfRegistersNotEqual(int p_register_one, int p_register_two) {
  int value_one = get_register(p_register_one);
  int value_two = get_register(p_register_two);
  if (value_one != value_two) {
    program_counter_ += 2;
  }
}

void Emu::SkipIfKeyPressed(int p_register) {
  // Find the hex value corresponding to the key to check from variable register.
  int key_to_check = get_register(p_register);

  // Check to see if the given key is currently pressed
  if (key_to_check >= 0 && key_to_check <= 0xF && keyboard_.IsKeyDown(key_to_check)) {
    // If so, skip next instruction.
    program_counter_ += 2;
  }
}

void Emu::SkipIfKeyNotPressed(int p_register) {
  int key_to_check = get_register(p_register);
  if (key_to_check >= 0 && key_to_check <= 0xF && !keyboard_.IsKeyDown(key_to_check)) {
    program_counter_ += 2;
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
  // Wrap around if coordinates would place sprite off screen.
  p_x_coord = p_x_coord % 64;
  p_y_coord = p_y_coord % 32;

  variable_registers_[0xF].Write(0);

  for (int i = 0; i < p_rows; i++) { 
    // Grab the current byte of the sprite from address stored in index register + i
    std::bitset<8> sprite = memory_.Read(index_register_.Read().to_ulong() + i);
    for (int j = 7; j > -1; j--) { // Iterate over each bit in the byte left to right.
      // Check if a pixel would be drawn on an existing pixel
      if (main_display_.GetPixel(p_y_coord + i, p_x_coord + (7-j)) && sprite[j]) {
        // Write 1 to the flag register.
        main_display_.SetPixel(p_y_coord + i, p_x_coord + (7-j), 0);
        variable_registers_[0xF].Write(1); 
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
  set_register(p_register, start_value);
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

void Emu::StepTick() {
  SDL_Event event;
  event.user.code = STEP_TICK;
  event.type = SDL_USEREVENT;
  SDL_PushEvent(&event);
}

