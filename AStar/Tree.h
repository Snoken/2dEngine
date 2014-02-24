#ifndef TREE_H
#define TREE_H

#include "Graph.h"
#include <deque>

class Tree
{
public:
	struct Node
	{
		Graph::Vertex *vert;
		//this value tracks path cost only
		int cost;
		// track level in tree for use in algorithms that can skip around (A*)
		int level;
		list<Node*> children;
		// for searches needing it, this is used to store best path to each node
		deque<Graph::Vertex*> path;

		Node(Graph::Vertex *vert, int cost, int level = 0, deque<Graph::Vertex*> *path = NULL) : vert(vert), cost(cost), level(level)
		{
			children = list<Node*>();
			if (path != NULL)
				this->path = deque<Graph::Vertex*>(*path);
		}

		virtual ~Node() {}
		void addChild(Node *newChild)
		{ 
			children.push_back(newChild); 
		}
		list<Node*> getChildren(){ return children; }
		// Treat nodes as equal if they point to same vertex
		bool operator==(const Node &other) const
		{
			return vert == other.vert; 
		}
	};
	Tree(Graph::Vertex *theHead)
	{
		deque<Graph::Vertex*> path;
		path.push_back(theHead);
		m_pNodes = new list<Node>();
		head = addNode(0, theHead);
	}
	virtual ~Tree() {}
	Node* addAsChild(int cost, Graph::Vertex* vert, Node *parent, deque<Graph::Vertex*> *path = NULL)
	{
		parent->addChild(addNode(cost, vert, parent->level + 1, path));
		return *--(parent->children.end());
	}
	//This returns a pointer to the new node for referencing later
	Node* addNode(int cost, Graph::Vertex* vert, int level = 0, deque<Graph::Vertex*> *path = NULL )
	{
		m_pNodes->push_back(Node(vert, cost, level, path));
		return &(*--m_pNodes->end());
	}
	Node* getHead()
	{
		return &(*m_pNodes->begin());
	}
	bool contains(Graph::Vertex *vert)
	{ 
		for (list<Node>::iterator itr = m_pNodes->begin(); itr != m_pNodes->end(); ++itr)
		{
			if(*itr->vert == *vert)
				return true;
		}
		return false;
	}

protected:
	list<Node>* m_pNodes;
	Node *head;
};

#endif