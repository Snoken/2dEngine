#ifndef COLLISION_H
#define COLLISION_H

#include "baseObject.h"

namespace collision
{
	//check intersection of two lines
	bool intersecting( primitives::vertex ends1[2], primitives::vertex ends2[2], float e );

	//check if vertex is within specified shape
	bool inObject( primitives::vertex point, baseObject obj );

	//check if one object is inside another
	bool areColliding( baseObject one, baseObject two );

	bool above( baseObject one, baseObject two );
	bool above( primitives::vertex point, baseObject obj );

	bool below( baseObject one, baseObject two );
	bool below( primitives::vertex point, baseObject obj );

	bool nextTo( baseObject one, baseObject two );
	bool nextTo( primitives::vertex point, baseObject obj );

	//returns true if one is left of two
	bool leftOf(baseObject &one, baseObject &two);
	bool leftOf(primitives::vertex &point, baseObject &obj);

	bool rightOf(baseObject &one, baseObject &two);

	bool against(baseObject &one, baseObject &two);
}
#endif