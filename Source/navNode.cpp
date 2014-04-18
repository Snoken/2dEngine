#include "navNode.h"

bool navNode::obstructed(const primitives::vertex &loc, const primitives::vertex &top, 
	const list<ground> &allGround, const ground &dest)
{
	//go through all ground objects, see if collision
	for (list<ground>::const_iterator gItr = allGround.begin();
		gItr != allGround.end(); ++gItr)
	{
		if (collision::inObject(loc, *gItr) || collision::inObject(top, *gItr))
			return true;
	}
	return false;
}

void navNode::generateDests(const list<ground> &allGround, const float &maxSpeed,
	const float &maxJumpSpeed, const primitives::vertex& playerSize )
{
	//find the highest a jump can go
	physics::vector jumpVec = physics::vector();
	jumpVec.setVerticalComp(maxJumpSpeed);
	primitives::vertex vert(origin.x, origin.y);
	float maxHeight = (float) physics::apex(jumpVec, vert);
	for (list<ground>::const_iterator itr = allGround.begin(); itr != allGround.end(); ++itr)
	{
		jumpVec = physics::vector(maxJumpSpeed, 90.0f);
		//no need for navigation to same object
		if (*itr == *m_source)
			continue;
		//if top of object is higher than max height, not reachable
		//	subtracting .01 to account for requirements of horizontal movement
		if (maxHeight - .01 < itr->yMax)
			continue;
		//find horizontal distance
		float xDif, landTime;
		primitives::vertex landLoc;
		baseObject checkObj = (baseObject)*itr;
		if (collision::leftOf(origin, checkObj))
		{
			xDif = abs(itr->xMin - origin.x);
			landLoc = primitives::vertex(itr->xMin, itr->yMax);
			if (origin.y < landLoc.y)
			{
				landLoc.x -= playerSize.x / 2;
			}
		}
		else
		{
			xDif = abs(origin.x - itr->xMax);
			landLoc = primitives::vertex(itr->xMax, itr->yMax);
			if (origin.y < landLoc.y)
			{
				landLoc.x += playerSize.x / 2;
			}
		}
		if (xDif > 1.5f && itr->yMax >= origin.y)
			continue;

		float yDif = abs(itr->yMax - m_source->yMax);
		if (origin.y < landLoc.y)
		{
			landLoc.y += yDif * 0.25f;
			xDif -= playerSize.x / 2;
		}
		float reqVertSpeed = physics::requiredVertSpeed(origin.y, landLoc.y);
		jumpVec.setVerticalComp(reqVertSpeed);
		landTime = physics::timeToLand(jumpVec, origin, landLoc);
		if (reqVertSpeed > 0)
		{
			//if the x travel takes longer, adjust jump strength, if y takes longer adjust speed
			float minHorizTime = xDif / maxSpeed;
			if (minHorizTime > landTime)
			{
				float reqVertSpeed = physics::reqSpeedWithTime(origin.y, landLoc.y, minHorizTime);
				if (reqVertSpeed > maxJumpSpeed)
					continue;
				
				origin.x < landLoc.x ? jumpVec.setHorizontalComp(maxSpeed) : 
					jumpVec.setHorizontalComp(-maxSpeed);
				jumpVec.setVerticalComp(reqVertSpeed);
				landTime = minHorizTime;
			}
			else
			{
				origin.x < landLoc.x ? jumpVec.setHorizontalComp(xDif / landTime) :
					jumpVec.setHorizontalComp(-xDif / landTime);
			}
		}
		//if the destination is below the start, need to find time with falling rather than
		//	horizontal tracel
		else if (reqVertSpeed == 0)
		{
			if (origin.x == m_source->xMin)
				jumpVec.setHorizontalComp(-maxSpeed);
			else if (origin.x == m_source->xMax)
				jumpVec.setHorizontalComp(maxSpeed);
			//can't drop off object if not at edge
			else
				continue;
			jumpVec.setVerticalComp(0);
		}


		//check for obstruction of path
		primitives::vertex loc = origin;
		physics::vector moveVec = jumpVec;
		//this int decides how many points along the path to check
		int segments = 40;
		for (int i = 0; i <= segments + 20; ++i)
		{
			if (i >= segments && collision::inObject(loc, *itr))
			{
				m_dests.push_back(navInfo((ground*) &(*itr), jumpVec, landTime));
				cout << "\tAdded a path to platform at (" << itr->origin.x << ", "
					<< itr->origin.y << ")" << endl;
			}
			primitives::vertex top(loc.x, loc.y + playerSize.y);
			if (obstructed(loc, top, allGround, *itr))
				break;
			float horizSpeed = (float)moveVec.getHorizComp();
			loc.x += landTime / (float) segments * horizSpeed;
			float vi = moveVec.getVertComp();
			moveVec.applyGravity(landTime / (float) segments);
			loc.y += (vi + moveVec.getVertComp()) / 2 * (landTime / (float) segments);
		}
	}
}
