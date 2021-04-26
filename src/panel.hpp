// Trent Julich ~ 24 April 2021

#ifndef PANEL_HPP
#define PANEL_HPP

#include <SDL.h>

class Panel {
  public: 
    Panel(int p_x, int p_y, int p_width, int p_height) {
      x_ = p_x;
      y_ = p_y;
      height_ = p_height;
      width_ = p_width;
    }

    Panel() {
      x_ = 0;
      y_ = 0;
      width_ = 0;
      height_ = 0;
    }

    virtual void Render(SDL_Renderer* p_renderer) = 0;

  protected: 
    int x_;
    int y_;

    int width_;
    int height_;
};

#endif