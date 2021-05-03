// Trent Julich ~ 25 April 2021

#include "var_register_panel.hpp"
#include <sstream>

VariableRegisterPanel::VariableRegisterPanel(int p_x, int p_y, int p_width, int p_height, 
  std::array<Register<8>, 16>* p_registers, FontAtlas* p_font_atlas) : Panel(p_x, p_y, p_width, p_height) {
  registers_ = p_registers;
  font_atlas_ = p_font_atlas;
}

void VariableRegisterPanel::Render(SDL_Renderer* p_renderer) {
  SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
  SDL_Rect bounds{
    x_ + 10, y_ + 10, width_, height_
  };
  SDL_RenderFillRect(p_renderer, &bounds);

  int line_height = font_atlas_->get_line_height();
  for (int i = 0; i < registers_->size(); i++) {
    std::stringstream ss;
    ss << 'V' << std::hex << i << ":" << registers_->at(i).Read().to_ulong();
    font_atlas_->RenderText(ss.str(), bounds.x, bounds.y);
    if (i % 2 == 0) {
      bounds.x += width_ / 2;
    } else {
      bounds.x -= width_ / 2;
      bounds.y += line_height;
    }
  }
}