// Trent Julich ~ 25 March 2021

#ifndef EMU_HPP
#define EMU_HPP

#include "ram.hpp"
#include "display.hpp"
#include "register.hpp"

#include <array>
#include <vector>
#include <bitset>

/**
 * Main emulator class. Contains all the emulated harware components (ram, display, registers).
 */
class Emu {

public: 

  Emu();

  void LoadInstruction(int p_address, std::bitset<16> p_instruction);
  void SetProgramCounter(int p_pc_value);
  void SetIndexRegister(int p_index_value);

  int GetProgramCounter();
  std::bitset<16> GetIndexRegister();

  // Step is the method used to execute one fetch->decode->execute cycle.
  void Step();
  void PrintMemory(int p_address);
  void PrintDisplay();
  void PrintRegisters();
  int GetRegister(int p_register);
  void SetRegister(int p_register, int p_new_value);
  Display& get_display();
private:
  std::array<Register<8>, 16> variable_registers_;
  Display main_display_;
  Ram memory_;
  int program_counter_;
  Register<16> index_register_;
  std::vector<std::bitset<16>> ret_address_stack_;

  void InitializeFonts();

  std::bitset<16> Fetch();
  void Decode(std::bitset<16> p_instruction);
  void Execute();

  // Instructions
  void ClearScreen();
  void Jump(int p_new_program_counter);
  void AddValToRegister(int p_register, int p_val);
  void Display(int p_rows, int p_x_coord, int p_y_coord);
};

#endif