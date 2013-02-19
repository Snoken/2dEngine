#ifndef ACTOR_H
#define ACTOR_H
#include "baseObject.h"
#include "physics.h"

using namespace std;
class actor : baseObject
{
public:
	static const unsigned int intervalMS = 33;
	void init()
	{
		walkSpeed = 0.2f;
		runSpeed = 0.4f;
		jumpSpeed = 0.3f;
		bOnGround = true;
	}
	actor(vertex origin, list<vertex> points):baseObject(origin, points)
	{
		init();
	};

	actor(vertex origin, list<vertex> points, GLfloat color[4] ):baseObject(origin, points, color)
	{
		init();
	};	

	void moveRight();
	void moveLeft();
	void jump();
	GLfloat *getColor()
	{
		return color;
	}
	list<vertex> getPoints()
	{
		return points;
	}

private:
	bool bOnGround;
	physics::vector aim;
	GLfloat walkSpeed, runSpeed, jumpSpeed;
};
#endif