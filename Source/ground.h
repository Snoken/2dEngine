#ifndef GROUND_H
#define GROUND_H

#include "actor.h"

class ground : public baseObject
{
public:
	ground(vertex origin, list<vertex> points, bool bIsPlatform = false):baseObject(origin, points), bIsPlatform(bIsPlatform){}
	ground(vertex origin, list<vertex> points, GLfloat color[4], bool bIsPlatform = false):baseObject(origin, points, color), bIsPlatform(bIsPlatform){}
	ground(const baseObject& base, bool bIsPlatform = false):baseObject(base), bIsPlatform(bIsPlatform){}
	bool bIsPlatform;
};
#endif