#ifndef GREEDYBESTFIRST_H
#define GREEDYBESTFIRST_H

#include "Search.h"

class GreedyBestFirst : public Search
{
public:
	GreedyBestFirst(Graph* theGraph, Graph::Vertex* start, Graph::Vertex* end) :
		Search(theGraph, start, end) {}

	//bool CompareFunc(const Tree::Node& first, const Tree::Node& second);
	virtual bool DoSearch(Tree::Node* currNode = NULL);
};

#endif