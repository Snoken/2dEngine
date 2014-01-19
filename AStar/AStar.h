#ifndef ASTAR_H
#define ASTAR_H

#include <algorithm>
#include "Tree.h"

class AStar
{
public:
	AStar(Graph::Vertex *start, Graph *toSearch): toSearch(toSearch)
	{
		theTree = new Tree(start);
		openList = list<Tree::Node*>();
		closedList = list<Tree::Node*>();
	}
	Tree *theTree;
	Tree::Path search(Graph::Vertex *end);

private:
	list<Tree::Path> m_shortests;
	list<Tree::Node*> openList;
	list<Tree::Node*> closedList;
	Graph *toSearch;
};
#endif