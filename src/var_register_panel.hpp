// Trent Julich ~ 25 April 2021

#ifndef REG_PANEL_HPP
#define REG_PANEL_HPP

#include "font_atlas.hpp"
#include "panel.hpp"
#include "register.hpp"

#include <array>

/**
 * A panel which displays the names and values of variable registers V0 -> VF.
 */
class VariableRegisterPanel : public Panel {
  public:

    /**
     * Constructor that takes x, y, width, height, a pointer to an array of registers to render the 
     * values of and a FontAtlas used to render the text to the screen.
     */
    VariableRegisterPanel(int p_x, int p_y, int p_width, int p_height,
      std::array<Register<8>, 16>* p_registers, FontAtlas* p_font_atlas);

    /**
     * Called to render this panel and the contents of the registers.
     */
    void Render(SDL_Renderer* p_renderer);

  private: 
    
    /**
     * Font atlas used to render text.
     */
    FontAtlas* font_atlas_;

    /**
     * Pointer to array of registers that are to be rendered to the screen.
     */
    std::array<Register<8>, 16>* registers_;
};

#endif