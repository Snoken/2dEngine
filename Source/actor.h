#ifndef ACTOR_H
#define ACTOR_H
#include "baseObject.h"
#include "ground.h"
#include "collision.h"
#include <cmath>
#include <map>

#ifdef WIN32
	#include <Windows.h>
	#include <MMSystem.h>
#endif

using namespace std;
class actor : public baseObject
{
public:
	enum ActorState { IDLE, RUNNING, ROLLING, CROUCHING, SLIDING };
	ActorState m_state;
	bool m_bOnGround, m_bFacingRight, m_bIsRolling, m_bOnWall;
	double m_frame;

	void init()
	{
		m_state = IDLE;
		m_walkSpeed = 0.2f;
		m_runSpeed = 1.0f;
		m_jumpSpeed = 3.0f;
		m_vertSpeed = 0.0f;
		m_slideSpeed = -0.3f;
		m_rollSpeed = .8f;
		m_bOnGround = false;
		m_bOnWall = false;
		m_bIsRolling = false;
		m_health = 100.0f;
		m_damageDistance = 1.0f;
		m_fallStart = 0.0f;
		m_timeToImpact = 0.0f;
		m_multiplier = 0.0f;
		m_frame = 0.0f;
		m_pSlidingOn = NULL;
		m_climbAngle = 45.0f;
		m_lastRollTime = 0;
	}
	actor(primitives::vertex origin, list<primitives::vertex> points):baseObject(origin, points)
	{
		init();
	}

	actor(primitives::vertex origin, list<primitives::vertex> points, GLfloat color[4] ):baseObject(origin, points, color)
	{
		init();
	}

	actor(const baseObject& base):baseObject(base){ init(); }
	actor(const actor& old):baseObject(old)
	{ 
		*this = old;
		m_bOnGround = old.m_bOnGround;
	}
	actor( primitives::vertex origin, float width, float height, GLuint texture = 0 ):baseObject(origin, width, height, texture)
	{
		init();
	}
	void jump()
	{
		if( !m_bIsRolling )
		{
			m_frame = 0;
			m_bOnGround = false;
			m_vertSpeed = m_jumpSpeed;
		}
	}
	void startRoll( const long double & elapsed );
	void takeFallDamage( float distance )
	{
		if( distance > m_damageDistance )
			m_health -= 10.0f;
	}

	void updateLocation( const long double & elapsed, ground *belowPlayer, 
		ground *abovePlayer, list<ground> *nearby, map<int, bool> *keyMap );
	float getHealth(){ return m_health; }
	double getMult(){ return m_multiplier; }
	void decayMult();
	void updateMult();
	void airFrameUpdate();
	ActorState groundFrameUpdate( const long double & elapsed, ground *abovePlayer );
	bool isMoving() { return m_multiplier != 0; }

private:
	double m_multiplier, m_lastRollTime;
	ground *m_pSlidingOn;
	bool m_bGravity;
	float m_health, m_fallStart, m_fallEnd, m_vertSpeed, m_slideSpeed,
		m_walkSpeed, m_runSpeed, m_jumpSpeed, m_damageDistance, 
		m_timeToImpact, m_climbAngle, m_rollSpeed, m_oldHeight;
	float travelTime( float d, float v, float a );
	void applyGravity( double elapsed );
	float timeToCollisionY( baseObject one );
	void moveByTimeY( float time );
	void moveByTimeX( long double elapsed );
	void moveByDistanceX( float distance );
	void moveByDistanceY( float distance );
	//not currently in use
	bool facing( const baseObject &one );
	void changeHeight( float newHeight );
	void endRoll( const long double & elapsed );
};
#endif
