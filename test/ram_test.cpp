// Trent Julich ~ 23 March 2021

#include "catch.hpp"
#include "../src/ram.hpp"

Ram test_ram;

TEST_CASE("Testing ram read write", "[hardware]") {
  int test_address = 0;
  std::bitset<8> write_value("00010001");
  test_ram.Write(test_address, write_value);
  REQUIRE(write_value.to_ulong() == test_ram.Read(test_address).to_ulong());
  write_value = std::bitset<8>("10010101");
  test_address = 100;
  test_ram.Write(test_address, write_value);
  REQUIRE(write_value.to_ulong() == test_ram.Read(test_address).to_ulong());
}

TEST_CASE("Testing bad Read addresses", "[hardware]") {
  int bad_address = -10;
  std::bitset<8> expected_value("00000000");
  REQUIRE(test_ram.Read(bad_address).to_ulong() == expected_value.to_ulong());
}