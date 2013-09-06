#ifndef COLLISION_H
#define COLLISION_H

#include "baseObject.h"

namespace collision
{
	//check intersection of two lines
	bool intersecting( baseObject::vertex ends1[2], baseObject::vertex ends2[2], float e );

	//check if vertex is within specified shape
	bool inObject( baseObject::vertex point, baseObject obj );

	//check if one object is inside another
	bool areColliding( baseObject one, baseObject two );

	bool above( baseObject one, baseObject two );

	bool below( baseObject one, baseObject two );

	bool nextTo( baseObject one, baseObject two );
}
#endif