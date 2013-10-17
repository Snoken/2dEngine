#ifndef PHYSICS_H
#define PHYSICS_H
#include <cmath>

namespace physics
{
	#define PI 3.14159265
	#define aGravity -9.8
	struct vector{
		double magnitude, angle;
		vector() : magnitude(0.0f), angle(0.0f){}
		void changeVerticalComp(double addAmount);
		void changeHorizontalComp(double addAmount);
		void setVerticalComp(double value);
		void setHorizontalComp(double value);
		void applyGravity(double elapsed);
		double getHorizComp()
		{
			return this->magnitude * cos(this->angle * PI / 180.0f);
		}
		double getVertComp()
		{
			return this->magnitude * sin(this->angle * PI / 180.0f);
		}
	};
}
#endif