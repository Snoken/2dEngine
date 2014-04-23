#ifndef ACTOR_H
#define ACTOR_H
#include "baseObject.h"
#include "ground.h"
#include "collision.h"
#include "physics.h"
#include <cmath>
#include <map>

#ifdef _WIN32
	#include <Windows.h>
	#include <MMSystem.h>
#endif

using namespace std;
class actor : public baseObject
{
public:
	enum ActorState { IDLE, RUNNING, ROLLING, CROUCHING, SLIDING };
	ActorState m_state;
	bool m_bOnGround, m_bFacingRight, m_bIsRolling, m_bOnWall, m_bJump;
	double m_frame;

	//TODO: Read these values in from a config file of some sort
	void init()
	{
		m_state = IDLE;
		m_walkSpeed = 0.2f;
		m_runSpeed = 1.3f;
		m_jumpSpeed = 3.0;
		m_slideSpeed = -0.3f;
		m_rollSpeed = .8f;
		m_bOnGround = false;
		m_bOnWall = false;
		m_bIsRolling = false;
		m_bJump = false;
		m_health = 100.0f;
		m_damageDistance = 1.0f;
		m_fallStart = 0.0f;
		m_timeToImpact = 0.0f;
		m_frame = 0.0f;
		m_pSlidingOn = NULL;
		m_climbAngle = 45.0f;
		m_lastRollTime = 0;
		m_timeToTopSpeed = 0.25;
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
	void jump(float percent = 1.0f)
	{
		if( !m_bIsRolling && m_bOnGround )
		{
			m_frame = 0;
			m_bOnGround = false;
			m_movement.setVerticalComp(m_jumpSpeed * percent);
		}
	}
	void startRoll( const long double & elapsed );
	void takeFallDamage( float distance )
	{
		if( distance > m_damageDistance )
			m_health -= 10.0f;
	}

	void updateLocation(const long double & elapsed, const long double & prevElapsed, ground *belowPlayer,
		ground *abovePlayer, map<float, ground*> *nearby, map<int, bool> *keyMap);
	float getHealth(){ return m_health; }
	void takeDamage(float pain){ m_health -= pain; }
	void decayMult(const long double & elapsed);
	void updateMult(const long double & elapsed, string dir);
	void airFrameUpdate();
	ActorState groundFrameUpdate( const long double & elapsed, ground *abovePlayer );
	bool isMoving() { return abs(m_movement.getHorizComp()) > 0.05f; }
	bool moveDirection() { return m_movement.getHorizComp() > 0.0f; }
	void getNearbyWalls(const float & maxDistance, map<float, ground*> &nearby, list<ground>* allGround);

	float getRunSpeed(){ return m_runSpeed; }
	double getJumpSpeed(){ return m_jumpSpeed; }

protected:
	double m_lastRollTime, m_jumpSpeed;
	ground *m_pSlidingOn;
	bool m_bGravity;
	physics::vector m_movement;
	float m_health, m_fallStart, m_fallEnd, m_slideSpeed,
		m_walkSpeed, m_runSpeed, m_damageDistance, 
		m_timeToImpact, m_climbAngle, m_rollSpeed, m_oldHeight,
		m_timeToTopSpeed, m_timeToStop;
	float travelTime( float d, float v, float a );
	void applyGravity( double elapsed );
	float timeToCollisionY( baseObject one );
	void moveByTimeY( float time );
	void moveByTimeX( long double elapsed );
	void moveByDistanceX( float distance );
	void moveByDistanceY( float distance );
	bool facing( const baseObject &one );
	void changeHeight( float newHeight );
	void endRoll( const long double & elapsed );
};
#endif
