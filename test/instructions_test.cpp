#include "catch.hpp"
#include "../src/emu.hpp "

#include <iostream>

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
  test_emu.set_program_counter(0);

  // Clear screen
  test_emu.Step();
  
  REQUIRE(main_display.IsClear());
}

TEST_CASE("Testing jump instruction", "[instructions]") {
  std::bitset<16> program_counter_instruction(0x1123);
  test_emu.set_program_counter(0);
  REQUIRE(test_emu.get_program_counter() == 0);
  test_emu.LoadInstruction(0, program_counter_instruction);
  test_emu.Step();
  REQUIRE(test_emu.get_program_counter() == 0x123);
  test_emu.LoadInstruction(0x123, std::bitset<16>(0x1234));
  test_emu.Step();
  REQUIRE(test_emu.get_program_counter() == 0x234);
}

TEST_CASE("Testing add value to register instruction", "[instructions]") {
  // Load instruction to add 4 to register 2
  test_emu.LoadInstruction(0, std::bitset<16>(0x7204));
  test_emu.set_program_counter(0);

  // Make sure register is set to zero
  test_emu.set_register(2, 0);

  REQUIRE(test_emu.get_register(2) == 0);
  test_emu.Step();
  REQUIRE(test_emu.get_register(2) == 4);
}

TEST_CASE("Testing add value to register instruction overflow", "[instructions]") {
  test_emu.set_register(0, 0xFF);
  REQUIRE(test_emu.get_register(0) == 0xFF);
  test_emu.LoadInstruction(0, std::bitset<16>(0x7001));
  test_emu.set_program_counter(0);
  test_emu.Step();
  REQUIRE(test_emu.get_register(0) == 0x0);
}

TEST_CASE("Testing set register instruction", "[instructions]") {
  // Load instruction to set register 2 to value 7
  test_emu.LoadInstruction(0, std::bitset<16>(0x6207));
  test_emu.set_program_counter(0);
  test_emu.set_register(2, 0);
  
  REQUIRE(test_emu.get_register(2) == 0);
  
  test_emu.Step();

  REQUIRE(test_emu.get_register(2) == 7);
}

TEST_CASE("Testing set index register instruction", "[instructions]") {
  test_emu.LoadInstruction(0, std::bitset<16>(0xA00F));
  test_emu.set_program_counter(0);
  
  test_emu.set_index_register(0);

  REQUIRE(test_emu.get_index_register().to_ulong() == 0);

  test_emu.Step();

  REQUIRE(test_emu.get_index_register().to_ulong() == 0xF);
}

TEST_CASE("Testing save registers to memory instruction", "[instructions]") {
  // Instruction to save registers V0 -> V2 to memory.
  test_emu.LoadInstruction(0, std::bitset<16>(0xF255));

  // Set the values of those registers.
  test_emu.set_register(0, 0x11);
  test_emu.set_register(1, 0x23);
  test_emu.set_register(2, 0x67);

  test_emu.set_program_counter(0);
  test_emu.set_index_register(0xF);

  REQUIRE(test_emu.get_register(0) == 0x11);
  REQUIRE(test_emu.get_register(1) == 0x23);
  REQUIRE(test_emu.get_register(2) == 0x67);

  test_emu.Step();

  REQUIRE(test_emu.get_memory(0xF) == 0x11);
  REQUIRE(test_emu.get_memory(0x10) == 0x23);
  REQUIRE(test_emu.get_memory(0x11) == 0x67);

  REQUIRE(test_emu.get_index_register().to_ulong() == 0x12);
}

TEST_CASE("Testing save registers to memory instruction 2", "[instructions]") {
  std::vector<int> register_values;
  for (int i = 0; i <= 0xF; i++) {
    register_values.push_back(rand() % 0xFF);
    test_emu.set_register(i, register_values[i]);
  }

  int mem_start = 0xF;

  test_emu.LoadInstruction(0, std::bitset<16>(0xFF55));
  test_emu.set_index_register(mem_start);
  test_emu.set_program_counter(0);
  test_emu.Step();

  for (int i = 0; i <= 0xF; i++) {
    REQUIRE(test_emu.get_memory(mem_start + i) == register_values[i]);
  }

  REQUIRE(test_emu.get_index_register() == (mem_start + 0xF + 1)); 
}

TEST_CASE("Testing read mem to registers instruction", "[instructions]") {
  int mem_start = 0xF;

  // Generate and store random bytes in memory from address 0xF -> 0x1E
  std::vector<int> random_byte_values;
  for (int i = 0; i <= 0xF; i++) {
    random_byte_values.push_back(rand() % 0xFF);
    test_emu.set_memory(mem_start + i, random_byte_values[i]);
  }

  // Set up emulator to read bytes into registers V0 -> VF
  test_emu.set_index_register(mem_start);
  test_emu.set_program_counter(0);
  test_emu.LoadInstruction(0, std::bitset<16>(0xFF65));
  test_emu.Step();

  // Ensure that the register successfully read in the correct bytes.
  for (int i = 0; i <= 0xF; i++) {
    REQUIRE(test_emu.get_register(i) == random_byte_values[i]);
  }

  // Also ensure the index register was updated correctly.
  REQUIRE(test_emu.get_index_register() == mem_start + 0xF + 1);
}

TEST_CASE("Testing store binary coded decimal to memory", "[instructions]") {
  test_emu.set_program_counter(0);
  test_emu.set_index_register(0xF);

  test_emu.set_register(0, 0xFF);

  // Instruction to store binary coded decimal value in V0 to memory.
  test_emu.LoadInstruction(0, std::bitset<16>(0xF033));
  test_emu.Step();

  REQUIRE(test_emu.get_memory(0xF) == 0x2);
  REQUIRE(test_emu.get_memory(0xF + 1) == 0x5);
  REQUIRE(test_emu.get_memory(0xF + 2) == 0x5);
}

TEST_CASE("Testing store binary coded decimal to memory 2", "[instructions]") {
  test_emu.set_program_counter(0);
  test_emu.set_index_register(0xF);

  test_emu.set_register(0, 0xF);

  // Instruction to store binary coded decimal value in V0 to memory.
  test_emu.LoadInstruction(0, std::bitset<16>(0xF033));
  test_emu.Step();

  REQUIRE(test_emu.get_memory(0xF) == 0x0);
  REQUIRE(test_emu.get_memory(0xF + 1) == 0x1);
  REQUIRE(test_emu.get_memory(0xF + 2) == 0x5);
}

TEST_CASE("Testing add register to index instruction", "[instructions]") {
  test_emu.set_index_register(0x234);
  test_emu.set_register(0, 0x45);
  
  test_emu.set_program_counter(0);
  test_emu.LoadInstruction(0, std::bitset<16>(0xF01E));
  test_emu.Step();

  REQUIRE(test_emu.get_index_register().to_ulong() == 0x279);
}

TEST_CASE("Testing add register to index instruction 2", "[instructions]") {
  test_emu.set_index_register(0x4567);
  test_emu.set_register(0, 0x0);
  
  test_emu.set_program_counter(0);
  test_emu.LoadInstruction(0, std::bitset<16>(0xF01E));
  test_emu.Step();

  REQUIRE(test_emu.get_index_register().to_ulong() == 0x4567);
}