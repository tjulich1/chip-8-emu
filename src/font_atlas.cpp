#include "font_atlas.hpp"

FontAtlas::FontAtlas(std::string p_font_path, int p_font_size, std::string p_characters,
  SDL_Renderer* p_renderer) {

  renderer_ = p_renderer;
  TTF_Font* font = TTF_OpenFont(p_font_path.c_str(), p_font_size);

  SDL_Color bg_color{
    255, 255, 255
  };

  SDL_Color fg_color{
    0, 0, 0
  };

  SDL_Surface* font_surface = TTF_RenderText_Shaded(font, p_characters.c_str(), fg_color, bg_color);
  font_atlas_ = SDL_CreateTextureFromSurface(renderer_, font_surface);
  SDL_FreeSurface(font_surface);
  CalculateClippingRects(font, p_characters);
  TTF_CloseFont(font);
}

bool FontAtlas::RenderText(std::string p_text, SDL_Renderer* p_renderer, SDL_Rect* p_dest_rect) {
  return true;
}

void FontAtlas::CalculateClippingRects(TTF_Font* p_font, std::string p_characters) {
  int clip_height = TTF_FontAscent(p_font) - TTF_FontDescent(p_font);
  int x = 0;
  int advance;
  for (int i = 0; i < p_characters.size(); i++) {
    TTF_GlyphMetrics(p_font, p_characters[i], nullptr, nullptr, nullptr, nullptr, &advance);
    clipping_rects_.emplace(std::pair<char, SDL_Rect>(p_characters[i], SDL_Rect{
      x, 0, advance, clip_height
    }));
    x += advance;
  }
}
