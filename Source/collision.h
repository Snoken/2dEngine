#ifndef COLLISION_H
#define COLLISION_H

#include "baseObject.h"

//This namespace contains methods for determining collision and relative positioning
// of two objects
namespace collision
{
	//check intersection of two lines
	bool intersecting( baseObject::vertex ends1[2], baseObject::vertex ends2[2], float e );

	//check if vertex is within specified shape
	bool inObject( baseObject::vertex point, baseObject obj );

	//check if one object is inside another, uses inObject
	bool areColliding( baseObject one, baseObject two );

	//check if one is in the given position relative to two
	bool above( baseObject one, baseObject two );
	bool below( baseObject one, baseObject two );
	//this function has no distance threshold, it only checks that one's height
		//lies at least in part within two's height
	bool nextTo( baseObject one, baseObject two );
}
#endif