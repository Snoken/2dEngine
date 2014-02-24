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
		generateAllm_pNodes(allGround, maxRunSpeed, maxJumpSpeed);
	}
	//fetch all navm_pNodes associated with the specified platform
	list<navNode> getm_pNodesForPlatform(ground *lookUp) 
	{
		map<ground*, list<navNode>>::iterator loc = m_mesh.find(lookUp);
		if (loc == m_mesh.end())
			return list<navNode>();
		return loc->second;
	}
	void removeEntry(ground *remove){ m_mesh.erase(remove); }
private:
	void generateAllm_pNodes(const list<ground> &allGround, const float &maxRunSpeed, const float &maxJumpSpeed);
	void colorm_pNodes();
	//this class stores it's data by associating a list of navm_pNodes with each platform
	map<ground*, list<navNode>> m_mesh;
};

#endif