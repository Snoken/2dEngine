#include "Graph.h"

void Graph::addEdge(Edge* edge)
{
	// Check for entry in map for first end
	map<Vertex*, list<Edge*>>::iterator loc = m_adjacencies.find(edge->ends[0]);
	// If exists, add new adjacency to list
	if (loc != m_adjacencies.end())
	{
		loc->second.push_back(edge);
	}
	// Otherwise create map entry and add edge
	else
	{
		pair<map<Vertex*, list<Edge*>>::iterator, bool> result
			= m_adjacencies.insert(make_pair(edge->ends[0], list<Edge*>()));
		result.first->second.push_back(edge);
	}

	// Check for entry in map for second end
	loc = m_adjacencies.find(edge->ends[1]);
	// If exists, add new adjacency to list
	if (loc != m_adjacencies.end())
	{
		loc->second.push_back(edge);
	}
	// Otherwise create map entry and add edge
	else
	{
		pair<map<Vertex*, list<Edge*>>::iterator, bool> result
			= m_adjacencies.insert(make_pair(edge->ends[1], list<Edge*>()));
		result.first->second.push_back(edge);
	}
}

list<Graph::Edge*> Graph::getConnectedEdges(Vertex *theVertex)
{
	list<Edge*> adjacent;
	// Find map entry for 
	map<Vertex*, list<Edge*>>::iterator itr = m_adjacencies.find(theVertex);
	if (itr != m_adjacencies.end())
	{
		// Iterate adjacencies to create list of vertices
		for (list<Edge*>::iterator listItr = itr->second.begin();
			listItr != itr->second.end(); ++listItr)
		{
			if ((*listItr)->ends[0] == theVertex)
				adjacent.push_back(*listItr);
			else
				adjacent.push_back(*listItr);
		}
	}
	return adjacent;
}

Graph::Graph(list<Vertex>* verts, list<Edge>* edges) : m_pVertices(verts), m_pEdges(edges)
{
	// Vertices and edges have been set, compute adjacencies
	m_adjacencies = map<Vertex*, list<Edge*>>();
	for (list<Edge>::iterator edge = m_pEdges->begin(); edge != m_pEdges->end(); ++edge)
	{
		// Using addEdge method ensures that all adjacencies get computed
		addEdge(&(*edge));
	}
}

Graph::Edge::Edge(Vertex* ends[2], int cost) : cost(cost)
{
	if (ends[0] == NULL)
		throw exception("Vertex one is NULL");
	if (ends[1] == NULL)
		throw exception("Vertex two is NULL");
	this->ends[0] = ends[0];
	this->ends[1] = ends[1];
}

ostream& operator<<(ostream& stream, const Graph& obj)
{
	for (map<Graph::Vertex*, list<Graph::Edge*>>::const_iterator itr = obj.m_adjacencies.begin();
		itr != obj.m_adjacencies.begin(); ++itr)
	{
	}
	return stream;
}
