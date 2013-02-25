#ifndef ACTOR_H
#define ACTOR_H
#include "baseObject.h"


using namespace std;
class actor : public baseObject
{
public:
	bool bGravity, bOnGround;
	GLfloat vertSpeed;

	void init()
	{
		walkSpeed = 0.2f;
		runSpeed = 0.3f;
		jumpSpeed = 3.0f;
		vertSpeed = 0.0f;
		bOnGround = false;
	}
	actor(vertex origin, list<vertex> points):baseObject(origin, points)
	{
		init();
	}

	actor(vertex origin, list<vertex> points, GLfloat color[4] ):baseObject(origin, points, color)
	{
		init();
	}

	actor(const baseObject& base):baseObject(base){ init(); }
	actor(const actor& old):baseObject(old)
	{ 
		*this = old;
		bOnGround = old.bOnGround;
	}

	void move( double multiplier );
	void moveX( float distance );
	void jump()
	{
		bOnGround = false;
		vertSpeed = jumpSpeed;
	}

	float getRunSpeed(){ return runSpeed; }

private:
	float walkSpeed, runSpeed, jumpSpeed;
};
#endif