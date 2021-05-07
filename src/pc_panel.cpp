// Trent Julich ~ 5 May 2021

#include "pc_panel.hpp"

ProgramCounterPanel::ProgramCounterPanel(int p_x, int p_y, int p_width, int p_height,
  FontAtlas* p_font_atlas, Emu* p_emu) : Panel(p_x, p_y, p_width, p_height) {
  emu_ = p_emu;
  font_atlas_ = p_font_atlas;
}

void ProgramCounterPanel::Render(SDL_Renderer* p_renderer) {
  SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
  SDL_Rect rect{
    x_, y_, width_, height_
  };
  SDL_RenderFillRect(p_renderer, &rect);
  int instruction = emu_->get_memory(emu_->get_program_counter());
  std::string text = "Pc: ";
  text += std::to_string(instruction);
  font_atlas_->RenderText(text, x_, y_);
}