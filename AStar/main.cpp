#include "DFS.h"
#include "BFS.h"
#include "GreedyBestFirst.h"
#include "AStar.h"
#include <sstream>
#include <fstream>
list<Graph::Vertex> verts;
list<Graph::Edge> edges;

Graph createLineGraph()
{
	verts.push_back(Graph::Vertex(1, 5));
	verts.push_back(Graph::Vertex(2, 5));
	verts.push_back(Graph::Vertex(3, 5));
	verts.push_back(Graph::Vertex(4, 5));
	verts.push_back(Graph::Vertex(5, 5));

	list<Graph::Vertex>::iterator itr = verts.begin();
	do
	{
		Graph::Vertex *prev = &(*itr++);
		Graph::Vertex* ends[2] = { prev, &(*itr) };
		edges.push_back(Graph::Edge(ends, 1));
	}
	while(itr != --verts.end());

	Graph theGraph(&verts, &edges);
	return theGraph;
}

// three vertices with edges 1-2, 1-3
Graph createTriangleGraph()
{
	verts.push_back(Graph::Vertex(1, 5));
	verts.push_back(Graph::Vertex(2, 5));
	verts.push_back(Graph::Vertex(3, 5));

	list<Graph::Vertex>::iterator itr = ++verts.begin();
	do
	{
		Graph::Vertex* ends[2] = { &(*verts.begin()), &(*itr++) };
		edges.push_back(Graph::Edge(ends, 1));
	} while (itr != verts.end());

	Graph theGraph(&verts, &edges);
	return theGraph;
}

// used to process input
// NOTE: Absolutely no format checking, this is purely for testing
Graph createGeneralGraph(int argc, const char* argv[])
{
	verts.clear();
	edges.clear();
	int lineNum = 1;
	ifstream inFile(argv[1]);
	while (inFile.good())
	{
		char line[20];
		inFile.getline(line, 20);
		stringstream lineStrm(string(line + sizeof(char)));
		if (line[0] == 'v')
		{
			int id, hVal;

			lineStrm >> id;
			lineStrm >> hVal;

			verts.push_back(Graph::Vertex(id, hVal));
		}
		else if (line[0] == 'e')
		{
			int id1, id2, cost;

			lineStrm >> id1;
			lineStrm >> id2;
			lineStrm >> cost;

			list<Graph::Vertex>::iterator itr1 = verts.begin();
			advance(itr1, id1 - 1);
			list<Graph::Vertex>::iterator itr2 = verts.begin();
			advance(itr2, id2 - 1);

			Graph::Vertex* ends[2] = { &(*itr1), &(*itr2) };
			edges.push_back(Graph::Edge(ends, cost));
		}
		else
			cout << "Invalid format at line " << lineNum << endl;
		++lineNum;
	}

	Graph theGraph(&verts, &edges);
	return theGraph;
}

Tree::Node testNode(Graph* theGraph)
{
	Graph::Vertex theVert = *theGraph->getVertices()->begin();
	Tree::Node theNode(&theVert, 0);
	return theNode;
}

bool TestDFS(Graph* theGraph)
{
	Graph::Vertex* start = &(*theGraph->getVertices()->begin());
	Graph::Vertex* end = &(*(--theGraph->getVertices()->end()));
	DFS searcher(theGraph, start, end);
	bool success = searcher.DoSearch();
	if (success)
		searcher.displayPath();
	return success;
}

bool TestBFS(Graph* theGraph)
{
	Graph::Vertex* start = &(*theGraph->getVertices()->begin());
	Graph::Vertex* end = &(*(--theGraph->getVertices()->end()));
	BFS searcher(theGraph, start, end);
	bool success = searcher.DoSearch();
	if (success)
		searcher.displayPath();
	return success;
}

bool TestGBFS(Graph* theGraph)
{
	Graph::Vertex* start = &(*theGraph->getVertices()->begin());
	Graph::Vertex* end = &(*(--theGraph->getVertices()->end()));
	GreedyBestFirst searcher(theGraph, start, end);
	bool success = searcher.DoSearch();
	if (success)
		searcher.displayPath();
	return success;
}

bool TestAStar(Graph* theGraph)
{
	Graph::Vertex* start = &(*theGraph->getVertices()->begin());
	Graph::Vertex* end = &(*(--theGraph->getVertices()->end()));
	AStar searcher(theGraph, start, end);
	bool success = searcher.DoSearch();
	if (success)
		searcher.displayPath();
	return success;
}

// This function manually builds a tree to represent the search
//	space of the graph returned by CreateLineGraph
Tree createSimpleTree(Graph* theGraph)
{
	list<Graph::Vertex>* vertices = theGraph->getVertices();
	list<Graph::Vertex>::iterator itr = vertices->begin();
	Graph::Vertex theVert = *itr++;
	Tree theTree(&theVert);
	theVert = *itr++;
	theTree.addAsChild(0, &theVert, theTree.getHead());
	return theTree;
}

// reset visited to false for all vertices
void resetVerts()
{
	for (list<Graph::Vertex>::iterator itr = verts.begin(); itr != verts.end(); ++itr)
		itr->visited = false;
}


int main(int argc, const char* argv[])
{
	Graph theGraph = createGeneralGraph(argc, argv);
	Tree::Node theNode = testNode(&theGraph);
	Tree theTree = createSimpleTree(&theGraph);
	bool foundPath = TestDFS(&theGraph);
	resetVerts();
	foundPath = TestBFS(&theGraph);
	resetVerts();
	foundPath = TestGBFS(&theGraph);
	cout << endl;
	resetVerts();
	foundPath = TestAStar(&theGraph);

	return 0;
}