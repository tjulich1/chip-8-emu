// Trent Julich ~ 24 March 2021

#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <array>

/**
 * Class that represents the 64x32 pixel chip-8 display matrix. 
 */
class Display {

public:
  Display();

  /**
   * Sets the value of sets the value of the pixel at (p_col, p_row) = p_value.
   */
  void SetPixel(int p_row, int p_col, bool p_value);

  /**
   * Gets and returns the value of the pixel at (p_col, p_row)
   */
  bool GetPixel(int p_row, int p_col);

  /**
   * Outputs the current pixels to the console.
   */
  void Print();

  /**
   * Sets all of the pixels in the pixel buffer to 0.
   */
  void Clear();

  /**
   * Returns true if no pixels are enabled, false otherwise.
   */
  bool IsClear();

  /**
   * Get the number of pixels the display width has.
   */
  int get_pixel_width();
  
  /**
   * Get the number of pixels the display height has.
   */
  int get_pixel_height(); 
private: 

  /**
   * Screen pixel dimensions.
   */
  const static int PIXEL_WIDTH = 64;
  const static int PIXEL_HEIGHT = 32;

  /**
   * 2D array containing the pixel data for the display. A value of true means the pixel should be
   * drawn, otherwise the pixel should not be drawn.
   */
  std::array<std::array<bool, PIXEL_WIDTH>, PIXEL_HEIGHT> pixels_;
};

#endif