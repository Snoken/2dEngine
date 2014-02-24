#include "navMesh.h"

void navMesh::generateAllm_pNodes(const list<ground> &allGround, const float &maxRunSpeed, const float &maxJumpSpeed)
{
	//iterate through all ground objects and make a navNode at left, center, and right
	for (list<ground>::const_iterator itr = allGround.begin(); itr != allGround.end(); ++itr)
	{
		//create a navNode for the left edge
		navNode node(primitives::vertex(itr->xMin, itr->yMax), (ground*)&(*itr), allGround, maxRunSpeed, maxJumpSpeed);
		//if there are any paths, add it to the list
		list<navNode> m_pNodes;
		if (!node.getDests().empty())
		m_pNodes.push_back(node);

		//create a navNode for the right edge
		node = navNode(primitives::vertex(itr->xMax, itr->yMax), (ground*) &(*itr), allGround, maxRunSpeed, maxJumpSpeed);
		//if there are any paths, add it to the list
		if (!node.getDests().empty())
			m_pNodes.push_back(node);

		//create a navNode for the center
		node = navNode(primitives::vertex(itr->origin.x, itr->yMax), (ground*) &(*itr), allGround, maxRunSpeed, maxJumpSpeed);
		//if there are any paths, add it to the list
		if (!node.getDests().empty())
			m_pNodes.push_back(node);

		//if the list has any m_pNodes in it, create an entry in the map
		if (!m_pNodes.empty())
			m_mesh.insert(make_pair((ground*)&(*itr), m_pNodes));
	}

	//iterate through m_mesh to find m_pNodes w/ 0 vertical velocity (this signifies a path dropping off an object)
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
				if (infoItr->visited)
					continue;
				else
					infoItr->visited = true;

				if (infoItr->dest->width == 200.0f)
					cout << "hi\n";
				//find ending x value
				float endX = innerItr->origin.x + (infoItr->moveVector.getHorizComp() * infoItr->travelTime);
				float endY = infoItr->dest->yMax;
				//create a navNode at end of path
				//cout << *infoItr->dest << endl;
				navNode newNode(primitives::vertex(endX, endY), infoItr->dest, allGround,
					maxRunSpeed, maxJumpSpeed);
				//add the node to the relevant map entry if any paths exist, create entry if needed
				if (!newNode.getDests().empty())
				{
					//cout << endX << endl;
					map<ground*, list<navNode>>::iterator loc = m_mesh.find(infoItr->dest);
					if (loc != m_mesh.end())
					{
						loc->second.push_back(newNode);
					}
					else
					{
						list<navNode> m_pNodes;
						m_pNodes.push_back(newNode);
						m_mesh.insert(make_pair((ground*) &(*infoItr->dest), m_pNodes));
					}
				}
			}
		}
	}
	colorm_pNodes();
}

void navMesh::colorm_pNodes()
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