#ifndef PHYSICS_H
#define PHYSICS_H
#include <cmath>
#include "primitives.h"

namespace physics
{
	#define PI 3.14159265
	#define aGravity -9.8
	struct vector
	{
		double magnitude, angle;
		vector() : magnitude(0.0f), angle(0.0f){}
		//use magnitude and destination to find angle
		vector(double mag, double angle) : magnitude(mag), angle(angle){}
		vector(double mag, primitives::vertex orig, primitives::vertex dest) : 
			magnitude(mag)
		{
			angle = atan2(dest.y - orig.y, dest.x-orig.x) * 180.0 / PI;
		}
		bool operator==(const vector& rhs)
		{
			return magnitude == rhs.magnitude &&
				angle == rhs.angle;
		}

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
	// TODO: why does this take a reference?
	double apex(vector &motion, primitives::vertex &start);
	double apexTime(vector &motion);
	//this method only considers the vertical aspect
	double timeToLand(vector &motion, primitives::vertex &start, primitives::vertex &end);

	//calculates minimum vertical launch speed to reach given end height
	float requiredVertSpeed(float &startHeight, float &endHeight);

	//calculates minimum vertical launch speed to reach given end height after given time
	float reqSpeedWithTime(float &startHeight, float &endHeight, float &time);
}
#endif
