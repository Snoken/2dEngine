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
		Vertex(int id, int hVal): id(id), hVal(hVal){}
		bool operator==(const Vertex &other) const{ return id == other.id; }
	};
	struct Edge
	{
		Vertex *ends[2];
		int cost;
		Edge(Vertex* one, Vertex* two, int cost): cost(cost)
		{
			if( one == NULL )
				throw exception("Vertex one is NULL");
			if( two == NULL )
				throw exception("Vertex two is NULL");
			ends[0] = one;
			ends[1] = two;
		}
	};

	void addVertex(int id, int cost)
	{
		m_vertices.push_back(Graph::Vertex(id, cost));
	}
	//add to adjacencies
	void addEdge(Vertex* ends[2], int cost)
	{
		map<Vertex*, list<Vertex*>>::iterator loc = m_adjacencies.find(ends[0]);
		//if there are already adjacencies for the given first vertex, 
		//	add an entry.
		if(loc != m_adjacencies.end())
		{
			//this assumes there aren't duplicate edges
			loc->second.push_back(ends[1]);
		}
		else
		{
			list<Vertex*> adjs;
			adjs.push_back(ends[1]);
			m_adjacencies.insert(make_pair(ends[0], adjs));
		}

		//same process for second vertex
		loc = m_adjacencies.end();
		loc = m_adjacencies.find(ends[1]);
		//if there are already adjacencies for the given second vertex, 
		//	add an entry.
		if(loc != m_adjacencies.end())
		{
			//this assumes there aren't duplicate edges
			loc->second.push_back(ends[0]);
		}
		else
		{
			list<Vertex*> adjs;
			adjs.push_back(ends[0]);
			m_adjacencies.insert(make_pair(ends[1], adjs));
		}
		m_edges.push_back(Edge(ends[0], ends[1], cost));
	}
	Graph(list<Vertex> verts): m_vertices(verts)
	{
		m_adjacencies = map<Vertex*, list<Vertex*>>();
		for(list<Edge>::iterator edge = m_edges.begin(); edge != m_edges.end(); ++edge)
		{

		}
	}
	list<Edge> getEdges(){ return m_edges; }
	list<Vertex> *getVertices(){ return &m_vertices; }
	list<Vertex*> adjacentNodes(Vertex *theVertex){
		map<Vertex*, list<Vertex*>>::iterator itr = m_adjacencies.begin();
		while(itr != m_adjacencies.end())
		{
			if(*itr->first == *theVertex)
				return itr->second;
		}
		if(itr == m_adjacencies.end())
			return list<Vertex*>();
		else
			return m_adjacencies.find(theVertex)->second; 
	}

private:
	list<Vertex> m_vertices;
	list<Edge> m_edges;
	map<Vertex*, list<Vertex*>> m_adjacencies;
};

#endif