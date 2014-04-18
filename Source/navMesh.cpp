#include "navMesh.h"
#include <algorithm>

void navMesh::generateAllNodes(const list<ground> &allGround, const float &maxRunSpeed, const float &maxJumpSpeed,
	const primitives::vertex& playerSize)
{
	cout << endl <<  "Generating Navigation Mesh... ";
	//iterate through all ground objects and make a navNode at left, center, and right
	for (list<ground>::const_iterator itr = allGround.begin(); itr != allGround.end(); ++itr)
	{
		cout << endl << "Calculating paths for platform at (" << itr->origin.x << ", "
			<< itr->origin.y << ")" << endl;
		//create a navNode for the left edge
		navNode node(primitives::vertex(itr->xMin, itr->yMax), (ground*)&(*itr), allGround, 
			maxRunSpeed, maxJumpSpeed, playerSize);
		//if there are any paths, add it to the list
		list<navNode> nodes;
		if (!node.getDests().empty())
			nodes.push_back(node);

		//create a navNode for the right edge
		node = navNode(primitives::vertex(itr->xMax, itr->yMax), (ground*) &(*itr), allGround, 
			maxRunSpeed, maxJumpSpeed, playerSize);
		//if there are any paths, add it to the list
		if (!node.getDests().empty())
			nodes.push_back(node);

		//create a navNode for the center
		node = navNode(primitives::vertex(itr->origin.x, itr->yMax), (ground*) &(*itr), allGround, 
			maxRunSpeed, maxJumpSpeed, playerSize);
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
			//yet another itr for processing the paths on each node
			for (list<navNode::navInfo>::iterator infoItr = dests.begin(); infoItr != dests.end(); ++infoItr)
			{
				if (abs(infoItr->moveVector.getVertComp()) > 0.05)
					continue;
				cout << endl << "Calculating reciprocal paths for platform at (" << infoItr->dest->origin.x << ", "
					<< infoItr->dest->origin.y << ")" << endl;
				if (infoItr->visited)
					continue;
				else
					infoItr->visited = true;

				//find ending x value
				float endX = innerItr->origin.x + (infoItr->moveVector.getHorizComp() * infoItr->travelTime);
				float endY = infoItr->dest->yMax;
				//create a navNode at end of path
				navNode newNode(primitives::vertex(endX, endY), infoItr->dest, allGround,
					maxRunSpeed, maxJumpSpeed, playerSize);
				//add the node to the relevant map entry if any paths exist, create entry if needed
				if (!newNode.getDests().empty())
				{
					map<ground*, list<navNode>>::iterator loc = m_mesh.find(infoItr->dest);
					if (loc != m_mesh.end())
					{
						loc->second.push_back(newNode);
					}
					else
					{
						list<navNode> nodes;
						nodes.push_back(newNode);
						m_mesh.insert(make_pair((ground*) &(*infoItr->dest), nodes));
					}
				}
			}
		}
	}
	colorNodes();
	cout << "Done" << endl;
}

double navMesh::calculateCost(navNode::navInfo info)
{
	return info.travelTime * 5.0;
}

void navMesh::buildGraph(const list<ground> &allGround)
{
	m_navGraph = new Graph;
	// add a vertex for each ground object
	for (list<ground>::const_iterator itr = allGround.begin(); itr != allGround.end(); ++itr)
	{
		// constant heuristic for now
		m_navGraph->addVertex(&*itr, 1);
	}
	// add an edge for each path from a navNode, cost = time to travel
	// iterator for ground objects
	list<Graph::Vertex>* verts = m_navGraph->getVertices();
	for (map <ground*, list<navNode>>::iterator itr = m_mesh.begin(); itr != m_mesh.end(); ++itr)
	{
		Graph::Vertex* ends[2];
		ends[0] = &*find(verts->begin(), verts->end(), Graph::Vertex(itr->first, 1));
		//inner iterator for processing the lists on each ground obj
		for (list<navNode>::iterator innerItr = itr->second.begin(); innerItr != itr->second.end(); ++innerItr)
		{
			list<navNode::navInfo> dests = innerItr->getDests();
			for (list<navNode::navInfo>::iterator destItr = dests.begin(); destItr != dests.end(); ++destItr)
			{
				ends[1] = &*find(verts->begin(), verts->end(), Graph::Vertex(destItr->dest, 1));
				if (*ends[0] == *ends[1])
					continue;
				// starting w/ 1, costs need to be updated when npc wants to pathfind
				Graph::Edge newEdge(ends, 1.0, &*innerItr, destItr->moveVector);
				m_navGraph->addEdge(newEdge);
			}
		}
	}
	cout << *m_navGraph << endl;
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
