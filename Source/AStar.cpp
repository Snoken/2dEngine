#include "AStar.h"
#include <stack>
#include <iostream>
#include <functional>

// comparison, not case sensitive.
bool AStarCompareFunc(const Tree::Node* first, const Tree::Node* second)
{
	return (first->cost + first->vert->hVal) < (second->cost + second->vert->hVal);
}

deque<Tree::Node*>::iterator AStar::OpenListFind(Tree::Node* find)
{
	//scan deque for vertex
	deque<Tree::Node*>::iterator itr = m_openList.begin();
	while (itr != m_openList.end())
	{
		if ((*itr)->vert->plat == find->vert->plat)
			return itr;
		++itr;
	}
	return m_openList.end();
}

bool AStar::inPath(Graph::Vertex* vert, Tree::Path path)
{
	if (path.size() == 0)
		return false;
	return find(path.verts.begin(), path.verts.end(), vert) != path.verts.end();
}

bool AStar::DoSearch(Tree::Node* currNode)
{
	//add head of tree to stack to start it off
	m_openList.push_back(m_searchTree->getHead());
	m_searchTree->getHead()->path.verts.push_back(m_searchTree->getHead()->vert);

	while (!m_openList.empty())
	{
		// Grab top of stack for next node
		currNode = m_openList.front();
		m_openList.pop_front();

		// Add to closed list
		m_closedList.insert(make_pair(currNode->vert, currNode));

		// pull path from new node
		m_path = currNode->path;

		if (m_verbose)
		{
			if (m_path.size() != 1)
				displayPath(currNode);
			cout << "Expanding platform at (" << currNode->vert->plat->origin.x << ", " << 
				currNode->vert->plat->origin.y << ") Cost: " << currNode->cost << " hVal: " 
				<< currNode->vert->hVal << endl << endl;
		}

		// Return true if dest has been reached
		if (*currNode->vert->plat == *m_end->plat)
		{
			pathCost = currNode->cost;
			return true;
		}

		// fetch edges connected to current vertex
		list<Graph::Edge*> conns = m_searchSpace->getConnectedEdges(currNode->vert);

		// create a node for each connection if one of following is true:
		//	- The vertex has not been visited
		//	- The cost to get to the vertex via this path is lower than the current best (open and closed list)
		for (list<Graph::Edge*>::iterator itr = conns.begin(); itr != conns.end(); ++itr)
		{
			// Compute cost of this path
			int nextCost = currNode->cost + (*itr)->cost;

			// figure out which end of the edge is the vertex in question
			Graph::Vertex* endOfEdge = NULL;
			if ((*itr)->ends[0] == currNode->vert)
				endOfEdge = (*itr)->ends[1];
			else
				endOfEdge = (*itr)->ends[0];

			Tree::Node* findMe;
			findMe = new Tree::Node(endOfEdge, 0);

			// check if already on open list
			deque<Tree::Node*>::iterator openListLoc = OpenListFind(findMe);
			delete findMe;

			// check for closed list entry
			map<Graph::Vertex*, Tree::Node*>::iterator closedListLoc = m_closedList.find(endOfEdge);

			Tree::Path path(m_path);
			path.addToPath(endOfEdge, *itr);

			// Case 1: not on either list
			//	Action: add to open list
			//		add to search tree
			if (openListLoc == m_openList.end() && closedListLoc == m_closedList.end())
			{
				m_searchTree->addAsChild(nextCost, endOfEdge, currNode, &path);
				m_openList.push_back(*--currNode->children.end());
			}
			// Case 2: already on open list
			else if (openListLoc != m_openList.end() && nextCost < (*openListLoc)->cost)
			{
				// Case 2.1: lower cost
				//	Action: replace current open list entry
				//		delete existing node, create new
				Tree::Node* oldNode = *openListLoc;
				m_openList.erase(openListLoc);

				m_searchTree->addAsChild(nextCost, endOfEdge, currNode, &path);
				m_openList.push_back(*--currNode->children.end());

				// Case 2.2: equal or higher cost
				//	Action: do nothing
			}
			// Case 3: already on closed list
			else if (closedListLoc != m_closedList.end() && nextCost < closedListLoc->second->cost)
			{
				// Case 3.1: lower cost
				//	Action: remove from closed list, add to open list
				//		delete existing node, create new
				Tree::Node* oldNode = closedListLoc->second;
				m_closedList.erase(closedListLoc);

				m_searchTree->addAsChild(nextCost, endOfEdge, currNode, &path);
				m_openList.push_back(*--currNode->children.end());
				// Case 3.2: equal or higher cost
				//	Action: do nothing
			}
		}
		
		// append to open list
		sort(m_openList.begin(), m_openList.end(), AStarCompareFunc);
		if (m_verbose)
		{
			cout << "Open List Size: " << m_openList.size() << endl;
			displayOpenList();
		}
	}
	// default case, no path found
	m_path.verts.clear();
	return false;
}