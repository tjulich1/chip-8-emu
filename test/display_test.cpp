// Trent Julich ~ 24 March 2021

#include "catch.hpp"
#include "../src/display.hpp"

#include <vector>

Display test_display;

TEST_CASE("Testing default display values", "[hardware]") {
  for (int i = 0; i < test_display.get_pixel_width(); i++) {
    for (int j = 0; j < test_display.get_pixel_height(); j++) {
      REQUIRE(test_display.GetPixel(j, i) == 0);
    }
  }
}

TEST_CASE("Testing several display set pixel calls", "[hardware]") {
  const int test_cases = 100;
  for (int i = 0; i < test_cases; i++) {
    int row = rand() % test_display.get_pixel_height();
    int col = rand() % test_display.get_pixel_width();
    bool value = rand() % 2;
    test_display.SetPixel(row, col, value);
    REQUIRE(test_display.GetPixel(row, col) == value);
  }
}

TEST_CASE("Testing the clear display method and is clear method", "[hardware]") {
  test_display.SetPixel(0, 0, 1);
  REQUIRE(!test_display.IsClear());
  test_display.Clear();
  REQUIRE(test_display.IsClear());
  test_display.SetPixel(2, 3, 1);
  test_display.SetPixel(27, 2, 1);
  REQUIRE(!test_display.IsClear());
  test_display.Clear();
  REQUIRE(test_display.IsClear()); 
}