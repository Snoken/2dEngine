#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <list>
#include <map>
#include <exception>
#include <iostream>
#include "ground.h"
#include "navNode.h"

using namespace std;

class Graph
{
public:
	struct Vertex
	{
		const ground* plat;
		double hVal;
		bool visited;
		Vertex(const ground* plat, double hVal): plat(plat), hVal(hVal), visited(false) {}
		bool operator==(const Vertex &other) const{ return *plat == *other.plat; }
	};
	struct Edge
	{
		Vertex *ends[2];
		navNode* startNode;
		physics::vector moveVector;
		double cost;
		double travelTime;
		Edge(Vertex* ends[2], double cost, double travelTime, navNode* startNode, physics::vector moveVector);
	};

	void addVertex(const ground* plat, int hVal)
	{
		m_pVertices->push_back(Graph::Vertex(plat, hVal));
	}
	//add to adjacencies
	void addEdge(Edge newEdge);
	Graph(list<Vertex>* verts = new list<Vertex>(), list<Edge>* edges = new list<Edge>(), 
		map<Vertex*, list<Edge*>> adjs = map<Vertex*, list<Edge*>>());
	~Graph() 
	{ 
		delete m_pEdges;
		delete m_pVertices;
	}
	void updateCosts(primitives::vertex startLoc, primitives::vertex endLoc);
	list<Edge>* getEdges(){ return m_pEdges; }
	list<Vertex>* getVertices(){ return m_pVertices; }
	list<Edge*> getConnectedEdges(Vertex *theVertex);
	friend ostream& operator<<(ostream& stream, const Graph& obj);

private:
	list<Vertex>* m_pVertices;
	list<Edge>* m_pEdges;
	map<Vertex*, list<Edge*>> m_adjacencies;
};

#endif