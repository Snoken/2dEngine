#include "actor.h"

void actor::move( double multiplier )
{
	GLfloat difX = runSpeed * (GLfloat)multiplier/15;
	this->origin.x += difX;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += difX;
	updateMaxMin();
}

void actor::moveX( float distance )
{
	GLfloat difX = distance;
	this->origin.x += difX;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += difX;
	updateMaxMin();
}

void actor::moveY( float distance )
{
	GLfloat difY = distance;
	this->origin.y += difY;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->y += difY;
	updateMaxMin();
}