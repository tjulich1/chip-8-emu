// Trent Julich ~ 25 April 2021

#include "var_register_panel.hpp"

VariableRegisterPanel::VariableRegisterPanel(int p_x, int p_y, int p_width, int p_height, 
  std::array<Register<8>, 16>* p_registers, FontAtlas* p_font_atlas) : Panel(p_x, p_y, p_width, p_height) {
  registers_ = p_registers;
}

void VariableRegisterPanel::Render(SDL_Renderer* p_renderer) {
  SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
  SDL_Rect bounds{
    x_, y_, width_, height_
  };
  SDL_RenderFillRect(p_renderer, &bounds);
}