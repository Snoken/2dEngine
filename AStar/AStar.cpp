#include "AStar.h"
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
		if (*(*itr)->vert == *find->vert)
			return itr;
		++itr;
	}
	return m_openList.end();
}

bool AStar::DoSearch(Tree::Node* currNode)
{
	if (currNode == NULL)
		currNode = m_searchTree->getHead();

	m_path.push_back(currNode->vert);
	currNode->path = m_path;

	// Add to closed list
	m_closedList.insert(make_pair(currNode->vert, currNode));

	// Return true if dest has been reached
	if (currNode->vert == m_end)
		return true;

	// fetch edges connected to current vertex
	list<Graph::Edge*> conns = m_searchSpace->getConnectedEdges(currNode->vert);

	// create a node for each connection if one of following is true:
	//	- The vertex has not been visited
	//	- The cost to get to the vertex via this path is lower than the current best (open and closed list)
	for (list<Graph::Edge*>::iterator itr = conns.begin(); itr != conns.end(); ++itr)
	{
		// Compute cost of this path
		int nextCost = currNode->cost + (*itr)->cost;

		// Figure out which end is the start, add child accordingly if not already visited
		if ((*itr)->ends[0] == currNode->vert && !(*itr)->ends[1]->visited)
		{
			m_searchTree->addAsChild(nextCost, (*itr)->ends[1], currNode, &m_path);
			(*itr)->ends[1]->visited = true;
		}

		else if (!(*itr)->ends[0]->visited)
		{
			m_searchTree->addAsChild(nextCost, (*itr)->ends[0], currNode, &m_path);
			(*itr)->ends[0]->visited = true;
		}
		// If visited, check if better match
		else
		{
			Tree::Node* findMe;
			// figure out which end we need to find
			if ((*itr)->ends[0] == currNode->vert)
				findMe = new Tree::Node((*itr)->ends[1], 0);
			else
				findMe = new Tree::Node((*itr)->ends[0], 0);

			// check if already on open list
			deque<Tree::Node*>::iterator openListLoc = OpenListFind(findMe);
			if (openListLoc != m_openList.end() && nextCost < (*openListLoc)->cost)
			{
				cout << "Found better path for node on open list\n";
				// if new cost is better remove old node, add new
				m_openList.erase(openListLoc);
				m_searchTree->addAsChild(nextCost, findMe->vert, currNode, &m_path);
				continue;
			}
			else if (openListLoc != m_openList.end())
				continue;

			// compare existing cost to new cost
			map<Graph::Vertex*, Tree::Node*>::iterator loc =
				m_closedList.find(findMe->vert);
			// if lower cost path found
			if (nextCost < loc->second->cost)
			{
				cout << "Found better path for " << loc->second->vert->id << "(prev: " <<
					loc->second->cost << " new: " << nextCost << endl;
				// fetch parent (second to last entry in path on node)
				Graph::Vertex* parentVert =
					*--loc->second->path.end();
				// look up address of node current storing parentVert, has to be on
				//	close list if visited
				Tree::Node* parentNode = m_closedList.find(parentVert)->second;
				// remove the node that needs updating from the old parent
				parentNode->children.remove(loc->second);

				// update path on node
				loc->second->path = deque<Graph::Vertex*>(m_path);

				// update cost on node
				loc->second->cost = nextCost;

				//add to children of currNode
				currNode->addChild(loc->second);

				// remove from closed list
				m_closedList.erase(loc);
			}
		}
	}

	// fetch children of current node
	list<Tree::Node*> currChildren = currNode->getChildren();

	// if no children and not at dest, not on correct path
	if (currChildren.empty())
	{
		return false;
	}

	// append to open list
	m_openList.insert(m_openList.end(), currChildren.begin(), currChildren.end());
	sort(m_openList.begin(), m_openList.end(), AStarCompareFunc);
	displayOpenList();

	// mark current vertex visited
	currNode->vert->visited = true;

	while (!m_openList.empty())
	{
		// get first entry in open list and remove from open list
		Tree::Node* nextNode = *m_openList.begin();
		m_openList.pop_front();

		// set path to next node's path
		m_path = nextNode->path;

		displayPath(currNode);
		// recurse on next entry in open list
		cout << "Expanding node " << nextNode->vert->id << endl;
		if (DoSearch(nextNode))
			return true;
	}

	// default case, no path found
	m_path.clear();
	return false;
}