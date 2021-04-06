// Trent Julich ~ 4 April 2021

#ifndef KEYBOARD_INPUT_HPP
#define KEYBOARD_INPUT_HPP

#include <SDL.h>
#include <iostream>
#include <map>
#include <array>

/**
 * Class designed to keep track of keyboard input for chip-8 simulator. Keeps track of the state of 
 * a 4x4 hex keypad, mapping to keys 1-4, q-r, a-f, z-v, on a qwerty keyboard.
 */
class KeyboardInput {

public:

  KeyboardInput();

  /**
   * Checks if given scancode is mapped and if so sets the mapped key state to 1 (or pressed).
   */
  void HandleKeyDown(SDL_Scancode p_code);

  /**
   * Checks if given scancode is mapped and if so, sets the mapped key state to 0 (or released).
   */
  void HandleKeyUp(SDL_Scancode p_code);

  /**
   * Checks if given scancode is mapped and if so returns true if mapped key is down, false 
   * otherwise. Method also returns false if key does not have a mapping.
   */
  bool IsKeyDown(SDL_Scancode p_code);

private:

  /**
   * Map of scan codes to values of the keys.
   */
  std::map<SDL_Scancode, int> key_mappings_;
  
  /**
   * Boolean array keeping track of pressed or released keys in hex keypad.
   */
  std::array<bool, 16> keys_;

  /**
   * Helper method used to set the state of a given key (0 -> 0xF).
   */
  void SetKey(int p_key, bool p_value);
};

#endif