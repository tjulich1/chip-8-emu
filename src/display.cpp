// Trent Julich ~ 24 March 2021

#include "display.hpp"

Display::Display() {
  for (int i = 0; i < pixels_.size(); i++) {
    pixels_[i].fill(0);
  }
}

void Display::SetPixel(int p_row, int p_col, bool p_value) {
  if (!(p_row < 0 || p_row > PIXEL_HEIGHT || p_col < 0 || p_col > PIXEL_WIDTH)) {
    pixels_[p_col][p_row] = p_value;
  }
}

bool Display::GetPixel(int p_row, int p_col) {
  bool return_val = false;
  if (!(p_row < 0 || p_col < 0 || p_row >= PIXEL_HEIGHT || p_col >= PIXEL_WIDTH)) {
    return_val = pixels_[p_col][p_row];
  }
  return return_val;
}

int Display::get_pixel_width() {
  return PIXEL_WIDTH;
}

int Display::get_pixel_height() {
  return PIXEL_HEIGHT;
}