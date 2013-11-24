#ifndef NAVNODE_H
#define NAVNODE_H

#include <random>
#include <time.h>

#include "baseObject.h"
#include "primitives.h"
#include "physics.h"
#include "ground.h"
#include "collision.h"

class navNode : public baseObject
{
public:
	struct navInfo
	{
		ground *dest;
		//this vector stores information on how fast and in which direction
		//	an actor needs to be moving to reach dest
		//this represents the minimum required velocities to end up on dest
		physics::vector moveVector;
		double travelTime;
		//this bool is for use during generation of the navmesh (avoids infinite looping)
		bool visited;
		navInfo(ground * dest, physics::vector moveVector, double travelTime) :
			dest(dest), moveVector(moveVector), travelTime(travelTime), visited(false) {}
	};

	navNode(primitives::vertex origin, ground *below, const list<ground> &allGround,
		const float &maxSpeed, const float &maxJumpSpeed) :
		baseObject(origin, .025f, .025f), m_source(below)
	{
		m_dests = list<navInfo>();
		generateDests(allGround, maxSpeed, maxJumpSpeed);
	}
	list<navInfo> getDests(){ return m_dests; }

private:
	bool obstructed(const primitives::vertex &loc, const list<ground> &allGround, const ground &dest);
	void generateDests(const list<ground> &allGround, const float &maxSpeed,
		const float &maxJumpSpeed);

	//Since each node represents a link (path) between two objects, 
	//	source represent the object the node is on, and 
	ground *m_source;
	list<navInfo> m_dests;
};

#endif