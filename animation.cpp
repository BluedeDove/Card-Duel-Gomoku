#pragma once

#include "timer.cpp"
#include "atlas.cpp"
#include "camera.cpp"
#include "vector2.cpp"
#include "setting.cpp"

#include <SDL.h>

#include <vector>
#include <functional>

extern Game_setting setting;

class Animation
{
public:
	Animation()
	{
		timer.set_one_shot(false);
		timer.set_on_timeout(
			[&]()
			{
				idx_frame++;
				if (idx_frame >= frame_list.size())
				{
					idx_frame = is_loop ? 0 : frame_list.size() - 1;
					if (on_finished)
						on_finished();
				}
			}
		);
	}

	~Animation() = default;

	void reset()
	{
		timer.restart();

		idx_frame = 0;
	}

	void set_position(const Vector2& position)
	{
		this->position = position;
	}

	void set_rotation(double angle)
	{
		this->angle = angle;
	}

	void set_center(const SDL_FPoint& center)
	{
		this->center = center;
	}

	void set_loop(bool is_loop)
	{
		this->is_loop = is_loop;
	}

	void set_interval(float interval)
	{
		timer.set_aim_time(interval);
	}

	void set_on_finished(std::function<void()> on_finished)
	{
		this->on_finished = on_finished;
	}

	void add_frame(SDL_Texture* texture, int num_h)
	{
		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		int width_frame = width / num_h;

		for (int i = 0; i < num_h; i++)
		{
			SDL_Rect rect_src;
			rect_src.x = i * width_frame, rect_src.y = 0;
			rect_src.w = width_frame, rect_src.h = height;

			frame_list.emplace_back(texture, rect_src);
		}
	}

	void add_frame(Atlas* atlas)
	{
		for (int i = 0; i < atlas->get_size(); i++)
		{
			SDL_Texture* texture = atlas->get_texture(i);

			int width, height;
			SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

			SDL_Rect rect_src;
			rect_src.x = 0, rect_src.y = 0;
			rect_src.w = width, rect_src.h = height;

			frame_list.emplace_back(texture, rect_src);
		}
	}

	void add_frame(Atlas* atlas, bool background)
	{
		for (int i = 0; i < atlas->get_size(); i++)
		{
			SDL_Texture* texture = atlas->get_texture(i);

			SDL_Rect rect_src = { 0,0,setting.resolution.resolution.x ,setting.resolution.resolution.y };

			frame_list.emplace_back(texture, rect_src);
		}
	}

	void on_update(float delta)
	{
		timer.on_update(delta);
	}

	void on_render(const Camera& camera) const
	{
		const Frame& frame = frame_list[idx_frame];
		const Vector2& pos_camera = camera.get_position();

		SDL_FRect rect_dst;
		rect_dst.x = position.x - frame.rect_src.w / 2;
		rect_dst.y = position.y - frame.rect_src.h / 2;
		rect_dst.w = (float)frame.rect_src.w, rect_dst.h = (float)frame.rect_src.h;

		camera.render_texture(frame.texture, &frame.rect_src, &rect_dst, angle, &center);
	}

	double get_time()
	{
		return timer.get_time();
	}

private:
	struct Frame
	{
		SDL_Rect rect_src;
		SDL_Texture* texture = nullptr;

		Frame() = default;
		Frame(SDL_Texture* texture, const SDL_Rect& rect_src)
			: texture(texture), rect_src(rect_src) { }

		~Frame() = default;
	};

private:
	Vector2 position;
	double angle = 0;
	SDL_FPoint center = { 0 };

	TIMER timer;
	bool is_loop = true;
	size_t idx_frame = 0;
	std::vector<Frame> frame_list;
	std::function<void()> on_finished;

};