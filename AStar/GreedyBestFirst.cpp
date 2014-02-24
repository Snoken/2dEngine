#include "GreedyBestFirst.h"
#include <iostream>

// comparison, not case sensitive.
bool CompareFunc(const Tree::Node* first, const Tree::Node* second)
{
	return first->cost < second->cost;
}

bool GreedyBestFirst::DoSearch(Tree::Node* currNode)
{
	if (currNode == NULL)
		currNode = m_searchTree->getHead();

	cout << "Visiting vertex with ID of " << currNode->vert->id << " (cost "
		<< currNode->cost << ")" << endl;
	// Add current vertex to path
	m_path.push_back(currNode->vert);

	// Return true if dest has been reached
	if (currNode->vert == m_end)
		return true;

	// fetch edges connected to current vertex
	list<Graph::Edge*> conns = m_searchSpace->getConnectedEdges(currNode->vert);

	// create a node for each connection
	for (list<Graph::Edge*>::iterator itr = conns.begin(); itr != conns.end(); ++itr)
	{
		// Figure out which end is the start, add child accordingly if not already visited
		if ((*itr)->ends[0] == currNode->vert && !(*itr)->ends[1]->visited)
		{
			int nextCost = currNode->cost + (*itr)->ends[1]->hVal;
			m_searchTree->addAsChild(nextCost, (*itr)->ends[1], currNode);
		}
		else if (!(*itr)->ends[0]->visited)
		{
			int nextCost = currNode->cost + (*itr)->ends[0]->hVal;
			m_searchTree->addAsChild(nextCost, (*itr)->ends[0], currNode);
		}
	}

	// fetch children of current node
	list<Tree::Node*> currChildren = currNode->getChildren();

	// if no children and not at dest, not on correct path
	//	so remove current node from path and go up a level
	if (currChildren.empty())
	{
		m_path.pop_back();
		return false;
	}

	// replace open list (this is greedy, no regression)
	m_openList.clear();
	m_openList.insert(m_openList.begin(), currChildren.begin(), currChildren.end());
	sort(m_openList.begin(), m_openList.end(), CompareFunc);
		
	// get first entry in open list and remove from open list
	Tree::Node* nextNode = *m_openList.begin();
	m_openList.pop_front();

	// mark current vertex visited
	currNode->vert->visited = true;

	// recurse on next entry in open list
	if (DoSearch(nextNode))
		return true;

	// default case, no path found
	m_path.clear();
	return false;
}