// Trent Julich ~ 24 April 2021

#include "emu.hpp"
#include "panel.hpp"

#include <iostream>

class EmulatorPanel : Panel {
  public:

    EmulatorPanel(int p_x, int p_y, int p_width, int p_height, Emu* p_emulator) 
      : Panel(p_x, p_y, p_width, p_height) {
      emulator_ = p_emulator;
    };

    void Render(SDL_Renderer* p_renderer) {
      SDL_SetRenderDrawColor(p_renderer, 255, 0, 0, 255);
      SDL_Rect bounds{
        x_, y_, width_, height_
      };
      SDL_RenderFillRect(p_renderer, &bounds);
      SDL_RenderPresent(p_renderer);
    }

  private:
    Emu* emulator_;
};