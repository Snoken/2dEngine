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
		m_bFollowing = false;
		m_lastFollowUpdate = 0.0;
	}
	bool findPath(Graph* navSpace);
	void colorPath(Tree::Path path);
	void setDest(primitives::vertex destLoc, ground * below, ground* dest, Graph* navSpace) {
		if (!dest)
			return;
		m_start = below;
		m_dest = dest;
		navSpace->updateCosts(origin, m_destLoc);
		m_destLoc = destLoc;
		if (findPath(navSpace))
		{
			//m_pSearch->displayPath();
			colorPath(*m_pSearch->getPath());
		}
		else
		{
			m_dest = NULL;
		}
	}
	// this function is to guard against key handling oddities (all but 
	//	the quickest of taps tends to result in at least 2 calls to keydown)
	void toggleFollow(const long double & elapsed)
	{
		if (m_lastFollowUpdate == 0 || elapsed - m_lastFollowUpdate > 0.1)
		{
			m_bFollowing = !m_bFollowing;
			m_lastFollowUpdate = elapsed;
		}
	}
	bool m_bFollowing;
	double m_lastFollowUpdate;
	void updateLocation(const long double & elapsed, const long double & prevElapsed, ground *belowPlayer,
		ground *abovePlayer, map<float, ground*> *nearby, map<int, bool> *keyMap);

private:
	primitives::vertex m_destLoc;
	ground* m_start;
	ground* m_dest;
	ground* m_nearestFacing;
	ground* m_below;
	AStar* m_pSearch;
	bool m_simpleMove;
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