#ifndef BFS_H
#define BFS_H

#include "Search.h"

class BFS : public Search
{
public:
	BFS(Graph* theGraph, Graph::Vertex* start, Graph::Vertex* end) :
		Search(theGraph, start, end) {}

	virtual bool DoSearch(Tree::Node* currNode = NULL);
};

#endif