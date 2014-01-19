#include "AStar.h"

Tree::Path AStar::search(Graph::Vertex *end)
{
	Tree::Path thePath = Tree::Path();
	Tree::Node *currNode = theTree->getHead();
	//Exit condition: We've found a path to the goal vertex (end)
	do
	{
		//tack currNode onto path
		thePath.addToPath(currNode);

		//remove current entry for shortest path and add new one
		list<Tree::Path>::iterator loc = find(m_shortests.begin(), m_shortests.end(), thePath);
		if( loc != m_shortests.end() )
			m_shortests.remove(*loc);
		m_shortests.push_back(Tree::Path(thePath));

		//for every vertex which the current touches
		list<Graph::Vertex*> currAdjs = toSearch->adjacentNodes(currNode->vert);
		for(list<Graph::Vertex*>::iterator adjItr = currAdjs.begin(); adjItr != currAdjs.end(); ++adjItr)
		{
			//if already explored
			if(find(closedList.begin(), closedList.end(), currNode) != closedList.end())
			{
				//find old value
				Tree::Path dummyPath;
				dummyPath.addToPath(currNode);
				list<Tree::Path>::iterator loc = find(m_shortests.begin(), 
					m_shortests.end(), dummyPath);
				//if new value is better
				if(thePath.cost() < loc->cost())
				{
					//move to proper location

					//Tree::Node 
				}
			}
			//nothing to check if it's not on tree
			else
			{
				thePath.addToPath(currNode);
				m_shortests.push_back(thePath);
				theTree->addAsChild(thePath.cost(), *adjItr, currNode);
			}
		}

		//add all children to openList
		for(list<Tree::Node*>::iterator child = currNode->children.begin(); 
			child != currNode->children.end(); ++child)
			openList.push_back(*child);

		//if the open list is empty, there's 
		if(openList.empty())
			return Tree::Path();
		//if(!openList.empty())
			//find lowest cost unexpanded node on open list

		//create a node for every vertex that currNode has an edge to and add to openList
		//sort open list by total cost
		//set currNode to lowest cost node on open list
		//while given vertex is already in closed list, check if this is a better path
			//if yes, update tree and back the path up accordingly
			//otherwise ignore it, remove from openlist and take next best
		//add currnode to path

		openList.sort();
		//move to next cheapest node, assumes open list is sorted
		currNode = *openList.begin();
	}
	while(currNode->vert != end);
	
	return Tree::Path();
}