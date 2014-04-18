#ifndef NAVMESH_H
#define NAVMESH_H

#include <map>
#include <random>

#include "navNode.h"
#include "Graph.h"

class navMesh
{
public:
	navMesh(const list<ground> &allGround, const float &maxRunSpeed, const float &maxJumpSpeed,
		const primitives::vertex& playerSize)
	{
		// Seed RNG for node coloring
		srand((unsigned)time(NULL));
		generateAllNodes(allGround, maxRunSpeed, maxJumpSpeed, playerSize);
		buildGraph(allGround);
	}
	//fetch all navNodes associated with the specified platform
	list<navNode> getNodesForPlatform(ground *lookUp) 
	{
		map<ground*, list<navNode>>::iterator loc = m_mesh.find(lookUp);
		if (loc == m_mesh.end())
			return list<navNode>();
		return loc->second;
	}
	void removeEntry(ground *remove){ m_mesh.erase(remove); }
	Graph* getNavGraph(){ return m_navGraph; }

private:
	void generateAllNodes(const list<ground> &allGround, const float &maxRunSpeed, const float &maxJumpSpeed,
		const primitives::vertex& playerSize);
	double calculateCost(navNode::navInfo info);
	void buildGraph(const list<ground> &allGround);
	void colorNodes();
	//this class stores it's data by associating a list of navNodes with each platform
	map<ground*, list<navNode>> m_mesh;
	Graph* m_navGraph;
};

#endif
