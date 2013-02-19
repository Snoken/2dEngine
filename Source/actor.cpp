#include "actor.h"

void actor::move( double multiplier )
{
	GLfloat difX = runSpeed * (GLfloat)multiplier/15;
	this->origin.x += difX;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += difX;
}

void actor::jump()
{
	bOnGround = false;
	vertSpeed = jumpSpeed;
}