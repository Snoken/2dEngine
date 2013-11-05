#ifndef BOT_H
#define BOT_H
#include "actor.h"
#include "primitives.h"

class bot : public actor
{
public:
	void setDest(primitives::vertex dest) { m_dest = dest; }
	bot(primitives::vertex origin, float width, float height, GLuint texture = 0):
		actor(origin, width, height, texture)
	{
		m_dest = primitives::vertex();
		m_locBeforeJump = primitives::vertex();
	}
	void updateLocation(const long double & elapsed, ground *belowPlayer,
		ground *abovePlayer, map<float, ground*> *nearby, map<int, bool> *keyMap);

private:
	primitives::vertex m_dest, m_locBeforeJump;
	ground* m_nearestFacing;
	//TODO: as of now, simply whether a jump is high enough, later impl path
	//	finding for more complex navigation
	bool scalable(baseObject obj);

	//Will move as close as possible
	void updateSpeed();
	bool pickDirection();
	void moveLeft();
	void moveRight();
	bool needsToJump(ground *nearest, float distance, const long double &elapsed);
	bool atDest()
	{
		return origin.x < (m_dest.x + .05) && origin.x > (m_dest.x - .05) &&
			origin.y < (m_dest.y + .1) && origin.y > (m_dest.y - .1);
	}
};

#endif