#ifndef GROUND_H
#define GROUND_H

#include "actor.h"

class ground : public baseObject
{
public:
	ground(vertex origin, list<vertex> points):baseObject(origin, points)
	{
		init();
	};
	actor(vertex origin, list<vertex> points, GLfloat color[4] ):baseObject(origin, points, color)
	{
		init();
	};

private:
	bool bCollision;
	list<vertex> collisionFaceCoords;
};
#endif