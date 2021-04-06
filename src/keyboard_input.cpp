// Trent Julich ~ 5 April 2021

#include "keyboard_input.hpp"

#include <vector>

KeyboardInput::KeyboardInput() {

  // SDL scan codes that will map to keypad hex values.
  std::vector<SDL_Scancode> codes{
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_Q, SDL_SCANCODE_W, 
    SDL_SCANCODE_E, SDL_SCANCODE_R, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, 
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
  };

  // The keypad values that can be used as input. 
  std::vector<int> values{
    1,   2, 3,   0xC, 
    4,   5, 6,   0xD,
    7,   8, 9,   0xE, 
    0xA, 0, 0xB, 0xF
  };

  // Map the keyboard scan codes to the hex values.
  for (int i = 0; i < codes.size(); i++) {
    key_mappings_.insert(std::pair<SDL_Scancode, int>(codes[i], values[i]));
  }
}

void KeyboardInput::HandleKeyDown(SDL_Scancode p_code) {
  if (key_mappings_.find(p_code) != key_mappings_.end()) {
    keys_[key_mappings_.find(p_code)->second] = 1;
  }
} 

void KeyboardInput::HandleKeyUp(SDL_Scancode p_code) {
  if (key_mappings_.find(p_code) != key_mappings_.end()) {
    keys_[key_mappings_.find(p_code)->second] = 0;
  }
}

bool KeyboardInput::IsKeyDown(SDL_Scancode p_code) {
  if (key_mappings_.find(p_code) != key_mappings_.end()) {
    return keys_[key_mappings_.find(p_code)->second];
  } 
  return 0;
}

void KeyboardInput::SetKey(int p_key, bool p_value) {
  if (p_key >= 0 && p_key <= 0xF) {
    keys_[p_key] = p_value;
  }
}