#pragma once
#include "timer.cpp"
#include "vector2.cpp"

#include <SDL.h>


class Camera
{
public:
	Camera(SDL_Renderer* renderer)
	{
		this->renderer = renderer;

		timer_shake.set_one_shot(true);
		timer_shake.set_on_timeout([&]()
			{
				this -> is_shaking = false;
				this -> reset();
			});
	}

	~Camera() = default;

	/*void set_timeout()
	{
		is_shaking = false;
		reset();
	}*/

	const Vector2& get_position() const
	{
		return position;
	}

	void reset()
	{
		position.x = 0;
		position.y = 0;
	}

	void on_update(float delta)
	{
		timer_shake.on_update(delta);

		if (is_shaking)
		{
			position.x = (-50 + rand() % 100) / 50.0f * shaking_strength;
			position.y = (-50 + rand() % 100) / 50.0f * shaking_strength;
		}
	}

	void shake(float strength, float duration)
	{
		is_shaking = true;
		shaking_strength = strength;

		timer_shake.set_aim_time(duration);
		timer_shake.restart();
	}

	void render_texture(SDL_Texture* texture, const SDL_Rect* rect_src,
		const SDL_FRect* rect_dst, double angle, const SDL_FPoint* center) const
	{
		SDL_FRect rect_dst_win = *rect_dst;
		rect_dst_win.x -= position.x;
		rect_dst_win.y -= position.y;

		SDL_RenderCopyExF(renderer, texture, rect_src,
			&rect_dst_win, angle, center, SDL_RendererFlip::SDL_FLIP_NONE);
	}

private:
	Vector2 position;
	TIMER timer_shake;
	bool is_shaking = false;
	float shaking_strength = 0;
	SDL_Renderer* renderer = nullptr;

};