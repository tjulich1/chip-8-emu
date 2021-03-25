#include "catch.hpp"
#include "../src/register.hpp"
#include <array>

Register<8> test_register;
Register<16> test_register_two;

TEST_CASE("Testing register read and write", "[hardware]") {
  std::array<bool, 8> value;
  std::array<bool, 16> value_two;
  const int test_cases = 100;
  for (int i = 0; i < test_cases; i++) {
    for (int j = 0; j < value.size(); j++) {
      value[j] = rand() % 2;
    }
    for (int j = 0; j < value_two.size(); j++) {
      value_two[j] = rand() % 2;
    }
    test_register.Write(value);
    test_register_two.Write(value_two);
    REQUIRE(test_register.Read() == value);
    REQUIRE(test_register_two.Read() == value_two);
  }
}