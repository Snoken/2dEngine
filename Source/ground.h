#ifndef GROUND_H
#define GROUND_H

#include "actor.h"

class ground : public baseObject
{
public:
	ground(vertex origin, list<vertex> points):baseObject(origin, points){}
	ground(vertex origin, list<vertex> points, GLfloat color[4] ):baseObject(origin, points, color){}
	ground(const baseObject& base):baseObject(base){}

private:
	bool bCollision;
};
#endif