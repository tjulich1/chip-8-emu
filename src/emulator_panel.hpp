// Trent Julich ~ 24 April 2021

#ifndef EMU_PANEL_HPP
#define EMU_PANEL_HPP

#include "emu.hpp"
#include "panel.hpp"

#include <SDL_ttf.h>
#include <iostream>

class EmulatorPanel : public Panel {
  public:
    EmulatorPanel(int p_x, int p_y, int p_width, int p_height, Emu* p_emulator);

    void Render(SDL_Renderer* p_renderer);

  private:
    Emu* emulator_;
};

#endif