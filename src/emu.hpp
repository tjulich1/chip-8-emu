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
  void LoadInstruction(int p_address, std::bitset<16> p_instruction);
  void SetProgramCounter(int p_pc_value);

  // Step is the method used to execute one fetch->decode->execute cycle.
  void Step();
  void PrintMemory(int p_address);
  void PrintDisplay();
private:
  std::array<Register<8>, 16> variable_registers_;
  Display main_display_;
  Ram memory_;
  int program_counter_;
  Register<16> index_counter_;
  std::vector<std::bitset<16>> ret_address_stack_;

  std::bitset<16> Fetch();
  void Decode();
  void Execute();
};

#endif