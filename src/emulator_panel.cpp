#include "emulator_panel.hpp"

EmulatorPanel::EmulatorPanel(int p_x, int p_y, int p_width, int p_height, Emu* p_emulator) 
      : Panel(p_x, p_y, p_width, p_height) {
      emulator_ = p_emulator;
    };

void EmulatorPanel::Render(SDL_Renderer* p_renderer) {
  TTF_Font* font = TTF_OpenFont("../fonts/OpenSans-Regular.ttf", 100);

      std::cout << "Font: " << font << std::endl;

      SDL_Color font_color = {255, 255, 255};

      SDL_Surface* font_surface = TTF_RenderText_Solid(font, "test text", font_color);
      SDL_Texture* font_texture = SDL_CreateTextureFromSurface(p_renderer, font_surface);

      SDL_SetRenderDrawColor(p_renderer, 255, 50, 0, 255);
      SDL_Rect bounds{
        x_, y_, width_, height_
      };

      SDL_RenderFillRect(p_renderer, &bounds);
      SDL_RenderCopy(p_renderer, font_texture, 0, 0);
      SDL_RenderPresent(p_renderer);

      SDL_FreeSurface(font_surface);
      TTF_CloseFont(font);
      SDL_DestroyTexture(font_texture);
}