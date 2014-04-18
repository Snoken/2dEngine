#include "bot.h"
bool bot::findPath(Graph* navSpace)
{
	Graph::Vertex* start = NULL;
	Graph::Vertex* end = NULL;
	for (list<Graph::Vertex>::iterator itr = navSpace->getVertices()->begin(); 
		itr != navSpace->getVertices()->end(); ++itr)
	{
		if (*itr->plat == *m_start)
			start = &*itr;
		if (*itr->plat == *m_dest)
			end = &*itr;
	}
	if (*start->plat == *end->plat)
	{ 
		// add code for simple move within current platform
		m_simpleMove = true;
		return false;
	}
	if (m_pSearch)
	{
		m_pSearch->resetTree(start);
		m_pSearch->setEnd(end);
	}
	else
		m_pSearch = new AStar(navSpace, start, end);
	m_pSearch->setVerbose(true);
	return m_pSearch->DoSearch();
}

void bot::updateLocation(const long double & elapsed, ground *belowPlayer,
	ground *abovePlayer, map<float, ground*> *nearby, map<int, bool> *keyMap)
{
	if (m_dest)
	{
		Graph::Edge* currEdge = NULL;
		// if we haven't gone anywhere on the path yet, set iterator to first edge in path
		if (!m_pSearch->getPath()->edges.empty())
		{
			currEdge = &*m_pSearch->getPath()->edges.front();

			// it can be assumed at this point that the bot is on the same platform as the 
			//	next node in the path

			//if close to navnode set vertical speed where it needs to be
			navNode nextNode = *currEdge->startNode;
			bool needsTo = needsToJump(origin, nextNode.origin, elapsed);
			if (needsTo && m_bOnGround || m_waitingToJump)
			{
				m_movement = currEdge->moveVector;
				origin = nextNode.origin;
				origin.y += height / 2;
				jump(currEdge->moveVector.getVertComp() / m_jumpSpeed);
				m_waitingToJump = false;
				m_pSearch->getPath()->edges.pop_front();
				// TODO: make the bot go approach node properly instead of magically getting right speed
			}
			else if (needsTo)
				m_waitingToJump = true;
			else if (origin.x < nextNode.origin.x)
				updateMult(elapsed, "right");
			else if (origin.x > nextNode.origin.x)
				updateMult(elapsed, "left");
		}
		else
			m_dest = NULL;
	}
	else if (m_simpleMove)
	{
		if (abs(origin.x - m_destLoc.x) < .03)
			m_simpleMove = false;
		else if (origin.x < m_destLoc.x)
			updateMult(elapsed, "right");
		else if (origin.x > m_destLoc.x)
			updateMult(elapsed, "left");
	}
	else if (m_bOnGround)
		decayMult(elapsed);
	actor::updateLocation(elapsed, belowPlayer, abovePlayer, nearby, keyMap);
}

void bot::colorPath(Tree::Path path)
{
	for (deque<Graph::Edge*>::iterator itr = path.edges.begin(); itr != path.edges.end(); ++itr)
		(*itr)->startNode->color[0] = (*itr)->startNode->color[1] = (*itr)->startNode->color[2] =
		(*itr)->startNode->color[3] = fmod((float) rand(), 255.0f) / 255.0f;
}

bool bot::scalable(baseObject obj)
{
	physics::vector jumpVec = physics::vector(m_movement);
	jumpVec.setVerticalComp(m_jumpSpeed);
	primitives::vertex maxHeight(origin.x, yMin);
	return physics::apex(jumpVec, maxHeight) >= obj.yMax;
}

bool bot::needsToJump(const primitives::vertex& startLoc, const primitives::vertex& jumpLoc,
	const long double & elapsed)
{
	primitives::vertex nextLoc(startLoc);
	nextLoc.x += m_movement.getHorizComp() * elapsed;
	nextLoc.y += m_movement.getVertComp() * elapsed;
	if (startLoc.x < jumpLoc.x && nextLoc.x > jumpLoc.x)
		return true;
	else if (startLoc.x > jumpLoc.x && nextLoc.x < jumpLoc.x)
		return true;
	return false;
}
