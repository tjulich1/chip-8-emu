// Trent Julich ~ 25 April 2021

#ifndef REG_PANEL_HPP
#define REG_PANEL_HPP

#include "panel.hpp"
#include "register.hpp"

#include <array>

class VariableRegisterPanel : public Panel {
  public:
    VariableRegisterPanel(int p_x, int p_y, int p_width, int p_height, std::array<Register<8>, 16>* p_registers);

    void Render(SDL_Renderer* p_renderer);

  private: 
    std::array<Register<8>, 16>* registers_;
};

#endif