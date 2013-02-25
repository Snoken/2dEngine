#include "physics.h"
#include <iostream>

namespace physics
{
	float aGravity = -9.8f;
}

void physics::applyGravity( actor *curr, double elapsed )
{
	if( curr->bOnGround )
		curr->vertSpeed = 0;
	else
	{
		//TODO: Add ground actors, check for collision with them, set bOnGround accordingly
		curr->vertSpeed += (float)elapsed * aGravity;
		curr->origin.y += curr->vertSpeed * (float)elapsed;
		for( list<baseObject::vertex>::iterator itr = curr->points.begin(); itr != curr->points.end(); ++itr )
		{
			itr->y += curr->vertSpeed * (float)elapsed;
		}
	}
	curr->updateMaxMin();
}

float physics::travelTime( float d, float v, float a )
{
	if( a == 0.0f )
		return d/v;
	//Note: this assumes constant acceleration
	float vf = sqrt( pow( v, 2.0f ) + 2*abs(a)*d );
	//returns time in seconds
	return (vf - abs(v)) / abs(a);
}

void physics::moveByTimeX( actor *curr, float time )
{
	curr->origin.x += curr->getRunSpeed() * (float)time;
	for( list<baseObject::vertex>::iterator itr = curr->points.begin(); itr != curr->points.end(); ++itr )
	{
		itr->x += curr->getRunSpeed() * (float)time;
	}
	curr->updateMaxMin();
}

void physics::moveByTimeY( actor *curr, float time )
{
	curr->origin.y += curr->vertSpeed * (float)time;
	for( list<baseObject::vertex>::iterator itr = curr->points.begin(); itr != curr->points.end(); ++itr )
	{
		itr->y += curr->vertSpeed * (float)time;
	}
	curr->updateMaxMin();
	if( curr->vertSpeed <= 0 )
	{
		curr->bOnGround = true;
		curr->vertSpeed = 0.0f;
	}
}