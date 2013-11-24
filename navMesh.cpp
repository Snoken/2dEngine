#include "navMesh.h"

void navMesh::generateAllNodes(const list<ground> &allGround, const float &maxRunSpeed, const float &maxJumpSpeed)
{
	//iterate through all ground objects and make a navNode at left, center, and right
	for (list<ground>::const_iterator itr = allGround.begin(); itr != allGround.end(); ++itr)
	{
		//create a navNode for the left edge
		navNode node(primitives::vertex(itr->xMin, itr->yMax), (ground*)&(*itr), allGround, maxRunSpeed, maxJumpSpeed);
		//if there are any paths, add it to the list
		list<navNode> nodes;
		if (!node.getDests().empty())
		nodes.push_back(node);

		//create a navNode for the right edge
		node = navNode(primitives::vertex(itr->xMax, itr->yMax), (ground*) &(*itr), allGround, maxRunSpeed, maxJumpSpeed);
		//if there are any paths, add it to the list
		if (!node.getDests().empty())
			nodes.push_back(node);

		//create a navNode for the center
		node = navNode(primitives::vertex(itr->origin.x, itr->yMax), (ground*) &(*itr), allGround, maxRunSpeed, maxJumpSpeed);
		//if there are any paths, add it to the list
		if (!node.getDests().empty())
			nodes.push_back(node);

		//if the list has any nodes in it, create an entry in the map
		if (!nodes.empty())
			m_mesh.insert(make_pair((ground*)&(*itr), nodes));
	}

	//iterate through m_mesh to find nodes w/ 0 vertical velocity (this signifies a path dropping off an object)
	//	and add a node at the end of the drop off path (to make it navigable both ways)
	for (map <ground*, list<navNode>>::iterator itr = m_mesh.begin(); itr != m_mesh.end(); ++itr)
	{
		//inner iterator for processing the lists on each ground obj
		for (list<navNode>::iterator innerItr = itr->second.begin(); innerItr != itr->second.end(); ++innerItr)
		{
			list<navNode::navInfo> dests = innerItr->getDests();
			//yet another itr for processing the paths on each node (boy is this efficient)
			for (list<navNode::navInfo>::iterator infoItr = dests.begin(); infoItr != dests.end(); ++infoItr)
			{
				//would say != 0.0 if using double, but this leeway is needed for floats
				if (infoItr->moveVector.getVertComp() > 0.0f)
					continue;
				//find ending x value
				float endX = innerItr->origin.x + (infoItr->moveVector.getHorizComp() * infoItr->travelTime);
				//create a navNode at end of path
				cout << *infoItr->dest << endl;
				navNode newNode(primitives::vertex(endX, infoItr->dest->yMax), infoItr->dest, allGround,
					maxRunSpeed, maxJumpSpeed);
				//add the node to the relevant map entry if any paths exist, create entry if needed
				if (!newNode.getDests().empty())
				{
					cout << endX << endl;
					map<ground*, list<navNode>>::iterator loc = m_mesh.find(infoItr->dest);
					if (loc != m_mesh.end())
					{
						loc->second.push_back(newNode);
					}
					else
					{
						list<navNode> nodes;
						nodes.push_back(newNode);
						m_mesh.insert(make_pair((ground*) &(*itr), nodes));
					}
				}
			}
		}
	}
	colorNodes();
}

void navMesh::colorNodes()
{
	for (map <ground*, list<navNode>>::iterator mapItr = m_mesh.begin(); mapItr != m_mesh.end(); ++mapItr)
	{
		GLfloat colors[4];
		for (int i = 0; i < 4; ++i)
			colors[i] = fmod((float) rand(), 255.0f) / 255.0f;
		for (list<navNode>::iterator listItr = mapItr->second.begin(); listItr != mapItr->second.end(); ++listItr)
		{
			for (int i = 0; i < 4; ++i)
				listItr->color[i] = colors[i];
		}
	}
	
}