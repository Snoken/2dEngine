#include "actor.h"

void actor::moveRight()
{
	GLfloat dif = walkSpeed * intervalMS/1000;
	this->origin.x += dif;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += dif;
}

void actor::moveLeft()
{
	GLfloat dif = walkSpeed * intervalMS/1000;
	this->origin.x -= dif;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x -= dif;
}