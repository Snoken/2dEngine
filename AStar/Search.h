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
		m_searchTree = new Tree(start);
	}
	// The return value of this function indicates whether a path from start to end was found.
	virtual bool DoSearch(Tree::Node* currNode) = 0;
	deque<Graph::Vertex*> getPath(){ return m_path; }
	void displayPath(Tree::Node* node = NULL)
	{
		deque<Graph::Vertex*>::iterator itr, enditr;
		if (node == NULL)
		{
			itr = m_path.begin();
			enditr = m_path.end();
		}
		else
		{
			itr = node->path.begin();
			enditr = node->path.end();
		}
		for (; itr != enditr; ++itr)
			std::cout << (*itr)->id << "  ";
		std::cout << std::endl << endl;
	}
	void displayOpenList()
	{
		deque<Tree::Node*> listCopy(m_openList);
		cout << "Open List: " << endl;
		while (!listCopy.empty())
		{
			Tree::Node* nextNode = listCopy.front();
			cout << "\tid: " << nextNode->vert->id << " g: " << nextNode->cost << 
				" h: " << nextNode->vert->hVal << endl;
			listCopy.pop_front();
		}
	}


protected:
	Graph* m_searchSpace;
	deque<Graph::Vertex*> m_path;
	deque<Tree::Node*> m_openList;
	// map structure being used for quick lookup of where vertex is in tree
	map<Graph::Vertex*, Tree::Node*> m_closedList;
	Graph::Vertex* m_end;
	Tree* m_searchTree;
	int m_expansions;
};

#endif