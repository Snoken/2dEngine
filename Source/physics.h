//This class is currently empty, will possibly be re-implemented for projectiles

#ifndef PHYSICS_H
#define PHYSICS_H

namespace physics
{
	extern float aGravity;
	struct vector{
		float mag, angle;
		vector():mag(0.0f),angle(0.0f){}
	};
}
#endif