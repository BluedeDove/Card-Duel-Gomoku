#pragma once

#include <SDL.h>
#include <functional>

class TIMER
{
public:
	TIMER() = default;
	~TIMER() = default;

	void set_on_timeout(std::function<void()> on_timeout)//�����ʱ������Ĺ��ܺ���
	{
		this->function = on_timeout;
	}

	void restart()
	{
		time_now = 0;
		shotted = false;
	}

	void set_aim_time(double val)//��Ա�ķ�װ���ӿ�
	{
		aim_time = val;
	}

	void set_one_shot(bool flag)
	{
		for_once = flag;
	}

	void pause()
	{
		paused = true;
	}

	void resume()
	{
		paused = true;
	}

	void on_update(double delta_time)//����
	{
		if (paused) { return; }
		else {
			time_now += delta_time;
			if (time_now >= aim_time)
			{
				bool can_shot = (!for_once || (for_once && !shotted));
				if (can_shot && function)
				{
					function();
				}
				time_now -= aim_time;//�����δ�������
			}
		}
	}

	double get_time()
	{
		return time_now;
	}

private:
	double aim_time = 0;
	double time_now = 0;

	std::function<void()> function;

	bool paused = false;
	bool for_once = false;
	bool shotted = false;

};