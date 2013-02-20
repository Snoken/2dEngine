#ifndef PHYSICS_H
#define PHYSICS_H 
#include "actor.h"

namespace physics
{
	extern float aGravity;
	struct vector{
		GLfloat mag, angle;
		vector():mag(0.0f),angle(0.0f){}
	};
	void applyGravity( actor *curr, double elapsed );
	void moveByTimeX( actor *curr, float time );
	void moveByTimeY( actor *curr, float time );
	float travelTime( float d, float v, float a );
}
#endif