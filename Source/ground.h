#ifndef GROUND_H
#define GROUND_H

#include "baseObject.h"
#include "primitives.h"

class ground : public baseObject
{
public:
	ground(primitives::vertex origin, list<primitives::vertex> points, bool bIsPlatform = false):baseObject(origin, points), bIsPlatform(bIsPlatform){}
	ground(primitives::vertex origin, list<primitives::vertex> points, GLfloat color[4], bool bIsPlatform = false):baseObject(origin, points, color), bIsPlatform(bIsPlatform){}
	ground(const baseObject& base, bool bIsPlatform = false):baseObject(base), bIsPlatform(bIsPlatform){}
	bool bIsPlatform;
};
#endif