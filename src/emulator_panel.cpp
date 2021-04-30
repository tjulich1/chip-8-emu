// Trent Julich 25 April 2021

#include "emulator_panel.hpp"

EmulatorPanel::EmulatorPanel(int p_x, int p_y, int p_width, int p_height, Emu* p_emulator) 
      : Panel(p_x, p_y, p_width, p_height) {
      emulator_ = p_emulator;
    };

void EmulatorPanel::Render(SDL_Renderer* p_renderer) {
  emulator_->Render();
}