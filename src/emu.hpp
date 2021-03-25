// Trent Julich ~ 25 March 2021

#ifndef EMU_HPP
#define EMU_HPP

#include "ram.hpp"
#include "display.hpp"
#include "register.hpp"

#include <array>

/**
 * Main emulator class. Contains all the emulated harware components (ram, display, registers).
 */
class Emu {

public: 
  void LoadInstruction(int p_address, std::array<bool, 8> p_instruction);
  void SetProgramCounter(std::array<bool, 16> p_pc_value);
  void Step();
  void PrintMemory(int p_address);
private:
  std::array<Register<8>, 16> variable_registers_;
  Display main_display_;
  Ram memory_;
  Register<16> program_counter_;
  Register<16> index_counter_;

};

#endif