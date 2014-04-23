#ifndef PROJECTILE_H
#define PROJECTILE_H
#include "primitives.h"
#include "physics.h"
#include "actor.h"

class projectile
{
public:
	projectile(primitives::vertex orig, primitives::vertex dest, float v, 
		actor* owner, int dmg = 15, bool drop = false) : m_drops(drop), m_loc(orig),
		m_damage(dmg), m_owner(owner)
	{
		m_movement = physics::vector(v, orig, dest);
		m_elapsed = m_prevElapsed = m_timeAlive = 0;
	}

	void update(long double elapsed)
	{
		if (m_timeAlive == 0)
			m_timeAlive = elapsed;
		else
			m_timeAlive += elapsed;
		this->move(elapsed);
	}

	inline bool timedOut()
	{
		if (m_timeAlive > 30)
			return true;
		return false;
	}

	int getDamage(){ return m_damage; }
	primitives::vertex getLoc(){ return m_loc; }
	double getRotation(){ return m_movement.angle; }
	bool operator==(const projectile& rhs)
	{
		return m_movement == rhs.m_movement &&
			m_loc == rhs.m_loc;
	}
	physics::vector m_movement;
	actor* getOwner() { return m_owner; }

private:
	primitives::vertex m_loc;
	bool m_drops;
	long double m_prevElapsed, m_elapsed, m_timeAlive;
	int m_damage;
	actor* m_owner;
	//TODO: impl teams for online play
	//team teamId;

	void move(long double elapsed)
	{
		m_loc.x += (GLfloat)(elapsed * m_movement.getHorizComp());
		//apply gravity if this projectile drops
		if (m_drops)
			m_movement.applyGravity(elapsed);
		m_loc.y += (GLfloat) (elapsed * m_movement.getVertComp());
	}
};
#endif