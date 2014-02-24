#ifndef ASTAR_H
#define ASTAR_H

#include "Search.h"

class AStar : public Search
{
public:
	AStar(Graph* theGraph, Graph::Vertex* start, Graph::Vertex* end) :
		Search(theGraph, start, end) {}

	deque<Tree::Node*>::iterator OpenListFind(Tree::Node* find);
	virtual bool DoSearch(Tree::Node* currNode = NULL);
};

#endif