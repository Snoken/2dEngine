#ifndef ACTOR_H
#define ACTOR_H
#include "baseObject.h"
#include "ground.h"
#include "collision.h"
#include <map>

using namespace std;
class actor : public baseObject
{
public:
	bool m_bOnGround, m_bFacingRight;
	double m_frame;

	void init()
	{
		m_walkSpeed = 0.2f;
		m_runSpeed = 1.0f;
		m_jumpSpeed = 3.0f;
		m_vertSpeed = 0.0f;
		m_slideSpeed = -0.3f;
		m_bOnGround = false;
		m_bOnWall = false;
		m_health = 100.0f;
		m_damageDistance = 1.0f;
		m_fallStart = 0.0f;
		m_timeToImpact = 0.0f;
		m_multiplier = 0.0f;
		m_frame = 0.0f;
		m_pSlidingOn = NULL;
		m_climbAngle = 45.0f;
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
		m_bOnGround = old.m_bOnGround;
	}
	actor( vertex origin, float width, float height, GLuint texture = 0 ):baseObject(origin, width, height, texture)
	{
		init();
	}
	void jump()
	{
		if( m_bOnGround )
		{
			m_frame = 0;
			m_bOnGround = false;
			m_vertSpeed = m_jumpSpeed;
		}
	}
	void takeFallDamage( float distance )
	{
		if( distance > m_damageDistance )
			m_health -= 10.0f;
	}

	void updateLocation( const long double & elapsed, ground *belowPlayer, 
		ground *abovePlayer, list<ground> *nearby, map<int, bool> *keyMap );
	float getHealth(){ return m_health; }
	void decayMult();
	void updateMult();
	void airFrameUpdate( bool &bSliding );
	void groundFrameUpdate( bool &running );

private:
	double m_multiplier;
	ground *m_pSlidingOn;
	bool m_bOnWall, m_bGravity;
	float m_health, m_fallStart, m_fallEnd, m_vertSpeed, m_slideSpeed,
		m_walkSpeed, m_runSpeed, m_jumpSpeed, m_damageDistance, 
		m_timeToImpact, m_climbAngle;
	float travelTime( float d, float v, float a );
	void applyGravity( double elapsed );
	float timeToCollisionY( baseObject one );
	void moveByTimeY( float time );
	void moveByTimeX( long double elapsed );
	void moveByDistanceX( float distance );
	void moveByDistanceY( float distance );
	//not currently in use
	bool facing( const baseObject &one );
};
#endif