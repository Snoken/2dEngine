#include "bot.h"
void bot::updateLocation(const long double & elapsed, ground *belowPlayer,
	ground *abovePlayer, map<float, ground*> *nearby, map<int, bool> *keyMap)
{
	updateSpeed();
	//this condition is hit if bot has landed on ground after a jump has
	//	been completed
	if (m_locBeforeJump != primitives::vertex() && m_bOnGround)
	{
		//if bot ended up in same spot, it's stuck, cancel move order
		if (m_locBeforeJump == origin)
			m_dest = primitives::vertex();
		m_locBeforeJump = primitives::vertex();
	}

	ground* nearestFacing = NULL;
	float distance;
	double horiz = m_movement.getHorizComp();
	for (map<float, ground*>::iterator itr = nearby->begin(); itr != nearby->end();
		++itr)
	{
		if (collision::leftOf(*this, *itr->second) && horiz > 0)
		{
			nearestFacing = itr->second;
			distance = itr->first;
		}
		else if (collision::rightOf(*this, *itr->second) && horiz < 0)
		{
			nearestFacing = itr->second;
			distance = -itr->first;
		}
	}

	if (nearestFacing != NULL && needsToJump(nearestFacing, distance, elapsed))
	{
		m_locBeforeJump = origin;
		jump();
	}
	else if (m_dest != primitives::vertex() &&
		m_dest.y > yMin+.15)
	{
		m_locBeforeJump = origin;
		jump();
	}
	this->actor::updateLocation(elapsed, belowPlayer, abovePlayer, nearby, keyMap);
}

void bot::updateSpeed()
{
	//if no move order set, slow down and return
	if (m_dest == primitives::vertex())
	{
		decayMult();
		return;
	}

	//if dest reached, clear move order
	if (atDest())
		m_dest = primitives::vertex();
	else if (origin.x < m_dest.x)
		moveRight();
	else if (origin.x > m_dest.x)
		moveLeft();
}

bool bot::scalable(baseObject obj)
{
	physics::vector jumpVec = physics::vector(m_movement);
	jumpVec.setVerticalComp(m_jumpSpeed);
	return physics::apex(jumpVec, primitives::vertex(origin.x, yMin)) >= obj.yMax;
}

void bot::moveLeft()
{
	if (!m_bOnWall)
	{
		m_bFacingRight = false;
		updateMult();
	}
}

void bot::moveRight()
{
	if (!m_bOnWall)
	{
		m_bFacingRight = true;
		updateMult();
	}
}

bool bot::needsToJump(ground *nearest, float distance, const long double &elapsed)
{
	if (abs(distance) > abs(origin.x - m_dest.x))
		return false;
	else if (!scalable(*nearest))
		return false;
	double horiz = m_movement.getHorizComp();
	primitives::vertex target;
	if (distance > 0.0f)
		target = primitives::vertex(nearest->xMin, nearest->yMax);
	else
		target = primitives::vertex(nearest->xMax, nearest->yMax);
	physics::vector jumpVec = physics::vector(m_movement);
	jumpVec.setVerticalComp(m_jumpSpeed);
	double landTime = physics::timeToLand(jumpVec, primitives::vertex(origin.x, yMin), target);
	if (distance > horiz*landTime)
		return false;
	else
		return true;
}