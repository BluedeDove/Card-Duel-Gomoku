#pragma once

#include<functional>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include "vector2.cpp"
#include "camera.cpp"


extern bool mouse_key_down;

extern SDL_Texture* button_up1;
extern SDL_Texture* button_down1;

extern SDL_Texture* button_up2;
extern SDL_Texture* button_down2;

extern SDL_Texture* tile_check;
extern Vector2 mouse_pos;
extern TTF_Font* font;
extern SDL_Renderer* renderer;


class Button
{
public:
	Button(int type, double x = 0, double y = 0, TTF_Font* select_font = font)//中心坐标
	{
		now_font = select_font;
		switch (type)//方便后期添加不同种类的按钮
		{
		case 0:up = down = tile_check; break;
		case 1:up = button_up1; down = button_down1; break;
		case 2:up = button_up2; down = button_down2; break;

		}

		int width, height;
		SDL_QueryTexture(up, nullptr, nullptr, &width, &height);
		rect.w = width; rect.h = height;
		rect.x = x - rect.w / 2; rect.y = y - rect.h / 2;
		center.x = x; center.y = y;
	}
	Button() = default;
	~Button() = default;

	void set_on_press(std::function<void()> function)
	{
		this->function = function;
	}

	bool if_press()
	{
		bool right_check = mouse_pos.x >= rect.x && mouse_pos.x <= (rect.x + rect.w);
		bool up_check = mouse_pos.y >= rect.y && mouse_pos.y <= (rect.y + rect.h);
		return (right_check && up_check && mouse_key_down);
	}

	bool if_hovering()
	{
		bool right_check = mouse_pos.x >= rect.x && mouse_pos.x <= (rect.x + rect.w);
		bool up_check = mouse_pos.y >= rect.y && mouse_pos.y <= (rect.y + rect.h);
		return (right_check && up_check);
	}

	void pressed()
	{
		if(B_on)
		{
			function(); 
			mouse_key_down = false;
		}
	}

	void set_pos(double x, double y)
	{
		int width, height;
		SDL_QueryTexture(up, nullptr, nullptr, &width, &height);
		rect.w = width; rect.h = height;
		rect.x = x - rect.w / 2; rect.y = y - rect.h / 2;
		center.x = x; center.y = y;
	}

	void set_text(std::string B_context)
	{
		this->context = B_context;
	}

	void render(const Camera& camera)
	{
		if (show) {
			SDL_Surface* text = TTF_RenderUTF8_Blended(now_font, context.c_str(), { 255, 255, 255, 255 });
			SDL_Texture* tex_text = SDL_CreateTextureFromSurface(renderer, text);

			SDL_FRect rect_dst;

			if (if_press())
			{
				rect_dst.x = rect.x; rect_dst.y = rect.y;
				rect_dst.w = rect.w; rect_dst.h = rect.h;
				camera.render_texture(down, nullptr, &rect_dst, 0, &center);
			}
			else //if (up && down)
			{
				rect_dst.x = rect.x; rect_dst.y = rect.y;
				rect_dst.w = rect.w; rect_dst.h = rect.h;
				camera.render_texture(up, nullptr, &rect_dst, 0, &center);
			}

			SDL_Rect text_rect = { center.x - (text->w / 2), center.y - (text->h / 2),text->w ,text->h };

			SDL_RenderCopy(renderer, tex_text, nullptr, &text_rect);

			SDL_DestroyTexture(tex_text); SDL_FreeSurface(text);
		}
	}

	void set_if_show(bool flag)
	{
		show = flag;
	}

	void set_id(int num)
	{
		id = num;
	}

	int get_id()
	{
		return id;
	}

	SDL_Rect get_rect()
	{
		return rect;
	}

public:
	bool B_on = true;

private:

	SDL_Rect rect;
	SDL_FPoint center = { 0 };

	std::function<void()> function = nullptr;

	//rctor2 B_pos;
	SDL_Texture* up = nullptr;
	SDL_Texture* down = nullptr;

	std::string context = " ";
	int id;

	bool show = true;

	TTF_Font* now_font;
};