#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include<string>
#include "scene.cpp"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

namespace Game_tools {

    void put_text(std::string context, double x, double y) {
        SDL_Surface* text_tmp = TTF_RenderUTF8_Blended(font, context.c_str(), { 255, 255, 255, 255 });
        SDL_Texture* tex_text_tmp = SDL_CreateTextureFromSurface(renderer, text_tmp);
        SDL_Rect text_rect = { x - (text_tmp->w / 2), y - (text_tmp->h / 2),text_tmp->w ,text_tmp->h };
        SDL_RenderCopy(renderer, tex_text_tmp, nullptr, &text_rect);
        SDL_DestroyTexture(tex_text_tmp); SDL_FreeSurface(text_tmp);
    }
}