#ifndef TREE_H
#define TREE_H

#include "Graph.h"

class Tree
{
public:
	struct Node
	{
		Graph::Vertex *vert;
		int cost;
		list<Node*> children;

		Node(Graph::Vertex *vert, int cost): vert(vert), cost(cost)
		{
			children = list<Node*>();
		}

		void addChild(Node *newChild)
		{ 
			children.push_back(newChild); 
		}
	};
	Tree(Graph::Vertex *theHead)
	{
		nodes = list<Node>();
		head = addNode(0, theHead);
	}
	Node* addAsChild(int cost, Graph::Vertex* vert, Node *parent)
	{
		parent->addChild(addNode(cost, vert));
		return *--(parent->children.end());
	}
	//This returns a pointer to the new node for referencing later
	Node* addNode(int cost, Graph::Vertex* vert)
	{
		nodes.push_back(Node(vert, cost));
		return &(*--nodes.end());
	}
	Node* getHead()
	{
		return &(*nodes.begin());
	}
	bool contains(Graph::Vertex *vert)
	{ 
		for(list<Node>::iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
		{
			if(*itr->vert == *vert)
				return true;
		}
		return false;
	}
	struct Path
	{
		list<Tree::Node*> nodes;
		Path(){ nodes = list<Tree::Node*>(); }
		void addToPath(Tree::Node *newNode){ nodes.push_back(newNode); }
		int cost()
		{
			int cost = 0;
			list<Tree::Node*>::iterator itr = nodes.begin();
			while(itr != nodes.end())
			{
				cost += (*itr)->cost;
				++itr;
			}
			cost += (*itr)->vert->hVal;
			return cost;
		}
		bool operator==(const Path &other) const
		{ 
			return *(*--nodes.end())->vert == *(*--other.nodes.end())->vert; 
		}
	};

private:
	list<Node> nodes;
	Node *head;
};

#endif