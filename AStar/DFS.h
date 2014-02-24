#ifndef DFS_H
#define DFS_H

#include "Search.h"

class DFS : public Search
{
public:
	DFS(Graph* theGraph, Graph::Vertex* start, Graph::Vertex* end) :
		Search(theGraph, start, end) {}
	
	virtual bool DoSearch(Tree::Node* currNode = NULL);
};

#endif