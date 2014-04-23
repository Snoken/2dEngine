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
	if(!start || !end)
		return false;
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

void bot::updateLocation(const long double & elapsed, const long double & prevElapsed, ground *belowPlayer,
	ground *abovePlayer, map<float, ground*> *nearby, map<int, bool> *keyMap)
{
	const long double timeDiff = elapsed - prevElapsed;

	double horizSpeed = abs(m_movement.getHorizComp());
	double stopTime = (horizSpeed / m_runSpeed) * m_timeToStop;
	float stopDistance = horizSpeed * stopTime + 0.5f * (horizSpeed / stopTime) * pow(stopTime, 2.0f);
	if (isMoving() && m_simpleMove && stopDistance >= abs(origin.x - m_destLoc.x))
	{
		decayMult(timeDiff);
		//this will be hit once the bot has arrived
		if (!isMoving())
			m_simpleMove = false;
	}
	else if (m_dest)
	{
		Graph::Edge* currEdge = NULL;
		// if we haven't gone anywhere on the path yet, set iterator to first edge in path
		if (!m_pSearch->getPath()->edges.empty())
		{
			currEdge = &*m_pSearch->getPath()->edges.front();

			// it can be assumed at this point that the bot is on the same platform as the 
			//	next node in the path

			navNode nextNode = *currEdge->startNode;
			bool needsTo = needsToJump(origin, nextNode.origin, timeDiff);
			//if close to navnode set vertical speed where it needs to be
			if (needsTo && m_bOnGround)
			{
				m_movement = currEdge->moveVector;
				origin = nextNode.origin;
				origin.y += height / 2;
				if (currEdge->moveVector.getVertComp() != 0.0)
					jump(currEdge->moveVector.getVertComp() / m_jumpSpeed + .1);
				m_pSearch->getPath()->edges.pop_front();
				// TODO: make the bot go approach node properly instead of magically getting right speed
			}
			else if (origin.x < nextNode.origin.x)
				updateMult(timeDiff, "right");
			else if (origin.x > nextNode.origin.x)
				updateMult(timeDiff, "left");
		}
		else
		{
			m_dest = NULL;
			// toggle simple move on so bot will walk to correct location on platform
			m_simpleMove = true;
		}
	}
	else if (m_simpleMove)
	{
		if (abs(origin.x - m_destLoc.x) < .03)
			m_simpleMove = false;
		else if (origin.x < m_destLoc.x)
			updateMult(timeDiff, "right");
		else if (origin.x > m_destLoc.x)
			updateMult(timeDiff, "left");
	}
	else if (m_bOnGround)
		decayMult(timeDiff);
	actor::updateLocation(elapsed, prevElapsed, belowPlayer, abovePlayer, nearby, keyMap);
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
	//only jump if actually on right platform
	if (abs(yMin - jumpLoc.y) > 0.005f)
		return false;
	primitives::vertex nextLoc(startLoc);
	nextLoc.x += m_movement.getHorizComp() * elapsed;
	nextLoc.y += m_movement.getVertComp() * elapsed;
	if (startLoc.x < jumpLoc.x && nextLoc.x > jumpLoc.x)
		return true;
	else if (startLoc.x > jumpLoc.x && nextLoc.x < jumpLoc.x)
		return true;
	return false;
}
