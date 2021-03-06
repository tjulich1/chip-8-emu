#include "font_atlas.hpp"

#include <iostream>
#include <exception>

FontAtlas::FontAtlas(std::string p_font_path, int p_font_size, std::string p_characters,
  SDL_Renderer* p_renderer) {

  renderer_ = p_renderer;
  TTF_Font* font = TTF_OpenFont(p_font_path.c_str(), p_font_size);

  SDL_Color fg_color{
    255, 255, 255
  };

  SDL_Color bg_color{
    0, 0, 0
  };

  SDL_Surface* font_surface = TTF_RenderText_Shaded(font, p_characters.c_str(), fg_color, bg_color);
  font_atlas_ = SDL_CreateTextureFromSurface(renderer_, font_surface);
  SDL_FreeSurface(font_surface);
  CalculateClippingRects(font, p_characters);

  TTF_CloseFont(font);
}

bool FontAtlas::RenderText(std::string p_text, int p_x, int p_y) {
  bool success = true;

  // Ensure that all the characters in p_text are loaded in the map (therefore able to be rendererd).
  for (int i = 0; i < p_text.size(); i++) {
    if (clipping_rects_.find(p_text[i]) == clipping_rects_.end()) {
      success = false;
      std::cout << "Unknown character: " << p_text[i] << std::endl;
      break;
    }
  }

  if (success) {
    SDL_Rect dest_rect{
      p_x, p_y, 0, 0
    };

    SDL_Rect* src_rect;

    for (int i = 0; i < p_text.size(); i++) {
      src_rect = clipping_rects_.find(p_text[i])->second;
      dest_rect.w = src_rect->w;
      dest_rect.h = src_rect->h;
      SDL_RenderCopy(renderer_, font_atlas_, src_rect, &dest_rect);
      dest_rect.x += dest_rect.w;
      if (p_text[i] == ':') {
        dest_rect.x += 5;
      }
    }
  }
  return success;
}

int FontAtlas::get_line_height() {
  return line_height_;
}

void FontAtlas::CalculateClippingRects(TTF_Font* p_font, std::string p_characters) {
  int clip_height = TTF_FontAscent(p_font) - TTF_FontDescent(p_font);
  line_height_ = clip_height;
  int x = 0;
  int advance;
  for (int i = 0; i < p_characters.size(); i++) {
    TTF_GlyphMetrics(p_font, p_characters[i], nullptr, nullptr, nullptr, nullptr, &advance);
    SDL_Rect temp{
      x, 0, advance, clip_height
    };

    SDL_Rect* dynam_temp = new SDL_Rect(temp);

    clipping_rects_.insert(std::pair<char, SDL_Rect*>(p_characters[i], dynam_temp));
    x += advance;
  }
}
