#ifndef GROUND_H
#define GROUND_H

//This class represent terrain in the editor
class ground : public baseObject
{
public:
	ground(vertex origin, list<vertex> points, bool bIsPlatform = false):baseObject(origin, points), bIsPlatform(bIsPlatform){}
	ground(vertex origin, list<vertex> points, GLfloat color[4], bool bIsPlatform = false):baseObject(origin, points, color), bIsPlatform(bIsPlatform){}
	ground(const baseObject& base, bool bIsPlatform = false):baseObject(base), bIsPlatform(bIsPlatform){}

	//This variable decides whether or not an object will have x direction collision
		//as well as whether or not an actor can jump through it
	bool bIsPlatform;
};
#endif