#ifndef BOT_H
#define BOT_H
#include "actor.h"
#include "primitives.h"
#include "AStar.h"

class bot : public actor
{
public:
	bot(primitives::vertex origin, float width, float height, GLuint texture = 0):
		actor(origin, width, height, texture)
	{
		m_dest = NULL;
		m_pSearch = NULL;
		m_simpleMove = false;
		m_waitingToJump = false;
	}
	bool findPath(Graph* navSpace);
	void colorPath(Tree::Path path);
	void setDest(primitives::vertex destLoc, ground * below, ground* dest, Graph* navSpace) {
		if (!dest)
			return;
		m_start = below;
		m_dest = dest;
		navSpace->updateCosts(origin);
		m_destLoc = destLoc;
		if (findPath(navSpace))
		{
			m_pSearch->displayPath();
			colorPath(*m_pSearch->getPath());
		}
		else
		{
			m_dest = NULL;
		}
	}
	void updateLocation(const long double & elapsed, ground *belowPlayer,
		ground *abovePlayer, map<float, ground*> *nearby, map<int, bool> *keyMap);

private:
	primitives::vertex m_destLoc;
	ground* m_start;
	ground* m_dest;
	ground* m_nearestFacing;
	ground* m_below;
	AStar* m_pSearch;
	bool m_simpleMove;
	bool m_waitingToJump;
	bool scalable(baseObject obj);

	//Will move as close as possible
	void updateSpeed();
	bool pickDirection();
	void moveLeft();
	void moveRight();
	bool needsToJump(const primitives::vertex& startLoc, const primitives::vertex& jumpLoc,
		const long double & elapsed);

};

#endif