#include "catch.hpp"
#include "../src/emu.hpp "

Emu test_emu;

TEST_CASE("Testing clear screen instruction", "[instructions]") {
  std::bitset<16> clear_instruction(0x00e0);
  
  // Get a reference to the emulator display.
  Display &main_display = test_emu.get_display();

  // Force draw some pixels.
  main_display.SetPixel(1, 1, 1);
  main_display.SetPixel(3, 5, 1);
  
  REQUIRE(!main_display.IsClear());

  // Load in machine code instruction to clear the display.
  test_emu.LoadInstruction(0,clear_instruction);
  test_emu.SetProgramCounter(0);

  // Clear screen
  test_emu.Step();
  
  REQUIRE(main_display.IsClear());
}

TEST_CASE("Testing set program counter instruction", "[instructions]") {
  std::bitset<16> program_counter_instruction(0x1123);
  test_emu.SetProgramCounter(0);
  REQUIRE(test_emu.GetProgramCounter() == 0);
  test_emu.LoadInstruction(0, program_counter_instruction);
  test_emu.Step();
  REQUIRE(test_emu.GetProgramCounter() == 0x123);
}

TEST_CASE("Testing add value to register instruction", "[instructions]") {

  // Load instruction to add 4 to register 2
  test_emu.LoadInstruction(0, std::bitset<16>(0x7204));
  test_emu.SetProgramCounter(0);

  // Make sure register is set to zero
  test_emu.SetRegister(2, 0);
  
  REQUIRE(test_emu.GetRegister(2) == 0);

  test_emu.Step();
  
  REQUIRE(test_emu.GetRegister(2) == 4);

}

TEST_CASE("Testing set register instruction", "[instructions]") {
  // Load instruction to set register 2 to value 7
  test_emu.LoadInstruction(0, std::bitset<16>(0x6207));
  test_emu.SetProgramCounter(0);
  test_emu.SetRegister(2, 0);
  
  REQUIRE(test_emu.GetRegister(2) == 0);
  
  test_emu.Step();

  REQUIRE(test_emu.GetRegister(2) == 7);
}

TEST_CASE("Testing set index register instruction", "[instructions]") {
  test_emu.LoadInstruction(0, std::bitset<16>(0xA00F));
  test_emu.SetProgramCounter(0);
  
  test_emu.SetIndexRegister(0);

  REQUIRE(test_emu.GetIndexRegister().to_ulong() == 0);

  test_emu.Step();

  REQUIRE(test_emu.GetIndexRegister().to_ulong() == 0xF);
}