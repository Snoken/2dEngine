#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <list>
#include <map>
#include <exception>
using namespace std;

class Graph
{
public:
	struct Vertex
	{
		int id;
		int hVal;
		bool visited;
		Vertex(int id, int hVal): id(id), hVal(hVal), visited(false) {}
		bool operator==(const Vertex &other) const{ return id == other.id; }
	};
	struct Edge
	{
		Vertex *ends[2];
		int cost;
		Edge(Vertex* ends[2], int cost);
	};

	void addVertex(int id, int cost)
	{
		m_pVertices->push_back(Graph::Vertex(id, cost));
	}
	//add to adjacencies
	void addEdge(Edge* newEdge);
	Graph(list<Vertex>* verts, list<Edge>* edges = new list<Edge>());
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