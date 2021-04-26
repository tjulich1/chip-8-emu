// Trent Julich ~ 25 March 2021

#ifndef EMU_HPP
#define EMU_HPP

#include "display.hpp"
#include "keyboard_input.hpp"
#include "ram.hpp"
#include "register.hpp"

#include <array>
#include <bitset>
#include <vector>

/**
 * Main emulator class. Contains all the emulated harware components (ram, display, registers), as 
 * well as methods to load and execute programs.
 */
class Emu {

public: 

  /**
   * The address that programs should start at.
   */
  const int PROGRAM_START = 0x200;

  /**
   * Default constructor.
   */
  Emu();

  /**
   * Constructor that takes SDL_Renderer pointer that the emulator display should be rendered to.
   */
  Emu(SDL_Renderer* p_renderer);

  /**
   * Load the 16 bit instruction into memory starting at p_address. The left byte is stored in 
   * mem[p_address] and the right byte is stored in mem[p_address + 1];
   */
  void LoadInstruction(int p_address, std::bitset<16> p_instruction);

  /**
   *  Fetches, decodes, and executes the next instruction pointed to by the program counter.
   */
  void Step();  

  /**
   * Called to render the current display state to the renderer being used.
   */
  void Render();

  /**
   * Print the byte byte at mem[p_address] to the console.
   */
  void PrintMemory(int p_address);

  /**
   * Print the pixel matrix to the console.
   */
  void PrintDisplay();

  /**
   * Prints the values of all registers to the console.
   */ 
  void PrintRegisters();
  
  /**
   * Returns a reference to the main display, mainly used to manipulate individual pixels directly.
   */
  Display& get_display();

  /**
   * Set the address pointed to by the program counter to p_pc_value.
   */
  void set_program_counter(int p_pc_value);

  /**
   * Get the memory address stored in the program counter.
   */
  int get_program_counter();

  /**
   * Write the value p_index_value into the index register.
   */
  void set_index_register(int p_index_value);

  /**
   * Get the 16 bits that are stored in the index register.
   */
  std::bitset<16> get_index_register();

  /**
   * Getter for the value held in register p_register. If p_register is < 0 or > 15 the value zero 
   * is returned instead.
   */
  int get_register(int p_register);

  /**
   * Store the value p_new_value in variable register p_register.
   */
  void set_register(int p_register, int p_new_value);

  /**
   * Gets the value of stored in memory at the given address.
   */
  int get_memory(int p_address);

  /**
   * Stores the value p_value in memory at p_addres.
   */
  void set_memory(int p_address, int p_value);

  /**
   * Updates sound timer to new timer value.
   */ 
  void set_sound_timer(int p_new_timer_value);

  /**
   * Returns the current value of the sound timer.
   */
  int get_sound_timer();

  /**
   * Updates delay timer to new timer value.
   */
  void set_delay_timer(int p_new_timer_value);

  /**
   * Returns the current value of the delay timer.
   */
  int get_delay_timer();

  /**
   * Returns reference to the array that contains all variable registers.
   */
  std::array<Register<8>, 16>* get_variable_registers();

  /**
   * Function that on emulator start, is called 60 times a second to decrement delay counter.
   */
  void DelayTick();

  /**
   * Function that on emulator start, is called 60 times a second to decrement sound counter.
   */
  void SoundTick();

  /**
   * Function that on emulator start, is called at 500hz to cycle the emulator.
   */
  void StepTick();

  /**
   * Helper method used to handle user defined events.
   */
  void HandleUserEvent(SDL_Event p_e);

  /**
   * Method called when a key press occurs.
   */ 
  void KeyDown(SDL_Scancode p_code);

  /**
   * Method called when a key is released.
   */
  void KeyUp(SDL_Scancode p_code);

private:

  /**
   * Enum used when 60hz timer events are added to the event queue.
   */
  enum CustomEvents {
    DELAY_TICK, 
    SOUND_TICK,
    STEP_TICK, 
  };

  /**
   * Event handler used to handle input and timer events.
   */
  SDL_Event event_handler_;

  /**
   * Object used to handle keyboard inputs.
   */
  KeyboardInput keyboard_;

  /**
   * 16 8-bit registers used to store program data. Registers are usually indexed 0x0 thru 0xF.
   */
  std::array<Register<8>, 16> variable_registers_;

  /**
   * The display object that the program renders its pixel data to.
   */
  Display main_display_;

  /**
   * The renderer that the emulator is rendering to.
   */
  SDL_Renderer* renderer_;

  /**
   * 4kB of simulated memory, with 4096 addressable 8-bit locations.
   */
  Ram memory_;

  /**
   * Counter that keeps track of the memory address of the next instruction to fetch.
   */
  int program_counter_;

  /**
   * Delay timer used by chip-8 programs. Will decrease at a rate of 60Hz until it reaches 0.
   */ 
  int delay_timer_;

  /**
   * Timer used by programs utilizing sound. Will decrease at a rate of 60Hz until it reaches 0.
   */
  int sound_timer_;

  /**
   * 16-Bit index register.
   */
  Register<16> index_register_;

  /**
   * Vector serving as a stack to store 16 bit return addresses.
   */
  std::vector<std::bitset<16>> ret_address_stack_;

