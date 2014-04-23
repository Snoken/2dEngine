#include "Graph.h"

void Graph::addEdge(Edge newEdge)
{
	m_pEdges->push_back(newEdge);
	Edge* edge = &*--m_pEdges->end();
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
			// remove condition to make edges undirected
			if ((*listItr)->ends[0] == theVertex)
				adjacent.push_back(*listItr);
		}
	}
	return adjacent;
}

Graph::Graph(list<Vertex>* verts, list<Edge>* edges, map<Vertex*, list<Edge*>> adjs) :
m_pVertices(verts), m_pEdges(edges), m_adjacencies(adjs)
{
	// don't need to compute adjs if given them
	if (m_adjacencies == map<Vertex*, list<Edge*>>())
		return;
	// Vertices and edges have been set, compute adjacencies
	m_adjacencies = map<Vertex*, list<Edge*>>();
	for (list<Edge>::iterator edge = m_pEdges->begin(); edge != m_pEdges->end(); ++edge)
	{
		// Using addEdge method ensures that all adjacencies get computed
		addEdge(*edge);
	}
}

Graph::Edge::Edge(Vertex* ends[2], double cost, double travelTime, navNode* startNode, physics::vector moveVector) : 
cost(cost), moveVector(moveVector), travelTime(travelTime)
{
	if (ends[0] == NULL)
		throw runtime_error("Vertex one is NULL");
	if (ends[1] == NULL)
		throw runtime_error("Vertex two is NULL");
	this->ends[0] = ends[0];
	this->ends[1] = ends[1];
	if (startNode == NULL)
		throw runtime_error("Start node is NULL");
	this->startNode = startNode;
}

void Graph::updateCosts(primitives::vertex startLoc, primitives::vertex endLoc)
{
	for (list<Graph::Edge>::iterator itr = m_pEdges->begin(); itr != m_pEdges->end(); ++itr)
	{
		//path cost
		// (horizontal distance to player + vertical distance to player ^ 2 + travel time +
		//	linear distance to dest platform ^ 2) * 10;
		double xDiff = abs(startLoc.x - itr->startNode->origin.x);
		double yDiff = abs(startLoc.y - itr->startNode->origin.y);
		itr->cost = (xDiff + pow(yDiff, 2.0) + itr->travelTime) * 10;

		xDiff = itr->startNode->origin.x - itr->ends[1]->plat->origin.x;
		yDiff = itr->startNode->origin.y - itr->ends[1]->plat->origin.y;
		itr->cost += (pow(xDiff, 2.0) + pow(yDiff, 2.0)) * 10;

		//cout << "Cost: " << itr->cost << endl;
	}

	for (list<Graph::Vertex>::iterator itr = m_pVertices->begin(); itr != m_pVertices->end(); ++itr)
	{
		// heuristic value
		// linear distance from given platform to destination * 100;
		double xDiff = itr->plat->origin.x - endLoc.x;
		double yDiff = itr->plat->origin.y - endLoc.y;
		itr->hVal = sqrt(pow(xDiff, 2.0) + pow(yDiff, 2.0)*100);
		//cout << "hVal: " << itr->hVal << endl;
	}
}

ostream& operator<<(ostream& stream, const Graph& obj)
{
	stream << "Vertices: " << obj.m_pVertices->size() << endl;
	if( obj.m_pVertices->size() < 2000 )
	{
		for (list<Graph::Vertex>::const_iterator itr = obj.m_pVertices->begin();
			itr != obj.m_pVertices->end(); ++itr)
		{
			stream << "\tLoc: (" << itr->plat->origin.x << ", " <<
				itr->plat->origin.y << ") hVal: " << itr->hVal << endl;
		}
	}
	
	stream << "Edges: " << obj.m_pEdges->size() << endl;
	if( obj.m_pEdges->size() < 2000 )
	{
		for (list<Graph::Edge>::const_iterator itr = obj.m_pEdges->begin();
			itr != obj.m_pEdges->end(); ++itr)
		{
			stream << "\tLoc 1: (" << (itr->ends[0])->plat->origin.x << ", " <<
				(itr->ends[0])->plat->origin.y << ") Loc 2: (" << (itr->ends[1])->plat->origin.x
				<< ", " << (itr->ends[1])->plat->origin.y << ") Cost: " << itr->cost << endl;
		}
	}
	
	return stream;
}
