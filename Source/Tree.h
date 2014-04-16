#ifndef TREE_H
#define TREE_H

#include "Graph.h"
#include "Search.h"
#include <deque>

class Tree
{
public:
	struct Path
	{
		Path()
		{
			verts = deque<Graph::Vertex*>();
			edges = deque<Graph::Edge*>();
		}
		/*Path(const Path& p)
		{
			verts = deque<Graph::Vertex*>(p.verts);
			edges = deque<Graph::Edge*>(p.edges);
		}*/
		void addToPath(Graph::Vertex* vert, Graph::Edge* edge)
		{
			verts.push_back(vert);
			edges.push_back(edge);
		}
		size_t size(){ return verts.size(); }
		deque<Graph::Vertex*> verts;
		deque<Graph::Edge*> edges;
	};

	struct Node
	{
		Graph::Vertex *vert;
		//this value tracks path cost only
		int cost;
		// track level in tree for use in algorithms that can skip around (A*)
		int level;
		list<Node*> children;
		// for searches needing it, this is used to store best path to each node
		Tree::Path path;

		Node(Graph::Vertex *vert, int cost, int level = 0, Path *path = NULL) : vert(vert), cost(cost), level(level)
		{
			children = list<Node*>();
			if (path != NULL)
				this->path = Path(*path);
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
		Path path;
		m_pNodes = new list<Node>();
		head = addNode(0, theHead);
		path.verts.push_back(theHead);
	}
	~Tree() 
	{
		delete m_pNodes;
	}
	Node* addAsChild(int cost, Graph::Vertex* vert, Node *parent, Tree::Path *path = NULL)
	{
		parent->addChild(addNode(cost, vert, parent->level + 1, path));
		return *--(parent->children.end());
	}
	//This returns a pointer to the new node for referencing later
	Node* addNode(int cost, Graph::Vertex* vert, int level = 0, Tree::Path *path = NULL )
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
	void removeNode(Node &node)
	{
		// first remove references as child
		for (list<Node>::iterator itr = m_pNodes->begin(); itr != m_pNodes->end(); ++itr)
		{
			for (list<Node*>::iterator childItr = itr->children.begin(); childItr != itr->children.end(); ++childItr)
			{
				if (**childItr == node)
				{
					itr->children.erase(childItr);
					break;
				}
			}
		}
		// then remove from list
		for (list<Node>::iterator itr = m_pNodes->begin(); itr != m_pNodes->end(); ++itr)
		{
			if (node == *itr)
				m_pNodes->erase(itr);
		}
	}

protected:
	list<Node>* m_pNodes;
	Node *head;
};

#endif