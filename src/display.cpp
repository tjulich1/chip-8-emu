// Trent Julich ~ 24 March 2021

#include "display.hpp"

#include <iostream>

Display::Display() {
  for (int i = 0; i < pixels_.size(); i++) {
    pixels_[i].fill(0);
  }
}

void Display::SetPixel(int p_row, int p_col, bool p_value) {
  if (!(p_row < 0 || p_row > PIXEL_HEIGHT || p_col < 0 || p_col > PIXEL_WIDTH)) {
    pixels_[p_row][p_col] = p_value;
  }
}

bool Display::GetPixel(int p_row, int p_col) {
  bool return_val = false;
  if (!(p_row < 0 || p_col < 0 || p_row >= PIXEL_HEIGHT || p_col >= PIXEL_WIDTH)) {
    return_val = pixels_[p_row][p_col];
  }
  return return_val;
}

int Display::get_pixel_width() {
  return PIXEL_WIDTH;
}

int Display::get_pixel_height() {
  return PIXEL_HEIGHT;
}

void Display::Print() {
  std::cout << "WIDTH: " << std::dec << PIXEL_WIDTH << std::endl;
  std::cout << "HEIGHT: " << std::dec << PIXEL_HEIGHT << std::endl;
  for (int i = 0; i < PIXEL_HEIGHT; i++) {
    std::array<bool, PIXEL_WIDTH> current_row = pixels_[i];
    for (int j = 0; j < PIXEL_WIDTH; j++) {
      std::cout << current_row[j];
    }
    std::cout << std::endl;
  }
}

void Display::Clear() {
  for (int i = 0; i < PIXEL_HEIGHT; i++) {
    for (int j = 0; j < PIXEL_WIDTH; j++) {
      SetPixel(i, j, 0);
    }
  }
}

bool Display::IsClear() {
  bool is_clear = true;
  for (int i = 0; i < pixels_.size(); i++) {
    for (int j = 0; j < pixels_[i].size(); j++) {
      if (pixels_[i][j]) {
        is_clear = false;
        break;
      }
    }
    if (!is_clear) {
      break;
    }
  }
  return is_clear;
}