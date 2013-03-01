#ifndef ACTOR_H
#define ACTOR_H
#include "baseObject.h"
#include "ground.h"

using namespace std;
class actor : public baseObject
{
public:
	bool bGravity, bOnGround, bOnWall;
	GLfloat vertSpeed, slideSpeed;
	float health;

	void init()
	{
		walkSpeed = 0.2f;
		runSpeed = 0.3f;
		jumpSpeed = 3.0f;
		vertSpeed = 0.0f;
		slideSpeed = -0.3f;
		bOnGround = false;
		bOnWall = false;
		health = 100.0f;
		damageDistance = 2.5f;
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
	actor( vertex origin, float width, float height, GLuint texture = 0 ):baseObject(origin, width, height, texture)
	{
		init();
	}

	void move( double multiplier );
	void moveX( float distance );
	void moveY( float distance );
	void jump()
	{
		bOnGround = false;
		vertSpeed = jumpSpeed;
	}
	void takeFallDamage( float distance )
	{
		if( distance > damageDistance )
			health -= 10.0f;
	}

	void updateLocation( const long double & elapsed, double multiplier );
	float getRunSpeed(){ return runSpeed; }

private:
	float walkSpeed, runSpeed, jumpSpeed, damageDistance;
	ground *getCurrentGround( list<ground> *groundObjs );
};
#endif