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

TEST_CASE("Testing set sound timer instruction", "[instructions]") {
  test_emu.set_program_counter(0);
  test_emu.LoadInstruction(0, std::bitset<16>(0xF018));
  test_emu.set_register(0, 5);
  test_emu.Step();
  REQUIRE(test_emu.get_sound_timer() == 5);
}

TEST_CASE("Testing set delay timer instruction", "[instructions]") {
  test_emu.set_program_counter(0);
  test_emu.LoadInstruction(0, std::bitset<16>(0xF015));
  test_emu.set_register(0, 0x4F);
  test_emu.Step();
  REQUIRE(test_emu.get_delay_timer() == 0x4F);
}

TEST_CASE("Testing skip if equal instruction", "[instructions]") {
  test_emu.set_program_counter(0);
  test_emu.LoadInstruction(0, std::bitset<16>(0x3022));
  test_emu.set_register(0, 0x22);
  test_emu.Step();
  REQUIRE(test_emu.get_program_counter() == 4);
}

TEST_CASE("Testing skip if equal instruction 2", "[instructions]") {
  const int CASES = 1000;

  for (int i = 0; i < CASES; i++) {
    // Generate 2 values between 0x0 and 0xF to give okay chance for values to be equal.
    int val_one = rand() % 0x10;
    std::bitset<8> val_two(rand() % 0x10);

    std::bitset<4> rand_register(rand() % 16);

    test_emu.set_program_counter(0);
    test_emu.set_register(rand_register.to_ulong(), val_one);

    // Expect not to have values be equal, i.e. not to skip instruction.
    int expected_prog_counter = 2;

    if (val_one == val_two.to_ulong()) {
      expected_prog_counter = 4;
    }

    std::bitset<16> instruction("0011" + rand_register.to_string() + val_two.to_string());
    test_emu.LoadInstruction(0, instruction);
    test_emu.Step();

    REQUIRE(test_emu.get_program_counter() == expected_prog_counter);
  }
}

TEST_CASE("Testing store register vy in vx" , "[instructions]") {
  // Load instruction to store register 2's value in register 1
  test_emu.LoadInstruction(0, 0x8120);

  // Clear destination register and give source register values.
  test_emu.set_register(1, 0x0);
  test_emu.set_register(2, 0xA9);

  test_emu.set_program_counter(0);
  test_emu.Step();

  REQUIRE(test_emu.get_register(1) == test_emu.get_register(2));
}

TEST_CASE("Testing store register vx value back to itself", "[instructions]") {
  const int value = 0x4D;

  test_emu.set_register(0, value);

  // Instruction to store register 0 in register 0.
  test_emu.LoadInstruction(0, 0x8000);
  test_emu.set_program_counter(0);
  test_emu.Step();

  // Ensure that the instruction didn't affect value stored.
  REQUIRE(test_emu.get_register(0) == value);
}

TEST_CASE("Testing store register vy in vx 2" , "[instructions]") {
  const int CASES = 1000;
  
  for (int i = 0; i < CASES; i++) {

    // Generate nibbles for src and destination registers.
    std::bitset<4> dest_register(rand() % 16);
    std::bitset<4> src_register(rand() % 16);

    // Build instruction to store src register value in destination register and load.
    std::bitset<16> instruction("1000" + dest_register.to_string() + src_register.to_string() + "0000");
    test_emu.LoadInstruction(0, instruction);
    test_emu.set_program_counter(0);

    // Generate random value between 0x00 and 0xFF and store in source register.
    int value = rand() % 0x100;
    test_emu.set_register(src_register.to_ulong(), value);
    
    test_emu.Step();
    REQUIRE(test_emu.get_register(dest_register.to_ulong()) == value);
  }
}