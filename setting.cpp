#pragma once
#include"vector2.cpp"
#include<vector>

class Resolution
{
public:
	Vector2 resolution = { 1280 ,720 };
	Vector2 resolution_copy = { 1280 ,720 };
	void operator[] (const double num)//用于设定分辨率
	{
		reset();
		resolution *= num;
	}
	void reset()
	{
		resolution = resolution_copy;
	}
};

class Game_setting
{
public:
	int fps = 144;
	int resolution_mode = 1;//设置分辨率的规格，1080（1.5），2k（2.0），4k（3.0）
	Resolution resolution;

public:
	Game_setting()
	{
		resolution[resolution_mode];
	}
};

class Game_player
	{
	public:
		int get_player_num()
		{
			return now_player_id;
		}

		void set_nowplayer(int num)
		{
			now_player_id = num;
		}

		void next_player()
		{
			now_player_id++;

			if (now_player_id > id_sets.size())
			{
				now_player_id = 1;
			}

		}

		void set_now_winner(int num)
		{
			now_winner = num;
		}

		int get_now_winner()
		{
			return now_winner;
		}

	private:
		int now_winner = -1;
		int now_player_id = 1;
		std::vector<int> id_sets = {1,2};
	};