  /**
   * Method used to grab and return the instruction pointed to by program counter. The program 
   * counter is then incremented to point to start of next instruction.
   */
  std::bitset<16> Fetch();

  /**
   * Decodes the instruction to determine what instruction and data values to use during execution. 
   * The data values are then passed to the corresponding instruction handler.
   */
  void Decode(std::bitset<16> p_instruction);
  
  /**
   * Helper method to decode instructions that only relate to a single register.
   */
  void DecodeRegisterOps(int p_register_number, std::bitset<8> p_instruction);

  /**
   * Helper method that decodes instructions that relate to arithmetic operations between registers.
   */
  void DecodeRegisterArithmetic(std::bitset<16> p_instruction);

  /**
   * Decodes the rest of instructions the correspond to instructions which handle checking key 
   * states.
   */
  void DecodeKeyInstructions(int p_register_num, std::bitset<8> p_rest_of_instruction);

  /**
   * Used to reset the main display to all blank pixels.
   */
  void ClearScreen();

  /**
   * Adjust the program counter 
   */
  void Jump(int p_new_program_counter);

  /**
   * Adds the value p_val to the value already stored in variable register p_register. This value is
   * written back to the same register.
   */
  void AddValToRegister(int p_register, int p_val);

  /**
   * Stores register V0 -> Vp_register_number in memory starting at the address stored in the index
   * register.
   */
  void StoreRegistersToMem(int p_register_number);

  /**
   * Stores the bytes found at mem[index] -> mem[index + p_register_number] into variable registers
   * V0 -> p_register_number
   */
  void ReadMemToRegisters(int p_register_number);

  /**
   * Stores the binary coded decimal equivalent of the value in variable register p_register_number
   * into memory at mem[index], mem[index+1], mem[index+2].
   */
  void StoreBinaryCodedDecimal(int p_register_number);

  /**
   * Adds the value stored in register p_register_number to the index counter, and stores back in 
   * the index counter.
   */
  void AddRegisterToIndex(int p_register_number);


  /**
   * Skips the next instruction if the value in p_register_number == p_value.
   */
  void SkipIfEqual(int p_register_number, int p_value);

  /**
   * Compares the values stores in p_first_register and p_second_register, and skips the next 
   * instruction if the values are equal.
   */
  void SkipIfRegistersEqual(int p_first_register, int p_second_register);

  /**
   * Skips the next instruction if the value in p_register_number does not equal the given value.
   */
  void SkipIfNotEqual(int p_register_number, int p_value);

  /**
   * Skips the next instruction if the two given registers do not hold equal values.
   */
  void SkipIfRegistersNotEqual(int p_register_one, int p_register_two);

  /**
   * Skips the next instructions if the key corresponding to the hex value in variable register 
   * p_register is currently pressed.
   */
  void SkipIfKeyPressed(int p_register);

  /**
   * Skips the next instruction if the key corresponding to the hex value in variable register
   * p_register is not currently pressed.
   */
  void SkipIfKeyNotPressed(int p_register);

  /**
   * Wait for a key to be pressed, and store that value in register p_register.
   */
  void WaitForKeyPress(int p_register);
  
  /**
   * Stores the current address on the stack, and jumps to subroutine located at p_address. 
   * p_address must be an even integer, so that program counter can stay aligned with instruction 
   * boundaries.
   */
  void ExecuteSubroutine(int p_address);

  /**
   * Instruction used to return from the current subroutine, i.e. pop an address from the stack and 
   * set the program counter to this value. If there are no addresses on the stack to return to, 
   * this instruction is skipped.
   */
  void ReturnFromSubroutine();

  /**
   * Draws a sprite that is p_rows tall starting at p_xcoord, p_ycoord. The bytes used to draw each
   * row should be stored in memory, and the address of the first byte that should be drawn should 
   * be stored in the index register. 
   */
  void DisplaySprite(int p_rows, int p_x_coord, int p_y_coord);

  /**
   * Sets the index register to the address of the sprite corresponding to the hex digit stored in 
   * variable register p_register_number. I.E. if variable register p_register_number contains the
   * value 0x4, the index register would be set to the address of the sprite for the '4' character.
   */
  void SetSpriteMemoryAddress(int p_register_number);

  /**
   * Stores basic sprites for fonts into memory from 0x50 -> 0x5F
   */
  void InitializeFonts();

  /**
   * Generates a random number, AND masks it with p_mask, and stores in register p_register.
   */
  void GenerateRandom(int p_register, int p_mask);  

  /**
   * When an emulator is started, it uses this callback and SDL_AddTimer to register to have 
   * DelayTick() method called every p_interval m.s.
   */
  static Uint32 delay_timer_callback(Uint32 p_interval, void* p_param);

  /**
   * When an emulator is started, it uses this callback and SDL_AddTimer to register to have 
   * SoundTick() method called every p_interval m.s.
   */
  static Uint32 sound_timer_callback(Uint32 p_interval, void* p_param);

  /**
   * When an emulator is started, it uses this callback and SDL_AddTimer to register to have 
   * StepTick() called every p_interval m.s.
   */
  static Uint32 step_timer_callback(Uint32 p_interval, void* p_param);

};

#endif