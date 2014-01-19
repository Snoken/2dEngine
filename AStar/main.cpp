#include "AStar.h"
list<Graph::Vertex> verts;

Graph createLineGraph()
{
	verts.push_back(Graph::Vertex(1, 5));
	verts.push_back(Graph::Vertex(2, 5));
	verts.push_back(Graph::Vertex(3, 5));
	verts.push_back(Graph::Vertex(4, 5));
	verts.push_back(Graph::Vertex(5, 5));

	Graph theGraph(verts);
	list<Graph::Vertex>::iterator itr = verts.begin();
	do
	{
		Graph::Vertex *prev = &(*itr++);
		Graph::Vertex* ends[2] = { prev, &(*itr) };
		theGraph.addEdge(ends, 5);
	}
	while(itr != --verts.end());
	return theGraph;
}

int main()
{
	Graph theGraph = createLineGraph();
	Graph::Vertex *headNode = &(*theGraph.getVertices()->begin());
	AStar searcher(headNode, &theGraph);
	//searcher.theTree->addAsChild(0, &*theGraph.getVertices().begin(), searcher.theTree->getHead());
	searcher.search(&*--theGraph.getVertices()->end());
	return 0;
}