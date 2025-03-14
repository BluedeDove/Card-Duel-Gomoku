#pragma once
#include<cmath>

class Vector2
{
public:
	double x = 0;
	double y = 0;

	Vector2() = default;
	~Vector2() = default;

	Vector2(double x, double y) :x(x), y(y) {};

	Vector2 operator+(const Vector2& v) const
	{
		return Vector2(x + v.x, y + v.y);
	}

	void operator+=(const Vector2& v)
	{
		x += v.x; y += v.y;
	}

	Vector2 operator-(const Vector2& v) const
	{
		return Vector2(x - v.x, y - v.y);
	}

	void operator-=(const Vector2& v)
	{
		x -= v.x; y -= v.y;
	}

	double operator*(const Vector2& v) const
	{
		return x * v.x + y * v.y;
	}

	void operator*=(double num)
	{
		x *= num; y *= num;
	}

	Vector2 operator/(double num) const
	{
		return Vector2(x / num, y / num);
	}

	void operator/=(double num)
	{
		x /= num; y /= num;
	}

	double length()
	{
		return sqrt(x * x + y * y);
	}

	Vector2 normalize()
	{
		double len = length();

		if (len == 0)
			return Vector2(0, 0);

		return Vector2(x / len, y / len);
	}

};