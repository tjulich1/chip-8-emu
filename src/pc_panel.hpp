// Trent Julich ~ 5 May 2021

#ifndef PC_PANEL_HPP
#define PC_PANEL_HPP

#include "font_atlas.hpp"
#include "panel.hpp"
#include "emu.hpp"

/**
 * Panel which is used to render the program counter of the chip-8 emulator during execution.
 */
class ProgramCounterPanel : public Panel {
  public:
    /**
     * Constructor that takes location and dimension information, as well as a font atlas used to 
     * render text and a program counter pointer to render during execution.
     */
    ProgramCounterPanel(int p_x, int p_y, int p_width, int p_height, FontAtlas* p_font_atlas, 
      Emu* p_emu);

    /**
     * Called to render the current value of the program counter to the screen.
     */
    void Render(SDL_Renderer* p_renderer);

  private:

    /**
     * Font atlas used to render text.
     */
    FontAtlas* font_atlas_;

    /**
     * Pointer to emulator program counter that should be rendered.
     */
    Emu* emu_;
};

#endif