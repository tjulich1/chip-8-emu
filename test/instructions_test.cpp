#include "catch.hpp"
#include "../src/emu.hpp "

#include <iostream>
#include <random>
#include <chrono>

Emu test_emu;

const int CASES = 1000;

int seed = std::chrono::system_clock::now().time_since_epoch().count();

std::default_random_engine generator(seed);
std::uniform_int_distribution<int> register_distribution(0, 15);

auto random_register = std::bind(register_distribution, generator);

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

TEST_CASE("Testing add value to register instruction 2", "[instructions]") {
  for (int i = 0; i < CASES; i++) {
    // Generate random value to add to register.
    std::bitset<8> value(rand() % 0x50);

    // Pick a random register to add to.
    std::bitset<4> rand_register(random_register());

    // Set the value of the random register.
    int register_start = rand() % 0x50;
    test_emu.set_register(rand_register.to_ulong(), register_start);

    // Instruction to add the random value to the random register.
    std::bitset<16> instruction("0111" + rand_register.to_string() + value.to_string());
    test_emu.LoadInstruction(0, instruction);
    test_emu.set_program_counter(0);
    test_emu.Step();

    REQUIRE(test_emu.get_register(rand_register.to_ulong()) == register_start + value.to_ulong());
  }
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

TEST_CASE("Testing set register instruction 2", "[instructions]") {
  for (int i = 0; i < CASES; i++) {
    std::bitset<8> value(rand() % 0x100);
    std::bitset<4> rand_register(random_register());
    std::bitset<16> instruction("0110" + rand_register.to_string() + value.to_string());
    test_emu.LoadInstruction(0, instruction);
    test_emu.set_program_counter(0);
    test_emu.Step();
    REQUIRE(test_emu.get_register(rand_register.to_ulong()) == value.to_ulong());
  }
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

TEST_CASE("Testing store registers to memory instruction 3", "[instructions]") {
  std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> register_distribution(0, 15);
  std::uniform_int_distribution<int> value_distribution(0, 0xFF);

  for (int i = 0; i < CASES; i++) {
    // Pick a random number of generators to initialize.
    int num_registers = register_distribution(generator);

    std::vector<int> generated_numbers;

    // Generate all the random numbers and store them in registers V0 -> V_num_registers
    for (int i = 0; i <= num_registers; i++) {
      int value = value_distribution(generator);
      test_emu.set_register(i, value);
      generated_numbers.push_back(value);
    }

    test_emu.set_index_register(0x200);
    test_emu.set_program_counter(0);
    test_emu.LoadInstruction(0, 0xF055 + (num_registers << 8));
    test_emu.Step();
    
    for (int i = 0; i <= num_registers; i++) {
      REQUIRE(test_emu.get_memory(0x200 + i) == generated_numbers[i]);
    }

    REQUIRE(test_emu.get_index_register() == 0x200 + num_registers + 1);
  }  
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

TEST_CASE("Testing store binary coded decimal to memory 3", "[instructions]") {
  test_emu.set_index_register(0x10);
  const int index_start = 0x10;

  for (int i = 0; i < CASES; i++) {
    // Pick a random register.
    std::bitset<4> rand_register(random_register());

    // Fill the register with a random value.
    int value = rand() % 0x100;
    test_emu.set_register(rand_register.to_ulong(), value);

    // Calculate each place value of the generated number.
    int hundreds_place = value / 100;
    int tens_place = (value / 10) % 10;
    int ones_place = (value % 100) % 10;

    // Instruction to store binary-coded decimal value in register to memory starting at index.
    std::bitset<16> instruction("1111" + rand_register.to_string() + "00110011");
    test_emu.LoadInstruction(0, instruction);
    test_emu.set_program_counter(0);
    test_emu.Step();

    REQUIRE(test_emu.get_memory(index_start) == hundreds_place);
    REQUIRE(test_emu.get_memory(index_start + 1) == tens_place);
    REQUIRE(test_emu.get_memory(index_start  + 2) == ones_place);
  }
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
  for (int i = 0; i < CASES; i++) {
    // Pick random register and fill with random value.
    std::bitset<4> rand_register(random_register());
    int register_value = rand() % 0x100;
    test_emu.set_register(rand_register.to_ulong() , register_value);

    int index_start = rand() % 0x1000;

    // Fill index register with random value.
    test_emu.set_index_register(index_start);

    // Instruction to add the value from the random register to the index register.
    std::bitset<16> instruction("1111" + rand_register.to_string() + "00011110");
    test_emu.LoadInstruction(0, instruction);
    test_emu.set_program_counter(0);
    test_emu.Step();

    REQUIRE(test_emu.get_index_register().to_ulong() == index_start + register_value);
  }
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
  for (int i = 0; i < CASES; i++) {
    // Generate 2 values between 0x0 and 0xF to give okay chance for values to be equal.
    int val_one = rand() % 0x10;
    std::bitset<8> val_two(rand() % 0x10);

    std::bitset<4> rand_register(random_register());

    test_emu.set_program_counter(0);
    test_emu.set_register(rand_register.to_ulong(), val_one);

    // Expect not to have values be equal, i.e. not to skip instruction (keep PC at 2).
    int expected_prog_counter = 2;

    // If equal, instruction is skipped and PC should be 4.
    if (val_one == val_two.to_ulong()) {
      expected_prog_counter = 4;
    }

    // Build and execute instruction.
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
  for (int i = 0; i < CASES; i++) {

    // Generate nibbles for src and destination registers.
    std::bitset<4> dest_register(random_register());
    std::bitset<4> src_register(random_register());

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

TEST_CASE("Testing jump to register 0 plus value test", "[instructions]") {
  for (int i = 0; i < CASES; i++) {
    int value_to_add = rand() % 0x1000;
    int register_start = rand() % 0x100;
    test_emu.set_register(0, register_start);
    test_emu.set_program_counter(0);
    test_emu.LoadInstruction(0, std::bitset<16>((0xB << 12) + value_to_add));
    test_emu.Step();

    REQUIRE(test_emu.get_program_counter() == register_start + value_to_add);
  }
}

TEST_CASE("Testing read registers from memory instruction 2", "[instructions]") {

  std::uniform_int_distribution<int> random_mem_start(0x200, 4000);

  for (int i = 0; i < CASES; i++) {
    // Pick random number of registers to use.
    int rand_registers = register_distribution(generator);

    // Starting memory address to write random values to.
    int start_mem = random_mem_start(generator);

    std::vector<int> random_values;

    // Initialize memory to random values.
    for (int i = 0; i < rand_registers; i++) {
      random_values.push_back(rand() % 0x100);
      test_emu.set_memory(start_mem + i, random_values[i]);
    }

    // Set index register to beginning of memory segment to read into registers.
    test_emu.set_index_register(start_mem);

    std::bitset<4> register_nibble(rand_registers);

    test_emu.LoadInstruction(0, std::bitset<16>("1111" + register_nibble.to_string() + "01100101"));
    test_emu.set_program_counter(0);
    test_emu.Step();

    REQUIRE(test_emu.get_index_register().to_ulong() == start_mem + rand_registers + 1);
    
    for (int i = 0; i < rand_registers; i++) {
      REQUIRE(test_emu.get_register(i) == random_values[i]);
    }
  }
}

TEST_CASE("Testing read registers from memory with out of bounds values", "[instructions]") {
  test_emu.set_index_register(4095);
  test_emu.set_memory(4095, 0xFF);
  test_emu.set_program_counter(0);
  test_emu.LoadInstruction(0, 0xF465);
  test_emu.Step();
  REQUIRE(test_emu.get_register(0) == 0xFF);
  REQUIRE(test_emu.get_register(1) == 0x00);
  REQUIRE(test_emu.get_register(2) == 0x00);
  REQUIRE(test_emu.get_register(3) == 0x00);
}

TEST_CASE("Testing store delay timer instruction", "[instructions]") {
  test_emu.set_program_counter(0);
  test_emu.LoadInstruction(0, 0xF107);
  test_emu.set_delay_timer(60);
  test_emu.Step();

  REQUIRE(test_emu.get_register(1) == 60);
}

TEST_CASE("Testing skip next instruction if registers are not equal", "[instructions]") {
  for (int i = 0; i < CASES; i++) {
    int value_one = rand() % 0x10;
    int value_two = rand() % 0x10;
    int register_one = random_register();
    int register_two = random_register();

    test_emu.set_register(register_one, value_one);
    test_emu.set_register(register_two, value_two);
    test_emu.set_program_counter(0);
    std::bitset<4> second_nibble(register_one);
    std::bitset<4> third_nibble(register_two);
    test_emu.LoadInstruction(0, std::bitset<16>("1001" + second_nibble.to_string() 
      + third_nibble.to_string() + "0000"));
    test_emu.Step();
    if (test_emu.get_register(register_one) == test_emu.get_register(register_two)) {
      REQUIRE(test_emu.get_program_counter() == 0x2);
    } else {
      REQUIRE(test_emu.get_program_counter() == 0x4);
    }
  }
}

TEST_CASE("Testing shift register left instruction", "[instructions]") {
  for (int i = 0; i < CASES; i++) {
    std::bitset<4> dest_register(rand() % 15);
    std::bitset<4> src_register(rand() % 15);
    int start_value = rand() % 0x100;
    test_emu.set_register(src_register.to_ulong(), start_value);
    int least_sig = std::bitset<8>(start_value)[0];
    test_emu.set_program_counter(0);
    std::bitset<16> instruction = std::bitset<16>("1000" + dest_register.to_string() 
      + src_register.to_string() + "1110"); 
    test_emu.LoadInstruction(0, instruction);
    test_emu.Step();
    REQUIRE(test_emu.get_register(0xF) == least_sig);
    REQUIRE(test_emu.get_register(dest_register.to_ulong()) == std::bitset<8>(start_value << 1).to_ulong());
  }
}

TEST_CASE("Testing execute subroutine at address", "[instructions]") {
  
  test_emu.set_register(0, 0);

  // Load instruction to add value 0x14 to register 0. This is a tiny "subroutine" to jump to.
  test_emu.LoadInstruction(0xE, 0x7014);

  // Load instruction to execute subroutine at address 0xE.
  test_emu.LoadInstruction(0x0, 0x200E);

  test_emu.set_program_counter(0);
  test_emu.Step();
  test_emu.Step();

  REQUIRE(test_emu.get_register(0) == 0x14);
  REQUIRE(test_emu.get_program_counter() == 0x10);

}

TEST_CASE("Testing return from subrouting instruction", "[instructions]") { 
  test_emu.set_program_counter(0);

  // Instruction to add value 0x14 to register 0. Used as a subroutine at memory location 0xE
  test_emu.LoadInstruction(0xE, 0x7014);

  //  Instruction to return from subroutine.
  test_emu.LoadInstruction(0x10, 0x00EE);

  // Instruction to jump to subroutine. 
  test_emu.LoadInstruction(0x0, 0x200E);

  test_emu.Step();
  test_emu.Step();
  test_emu.Step();

  REQUIRE(test_emu.get_program_counter() == 0x2);
}

TEST_CASE("Testing set sprite memory address instruction", "[instructions]") {
  test_emu.set_program_counter(0);
  
  test_emu.set_register(0, 0x0);
  test_emu.set_register(1, 0x4);
  test_emu.LoadInstruction(0, 0xF029);
  test_emu.LoadInstruction(2, 0xF129);

  test_emu.Step();

  REQUIRE(test_emu.get_index_register().to_ulong() == 0x50);

  test_emu.Step();

  REQUIRE(test_emu.get_index_register().to_ulong() == 0x54);

}

TEST_CASE("Testing skip following if register not equal", "[instructions]") {
  for (int i = 0; i < CASES; i++) {
    int register_num = random_register();
    int register_value = rand() % 0x100;
    int random_value = rand() % 0x100;
    test_emu.set_register(register_num, register_value);
    test_emu.set_program_counter(0);
    std::bitset<16> instruction((0x4<<12) + (register_num<<8) + random_value);
    test_emu.LoadInstruction(0, instruction);
    test_emu.Step();

    if (register_value == random_value) {
      REQUIRE(test_emu.get_program_counter() == 0x2);
    } else {
      REQUIRE(test_emu.get_program_counter() == 0x4);
    }

  }
}