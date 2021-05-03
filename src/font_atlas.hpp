// Trent Julich ~ 29 April 2021

#ifndef FONT_ATLAS_HPP
#define FONT_ATLAS_HPP

#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class FontAtlas {
  public:

    /**
     * Constructor for new FontAtlas, which preloads a texture containing p_characters rendered using
     * the font located at p_font_path. 
     */
    FontAtlas(std::string p_font_path, int p_font_size, std::string p_characters,
      SDL_Renderer* p_renderer);

    /**
     * Method used to render the string p_text at x, y.
     */
    bool RenderText(std::string p_text, int p_x, int p_y);

  private: 

    /**
    * Map of characters to bounding boxes, where the bounding boxes contain info about the characters
    * location in the font atlas.
    */
    std::map<char, SDL_Rect*> clipping_rects_;
    
    /**
     * Texture containing all initialized characters rendered using the font passed into 
     * construction.
     */
    SDL_Texture* font_atlas_;

    /**
     * The renderer to
     */
    SDL_Renderer* renderer_;

    /**
     * Calculates the clipping box around each individual character in p_characters if they were 
     * rendered in p_font, and stores these clipping boxes in the clipping_rects_ map.
     */
    void CalculateClippingRects(TTF_Font* p_font, std::string p_characters);
};

#endif