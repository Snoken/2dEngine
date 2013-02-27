#ifndef COLLISION_H
#define COLLISION_H

#include "baseObject.h"
#include "physics.h"

namespace collision
{
	//check intersection of two lines
	bool intersecting( baseObject::vertex ends1[2], baseObject::vertex ends2[2], float e );

	//check if vertex is within specified shape
	bool inObject( baseObject::vertex point, baseObject obj );

	//check if one object is inside another
	bool areColliding( baseObject one, baseObject two );

	float timeToCollisionY( actor one, baseObject two );

	bool above( actor one, baseObject two );

	bool below( actor one, baseObject two );

	bool nextTo( actor one, baseObject two );
}
#endif