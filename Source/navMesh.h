#ifndef NAVMESH_H
#define NAVMESH_H

#include <map>
#include <random>

#include "navNode.h"

class navMesh
{
public:
	navMesh(const list<ground> &allGround, const float &maxRunSpeed, const float &maxJumpSpeed)
	{
		srand((unsigned)time(NULL));
		generateAllNodes(allGround, maxRunSpeed, maxJumpSpeed);
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
private:
	void generateAllNodes(const list<ground> &allGround, const float &maxRunSpeed, const float &maxJumpSpeed);
	void colorNodes();
	//this class stores it's data by associating a list of navNodes with each platform
	map<ground*, list<navNode>> m_mesh;
};

#endif