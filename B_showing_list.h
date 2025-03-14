#pragma once

#include<vector>
#include<functional>
#include"button.cpp"
#include"camera.cpp"

extern Game_setting setting;

class B_list
{
public:
	B_list() = default;
	~B_list() = default;

	void add_button(int type,std::function<void()> function, std::string text = u8" ")//无位置信息，需要初始化赋予
	{
		Button tmp = { type ,0,0 };
		tmp.set_on_press(function);
		tmp.set_text(text);
		list.push_back(tmp);
		std::cout << &function << std::endl;
	}

	void init()
	{
		this->total_number = list.size();
		std::cout << "加载按钮数" << total_number << std::endl;
		for (int i = 0; i < total_number; i++)
		{
			double height = list[i].get_rect().h;
			list[i].set_pos(setting.resolution.resolution.x / 2, setting.resolution.resolution.y / 4 + height * 1.5 * i + pos_shift);
			//居中设置
		}
	}

	void set_B_id()
	{
		for (int i = 0; i < total_number; i++)
		{
			list[i].set_id(i);
		}
	}

	void handleEvent(const SDL_Event& event)/////小心，按钮类型要统一
	{
		double height = list[0].get_rect().h;
		if (event.type == SDL_MOUSEWHEEL)
		{
			if (event.wheel.y == 1)
			{
				if(pos_shift < 0)
				pos_shift += wheel_speed;
			}
			if (event.wheel.y == -1)
			{
				double tes_tmp = setting.resolution.resolution.y / 2 - height * 1.5 * total_number;
				if (pos_shift > tes_tmp)
				pos_shift -= wheel_speed;
			}
			//pos_shift = event.wheel.y * wheel_speed;
		}
		for (int i = 0; i < total_number; i++)
		{
			list[i].set_pos(setting.resolution.resolution.x / 2, setting.resolution.resolution.y / 4 + height*1.5*i + pos_shift);
			//居中设置
		}

	}

	void clear()
	{
		/*for (int i = 0; i < list.size(); i++)
		{
			list.pop_back();
		}*/
		list.resize(0);
	}

	void on_updata(double delta)
	{

	}

	void render(const Camera& camera)
	{
		for (Button tmp : list)
		{
			tmp.render(camera);
		}
	}

public:
	std::vector<Button> list;
	int total_number;
	double pos_shift = 0;
	double wheel_speed = 60;
};