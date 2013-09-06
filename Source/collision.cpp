#include "collision.h"

bool collision::intersecting( baseObject::vertex ends1[2], baseObject::vertex ends2[2], float e )
{
	float a1 = ends1[1].y - ends1[0].y;
	float b1 = ends1[0].x - ends1[1].x;
	//the Cs aren't needed since we're not actually solving for where they intersect
	//float c1 = b1*ends1[0].y + a1*ends1[0].x;

	float a2 = ends2[1].y - ends2[0].y;
	float b2 = ends2[0].x - ends2[1].x;
	//float c2 = b2*ends2[0].y + a2*ends2[0].x;

	float det = a1*b2 - a2*b1;

	//parallel, return false
	if( det < 0 + e && det > 0 - e )
		return false;

	//otherwise return true
	return true;
}

bool collision::inObject( baseObject::vertex point, baseObject obj )
{
	if (point.x < obj.xMin || point.x > obj.xMax || point.y < obj.yMin || point.y > obj.yMax)
		// Definitely not within the polygon
		return false;

	//ray casting impl

	//epsilon to account for float precision
	float e = ((obj.xMax - obj.xMin) / 100);

	//points of ray to test
	baseObject::vertex rayPoints[2] = {baseObject::vertex(obj.xMin-e, point.y), 
		baseObject::vertex( point.x, point.y )};

	//start on second vertex so we're sure there's at least two points
	for( list<baseObject::vertex>::iterator p2 = ++(obj.points.begin()); p2 != obj.points.end(); ++p2 )
	{
		list<baseObject::vertex>::iterator p1 = p2;
		--p1;
		baseObject::vertex linePoints[2] = { baseObject::vertex(p1->x, p1->y), 
			baseObject::vertex(p2->x, p2->y) };
		if( intersecting(rayPoints,linePoints, e) )
			return true;
	}

	return false;
}

bool collision::areColliding( baseObject one, baseObject two )
{
	for( list<baseObject::vertex>::iterator itrOne = one.points.begin(); itrOne != one.points.end(); ++itrOne )
	{
		if( inObject( *itrOne, two ) )
			return true;
	}
	return false;
}

bool collision::above( baseObject one, baseObject two )
{
	//make sure one is withing x dimensions of two
	if( !(one.xMax > two.xMin && one.xMin < two.xMax) )
		return false;
	if( (one.yMin +.01) >= two.yMax )
		return true;
	return false;
}

bool collision::below( baseObject one, baseObject two )
{
	//make sure one is withing x dimensions of two
	if( !(one.xMax > two.xMin && one.xMin < two.xMax) )
		return false;
	return one.yMax < two.yMin;
}

bool collision::nextTo( baseObject one, baseObject two )
{
	//return true if any point in one is between two max and min height 
	if( (one.yMin <= two.yMax && one.yMin >= two.yMin) || 
		(one.yMax <= two.yMax && one.yMax >= two.yMin) ||
		(two.yMax <= one.yMax && two.yMax >= one.yMin) ||
		(two.yMin <= one.yMax && two.yMin >= one.yMin) )
		return true;
	return false;
}