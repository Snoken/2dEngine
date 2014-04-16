#ifndef SEARCH_H
#define SEARCH_H

#include "Tree.h"
#include <iostream>
#include <deque>
#include <algorithm>

class Search
{
public:
	Search(Graph* theGraph, Graph::Vertex* start, Graph::Vertex* end) :
		m_searchSpace(theGraph), m_end(end), m_openList(deque<Tree::Node*>()),
		m_closedList(map<Graph::Vertex*, Tree::Node*>()), m_expansions(0)
	{
		m_verbose = false;
		m_searchTree = new Tree(start);
	}
	~Search()
	{
		delete m_searchTree;
	}

	// The return value of this function indicates whether a path from start to end was found.
	virtual bool DoSearch(Tree::Node* currNode) = 0;
	Tree::Path* getPath(){ return &m_path; }
	void displayPath(Tree::Node* node = NULL)
	{
		cout << "Length: " << m_path.verts.size() << " nodes, Cost: " << pathCost << endl;
		deque<Graph::Vertex*>::iterator itr, enditr;
		if (node == NULL)
		{
			itr = m_path.verts.begin();
			enditr = m_path.verts.end();
		}
		else
		{
			itr = node->path.verts.begin();
			enditr = node->path.verts.end();
		}
		cout << "\t(" << (*itr)->plat->origin.x << ", " << (*itr)->plat->origin.y << ") ";
		++itr;
		for (; itr != enditr; ++itr)
		{
			cout << "-> (" << (*itr)->plat->origin.x << ", " << (*itr)->plat->origin.y << ") ";
		}
		cout << endl;
	}
	void displayOpenList()
	{
		deque<Tree::Node*> listCopy(m_openList);
		cout << "Open List: " << endl;
		while (!listCopy.empty())
		{
			Tree::Node* nextNode = listCopy.front();
			cout << "\tLoc: (" << nextNode->vert->plat->origin.x << ", " <<
				nextNode->vert->plat->origin.y << ") Cost: " << nextNode->cost << " hVal: "
				<< nextNode->vert->hVal << endl;
			listCopy.pop_front();
		}
	}
	void setVerbose( bool verb ) { m_verbose = verb; }

protected:
	Graph* m_searchSpace;
	Tree::Path m_path;
	int pathCost;
	deque<Tree::Node*> m_openList;
	// map structure being used for quick lookup of where vertex is in tree
	map<Graph::Vertex*, Tree::Node*> m_closedList;
	Graph::Vertex* m_end;
	Tree* m_searchTree;
	int m_expansions;
	bool m_verbose;
};

#endif