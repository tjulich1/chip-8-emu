// Trent Julich ~ 24 March 2021

#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <array>

class Display {

public:
  Display();
  void SetPixel(int p_row, int p_col, bool p_value);
  bool GetPixel(int p_row, int p_col);

  void Print();
  void Clear();

  int get_pixel_width();
  int get_pixel_height(); 
private: 
  const static int PIXEL_WIDTH = 64;
  const static int PIXEL_HEIGHT = 32;
  std::array<std::array<bool, PIXEL_WIDTH>, PIXEL_HEIGHT> pixels_;
};

#endif