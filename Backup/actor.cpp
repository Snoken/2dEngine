#include "actor.h"
#include <iostream>

float actor::travelTime( float d, float v, float a )
{
	if( a == 0.0f )
		return d/v;
	//Note: this assumes constant acceleration
	float vf = sqrt( pow( v, 2.0f ) + 2*abs(a)*d );
	//returns time in seconds
	return (vf - abs(v)) / abs(a);
}

void actor::applyGravity( double elapsed )
{
	if( elapsed == 0.0f )
		return;
	if( m_bOnGround )
		m_vertSpeed = 0;
	else
	{
		m_vertSpeed += (float)elapsed * -9.8f; //accel of gravity
		origin.y += m_vertSpeed * (float)elapsed;
		for( list<baseObject::vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		{
			itr->y += m_vertSpeed * (float)elapsed;
		}
	}
	setMaxMin();
}

float actor::timeToCollisionY( baseObject one )
{
	return travelTime( yMin - one.yMax, m_vertSpeed, -9.8f );
}

void actor::moveByTimeX( long double elapsed )
{
	GLfloat difX = m_runSpeed*elapsed * (GLfloat)m_multiplier;
	this->origin.x += difX;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += difX;
	setMaxMin();
}

void actor::moveByTimeY( float time )
{
	origin.y += m_vertSpeed * (float)time;
	for( list<baseObject::vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->y += m_vertSpeed * (float)time;
	setMaxMin();
	m_bOnGround = true;
	m_vertSpeed = 0.0f;
}

void actor::moveByDistanceX( float distance )
{
	GLfloat difX = distance;
	this->origin.x += difX;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += difX;
	setMaxMin();
}

void actor::moveByDistanceY( float distance )
{
	GLfloat difY = distance;
	this->origin.y += difY;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->y += difY;
	setMaxMin();
}

void actor::updateLocation( const long double & elapsed, ground *belowPlayer, 
	ground *abovePlayer, list<ground> *nearby, map<int, bool> *keyMap )
{
	actor *temp;
	if( m_pSlidingOn == NULL )
		m_bOnWall = false;
	else
	{
		if( m_pSlidingOn->yMin > yMin )
			m_pSlidingOn = NULL;
		else if( m_bOnWall && (keyMap->find('w'))->second == true )
		{
			m_bFacingRight ? m_multiplier = 1.0f: m_multiplier = -1.0f;
			m_bOnWall = false;
			m_vertSpeed = 3.0f;
			m_pSlidingOn = NULL;
		}
		else if( m_bOnWall && (keyMap->find('d'))->second == true && m_bFacingRight )
		{
			m_bOnWall = false;
			m_pSlidingOn = NULL;
		}
		else if( m_bOnWall && (keyMap->find('a'))->second == true && !m_bFacingRight )
		{
			cout << "leaving wall to left" << endl;
			m_bOnWall = false;
			m_pSlidingOn = NULL;
		}
		else
		{
			if( !m_bOnWall )
			{
				m_bOnWall = true;
				m_bFacingRight = !m_bFacingRight;
			}
			m_fallStart = origin.y;
		}
	}

	if( m_vertSpeed < 0.0f && m_fallStart == 999.99f )
		m_fallStart = origin.y;

	baseObject::vertex below( origin.x, yMin - .02 );
	if( !(collision::inObject( below, *belowPlayer ) && m_vertSpeed > 0.0f) )
	{
		if( m_bOnGround )
			m_fallStart = origin.y;
		m_bOnGround = false;
	}

	//if the player is currently in the air, apply gravity
	if( !m_bOnGround )
	{
		temp = new actor(*this);

		//apply gravity to copy to make sure they don't fall through world
		if( m_bOnWall )
			temp->moveByDistanceY( temp->m_slideSpeed * (float)elapsed );
		else
			temp->applyGravity( elapsed );

		if( abovePlayer != NULL && !abovePlayer->bIsPlatform && 
			m_vertSpeed > 0.0f && collision::areColliding( *temp, *abovePlayer ) )
		{
			moveByDistanceY( abovePlayer->yMin - yMax - .005 );
			m_vertSpeed = 0.0f;
		}
		//if next iter of motion still leaves player above ground, do it
		else if(temp->timeToCollisionY( *belowPlayer ) > 0)
		{
			*this=*temp;
			m_timeToImpact = temp->timeToCollisionY( *belowPlayer );
		}
		//otherwise, moveByTimeX player just enough to be on ground
		else
		{
			moveByTimeY( m_timeToImpact );
			m_fallEnd = origin.y;
			takeFallDamage( m_fallStart-m_fallEnd );
			m_fallStart = 999.99f;

		}
	}

	if( m_multiplier != 0.0f && !m_bOnWall )
	{
		//check if there is a wall within 1.0f
		if( nearby->empty() )
		{
			moveByTimeX( elapsed );
		}
		//try moving
		else
		{
			temp = new actor(*this);
			bool moved = false;
			temp->moveByTimeX( elapsed );
			for( list<ground>::iterator itr = nearby->begin(); itr != nearby->end(); ++itr )
			{
				if( xMin == itr->xMax )
				{
					if( m_multiplier < 0)
					{
						m_multiplier = 0.0f;
						moved = true;
						break;
					}
				}
				else if( xMax == itr->xMin )
				{
					if( m_multiplier > 0)
					{
						m_multiplier = 0.0f;
						moved = true;
						break;
					}
				}
				if( collision::areColliding( *temp, *itr ) && !itr->bIsPlatform )
				{
					if( !m_bOnGround )
					{
						cout << "hit wall" << endl;
						cout << "Setting to " << &(*itr) << endl;
						m_pSlidingOn = new ground(*itr);
					}
					if(xMin > itr->xMax) 
						moveByDistanceX( -(xMin - itr->xMax) );
					else
						moveByDistanceX( itr->xMin - xMax );
					m_multiplier = 0.0f;
					moved = true;
					break;
				}
			}
			if( !moved )
				*this = *temp;
		}
	}
}

void actor::decayMult()
{
	if( m_bOnGround && m_multiplier != 0 )
	{
		if( abs(m_multiplier) < .05 )
		{
			m_frame = 0;
			m_multiplier = 0;
		}
		else
			m_multiplier /= 1.2;
	}
}

void actor::updateMult()
{
	//only do if player is not on wall and not facing right
	if( !m_bOnWall && !m_bFacingRight)
	{
		//logic for ramping up moveByTimeX speed
		if( m_multiplier >= 0 )
		{
			if( m_bOnGround )
				m_multiplier = -.1;
			else
				m_multiplier -= .1;
		}
		else if( m_multiplier > -1.0 )
		{
			if( m_multiplier > -.1 )
				m_multiplier = -.1;
			m_multiplier *= 1.2;
		}
		else
			m_multiplier = -1.0;
	}
	else if( !m_bOnWall && m_bFacingRight )
	{
		//logic for ramping up moveByTimeX speed
		if( m_multiplier <= 0 )
			if( m_bOnGround )
				m_multiplier = .1;
			else
				m_multiplier += .1;
		else if( m_multiplier < 1.0 )
		{
			if( m_multiplier < .1 )
				m_multiplier = .1;
			m_multiplier *= 1.2;
		}
		else
			m_multiplier = 1.0;
	}
}

void actor::airFrameUpdate( bool &bSliding )
{
	if( m_bOnWall )
		bSliding = true;
	else
		bSliding = false;
	m_frame += .15;
	if( m_frame >= 4.85 )
		m_frame = 4;
}

void actor::groundFrameUpdate( bool &running )
{
	if( m_multiplier == 0 )
	{
		m_frame += .05;
		if( m_frame >= 3.95 )
			m_frame = 0;
		running = false;
	}
	//if player is running
	else
	{
		m_frame += .175;
		if( m_frame >= 7.825 )
			m_frame = 0;
		running = true;
	}
}

bool actor::facing( const baseObject &one )
{
	//check if actor is facing the given object
	if( one.xMin >= xMax && m_bFacingRight )
		return true;
	else if( one.xMax <= xMin && !m_bFacingRight )
		return true;
	return false;
}