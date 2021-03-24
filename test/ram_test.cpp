// Trent Julich ~ 23 March 2021

#include "catch.hpp"
#include "../src/ram.hpp"

Ram test_ram;

TEST_CASE("Testing ram read write", "[hardware]") {
  int test_address = 0;
  std::array<bool, 8> write_value = {0, 0, 0, 1, 0, 0, 0, 1};
  test_ram.Write(test_address, write_value);
  REQUIRE(write_value == test_ram.Read(test_address));
  write_value = {1, 0, 0, 1, 0, 1, 0, 1};
  test_address = 100;
  test_ram.Write(test_address, write_value);
  REQUIRE(write_value == test_ram.Read(test_address));
}

TEST_CASE("Testing bad Read addresses", "[hardware]") {
  int bad_address = -10;
  std::array<bool, 8> expected_value = {0, 0, 0, 0, 0, 0, 0, 0};
  REQUIRE(test_ram.Read(bad_address) == expected_value);
}