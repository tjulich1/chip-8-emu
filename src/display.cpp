// Trent Julich ~ 24 March 2021

#include "display.hpp"

#include <iostream>

Display::Display() {
  for (int i = 0; i < pixels_.size(); i++) {
    pixels_[i].fill(0);
  }
}

Display::Display(SDL_Renderer* p_renderer) {
  renderer_ = p_renderer;
  for (int i = 0; i < pixels_.size(); i++) {
    pixels_[i].fill(0);
  }
}

void Display::Render() {
  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
  for (int i = 0; i < pixels_.size(); i++) {
    std::array<bool, PIXEL_WIDTH> current_row = pixels_[i];
    for (int j = 0; j < PIXEL_WIDTH; j++) {
      if (current_row[j]) {
        SDL_Rect rect{
          j*10,
          i*10,
          10,
          10
        };
        SDL_RenderFillRect(renderer_, &rect);
      }
    }
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
  for (auto row : pixels_) {
    for (bool value : row) {
      std::cout << value;
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
      // If a single pixel is found in any row, display isn't clear.
      if (pixels_[i][j]) {
        is_clear = false;
        break;
      }
    }
    // This is true only if a pixel has been found to be enabled, so stop searching and break.
    if (!is_clear) {
      break;
    }
  }
  return is_clear;
}