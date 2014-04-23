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
	m_movement.applyGravity(elapsed);
	moveByTimeY((float)elapsed);
	setMaxMin();
}

float actor::timeToCollisionY( baseObject one )
{
	double vert = m_movement.magnitude * sin(m_movement.angle * PI / 180.0f);
	return travelTime( yMin - one.yMax, (float)vert, -9.8f );
}

void actor::moveByTimeX( long double elapsed )
{
	GLfloat difX;
	if( m_bIsRolling )
	{
		difX = m_rollSpeed*(float)elapsed;
		if( !m_bFacingRight )
			difX *= -1.0f;
	}
	else
		difX = (float)elapsed * (float)m_movement.getHorizComp();
	this->origin.x += difX;
	for( list<primitives::vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += difX;
	setMaxMin();
}

void actor::moveByTimeY( float time )
{
	double vert = m_movement.magnitude * sin(m_movement.angle * PI / 180.0f);
	origin.y += (float)vert * (float)time;
	for( list<primitives::vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->y += (float) vert * (float) time;
	setMaxMin();
	vert = 0.0f;
}

void actor::moveByDistanceX( float distance )
{
	this->origin.x += distance;
	for( list<primitives::vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += distance;
	setMaxMin();
}

void actor::moveByDistanceY( float distance )
{
	this->origin.y += distance;
	for (list<primitives::vertex>::iterator itr = points.begin(); itr != points.end(); ++itr)
		itr->y += distance;
	setMaxMin();
}

void actor::updateLocation(const long double & elapsed, const long double & prevElapsed, ground *belowPlayer,
	ground *abovePlayer, map<float, ground*> *nearby, map<int, bool> *keyMap)
{
	double timeDiff = elapsed - prevElapsed;
	if (m_bJump && !m_bOnWall)
	{
		jump();
		m_bJump = false;
	}
	//multisample for collision if linear move distance longer than .05 (minimum platform dimension)
	float xDiff = m_movement.getHorizComp() * timeDiff;
	float yDiff = m_movement.getHorizComp() * timeDiff;
	float linear = sqrtf(pow(xDiff, 2.0f) + pow(yDiff, 2.0f));
	int samples = floor(linear / .05f) + 1;
	double sampleTime = timeDiff / samples;

	for (int i = 0; i < samples; ++i)
	{
		actor *temp;
		if (m_pSlidingOn == NULL)
			m_bOnWall = false;
		// if actor is sliding on something
		else
		{
			//if now below what is being slid on, drop off
			if (m_pSlidingOn->yMin >= yMin)
				m_pSlidingOn = NULL;
			//if player has pressed jump, jump off wall
			else if (m_bOnWall && m_bJump)
			{
				m_bFacingRight ? m_movement.setHorizontalComp(1.0f) :
					m_movement.setHorizontalComp(-1.0f);
				m_bOnWall = false;
				m_movement.setVerticalComp(m_jumpSpeed);
				m_bJump = false;
				m_pSlidingOn = NULL;
			}
			//drop off wall if facing correct direction for key pressed
			else if (m_bOnWall && (keyMap->find('d'))->second == true && m_bFacingRight)
			{
				m_bOnWall = false;
				m_pSlidingOn = NULL;
				m_movement.setHorizontalComp(.1f);
			}
			//drop off wall if facing correct direction for key pressed
			else if (m_bOnWall && (keyMap->find('a'))->second == true && !m_bFacingRight)
			{
				m_bOnWall = false;
				m_pSlidingOn = NULL;
				m_movement.setHorizontalComp(-.1f);
			}
			else
			{
				if (!m_bOnWall)
				{
					m_bOnWall = true;
					m_movement.setVerticalComp(0);
					collision::leftOf(*this, *m_pSlidingOn) ?
						m_bFacingRight = false : m_bFacingRight = true;
				}
				m_fallStart = origin.y;
			}
		}

		double vert = m_movement.getVertComp();
		if (vert < 0.0f && m_fallStart == 999.99f)
			m_fallStart = origin.y;

		primitives::vertex below(origin.x, yMin - .02f);
		if (belowPlayer != NULL && !(collision::inObject(below, *belowPlayer) && vert >= 0.0f))
		{
			if (m_bOnGround)
				m_fallStart = origin.y;
			m_bOnGround = false;
		}

		//if the actor is currently in the air, apply gravity
		if (!m_bOnGround)
		{
			airFrameUpdate();
			temp = new actor(*this);

			//apply gravity to copy to make sure they don't fall through world
			if (m_bOnWall)
				temp->moveByDistanceY(temp->m_slideSpeed * (float) sampleTime);
			else
				temp->applyGravity(sampleTime);

			if (abovePlayer != NULL && !abovePlayer->bIsPlatform &&
				vert > 0.0f && collision::areColliding(*temp, *abovePlayer))
			{
				moveByDistanceY(abs(abovePlayer->yMin - yMax) - .005f);
				m_movement.setVerticalComp(0.0);
			}
			//if next iter of motion still leaves player above ground, do it
			else if (temp->timeToCollisionY(*belowPlayer) > 0)
			{
				*this = *temp;
				m_timeToImpact = temp->timeToCollisionY(*belowPlayer);
			}
			//otherwise, move player just enough to be on ground
			else
			{
				moveByDistanceY(m_timeToImpact*m_movement.getVertComp());
				m_bOnGround = true;
				m_bOnWall = false;
				m_pSlidingOn = NULL;
				m_movement.setVerticalComp(0);
				m_fallEnd = origin.y;
				//TODO: change to be velocity based
				takeFallDamage(m_fallStart - m_fallEnd);
				m_fallStart = 999.99f;
			}
		}
		else
			m_state = groundFrameUpdate(elapsed, abovePlayer);

		double horiz = m_movement.getHorizComp();
		//the magic number of .25 is to prevent large moves after break points and the like
		if (horiz != 0.0f && !m_bOnWall && sampleTime < .25)
		{
			//check if there is a wall within 0.5f
			if (nearby->empty())
				moveByTimeX(sampleTime);
			//try moving
			else
			{
				bool moved = false;
				const long double moveDistance = horiz*sampleTime;
				for (map<float, ground*>::iterator itr = nearby->begin(); itr != nearby->end(); ++itr)
				{
					//if the distance to the object is greater than the amount moved, don't need to worry
					//	about collision
					if (itr->first > abs(moveDistance))
						continue;
					//if object in question is to the right of player and moving in positive dir
					if (collision::leftOf(*this, *itr->second) && horiz > 0)
					{
						moveByDistanceX(itr->first - .001f);
						m_movement.setHorizontalComp(0);
						moved = true;
						//can't slide down soemthing while standing on ground
						if (!m_bOnGround && itr->second->height > 0.051f)
							m_pSlidingOn = itr->second;
						break;
					}
					//otherwise move by negative amt
					else if (collision::rightOf(*this, *itr->second) && horiz < 0)
					{
						moveByDistanceX(-(itr->first - .001f));
						m_movement.setHorizontalComp(0);
						moved = true;
						//can't slide down soemthing while standing on ground
						if (!m_bOnGround && itr->second->height > 0.051f)
							m_pSlidingOn = itr->second;
						break;
					}
				}
				if (!moved)
					moveByTimeX(sampleTime);
			}
		}
	}
}

void actor::decayMult(const long double & elapsed)
{
	//don't decay if in air
	if (!m_bOnGround)
		return;
	double horiz = m_movement.getHorizComp();
	double timeToStop = 0.25;
	if( horiz != 0 )
	{
		if( abs(horiz) < .05 )
		{
			m_frame = 0;
			m_movement.setHorizontalComp(0);
		}
		else
		{
			double mult;
			if (horiz < 0)
				mult = -1.0;
			else
				mult = 1.0;
			m_movement.setHorizontalComp(horiz - (elapsed/timeToStop * m_runSpeed * mult));
		}
	}
}

void actor::updateMult(const long double & elapsed, string dir)
{
	//only change mult if player is not rolling
	double timeToTopSpeed = 0.25;
 	if( !m_bIsRolling )
	{
		//only do if player is not on wall and not facing right
		double horiz = m_movement.getHorizComp();
		double mult;
		if (dir == "left")
		{
			mult = -1.0;
			if (!m_bOnWall)
				m_bFacingRight = false;
		}
		else
		{
			mult = 1.0;
			if (!m_bOnWall)
				m_bFacingRight = true;
		}
		// two different change rates for air and ground
		if (m_bOnGround)
			horiz += elapsed / timeToTopSpeed * m_runSpeed * mult;
		else
			horiz += elapsed / timeToTopSpeed * m_runSpeed * mult / 2;
		if (abs(horiz) > m_runSpeed)
			horiz = m_runSpeed * mult;
		m_movement.setHorizontalComp(horiz);
	}
}

void actor::airFrameUpdate()
{
	m_frame += .15;
	if( m_frame >= 4.85 )
		m_frame = 4;
}

actor::ActorState actor::groundFrameUpdate( const long double & elapsed, ground *abovePlayer )
{
	double horiz = m_movement.getHorizComp();
	if( abs(horiz) <= .0001 )
	{
		m_frame += .05;
		if( m_frame >= 3.95 )
			m_frame = 0;
		if( m_bIsRolling )
			return CROUCHING;
		return IDLE;
	}
	//if player is running
	else
	{
		if( m_bIsRolling )
		{
			m_frame += .15;
			if( m_frame >= 2.95 )
			{
				m_frame = 0;
				actor *endRollTest = new actor(*this);
				endRollTest->endRoll( elapsed );
				if( abovePlayer == NULL || !collision::areColliding(*endRollTest, *abovePlayer) )
				{
					endRoll(elapsed);
					if( abs(horiz) <= .0001 )
						return IDLE;
					else
						return RUNNING;
				}
			}
			return ROLLING;
		}
		else
		{
			m_frame += .175;
			if( m_frame >= 7.825 )
				m_frame = 0;
			return RUNNING;
		}
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

void actor::changeHeight( float newH )
{
	for (list<primitives::vertex>::iterator itr = points.begin(); itr != points.end(); ++itr)
	{
		if( itr->y == yMax )
			itr->y = yMin+newH;
	}
	yMax = yMin + newH;
	origin.y = yMin + newH/2;
	height = newH;
}
void actor::endRoll( const long double & elapsed )
{
	if( m_bIsRolling )
	{
		m_lastRollTime = elapsed;
		m_bIsRolling = false;
		m_frame = 0;
		changeHeight( m_oldHeight );
	}
}
void actor::startRoll( const long double & elapsed )
{
	if(!m_bIsRolling && (elapsed - m_lastRollTime > 0.5 || m_lastRollTime == 0.0))
	{
		m_bIsRolling = true;
		m_frame = 0;
		m_oldHeight = height;
		changeHeight( height*(1.0f/2.0f) );
		setMaxMin();
	}
}

void actor::getNearbyWalls(const float & maxDistance, map<float,ground*> &nearby, list<ground>* allGround)
{
	//get list of walls nearest actor
	for (list<ground>::iterator itr = allGround->begin(); itr != allGround->end(); ++itr)
	{
		float gapSize = 0.0f;
		if (itr->xMin > this->xMax)
			gapSize = itr->xMin - this->xMax;
		else
			gapSize = this->xMin - itr->xMax;

		if (gapSize < maxDistance && collision::nextTo(*this, *itr) && !collision::above(*this, *itr))
			nearby.insert(make_pair(gapSize, &(*itr)));
	}
}