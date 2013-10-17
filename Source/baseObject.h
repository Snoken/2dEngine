#ifndef BASEOBJECT_H
#define BASEOBJECT_H
#include <list>
#include <GL/freeglut.h>
#include "primitives.h"

using namespace std;
class baseObject
{
public:
	list<primitives::vertex> points;
	GLfloat color[4];
	GLfloat xMax, yMax, xMin, yMin, width, height;
	GLuint texture;
	float texRotation;
	bool bSelected;

	void setMaxMin()
	{
		list<primitives::vertex>::iterator itr = points.begin();
		xMax = xMin = itr->x;
		yMax = yMin = itr->y;
		for( ; itr != points.end(); ++itr )
		{
			if( itr->x > xMax ) xMax = itr->x;
			else if( itr->x < xMin ) xMin = itr->x;

			if( itr->y > yMax ) yMax = itr->y;
			else if( itr->y < yMin ) yMin = itr->y;
		}
	}

	void init()
	{
		setMaxMin();
		width = xMax - xMin;
		height = yMax - yMin;
		texture = 0;
		bSelected = false;
		texRotation = 0;
		for( int i = 0; i < 4; ++i )
			color[i] = 1.0f;
	}
	baseObject( primitives::vertex origin, list<primitives::vertex> points ):
		origin(origin), points(points)
	{
		init();
	}
	baseObject( primitives::vertex origin, list<primitives::vertex> points, GLfloat color[4] ): origin(origin)
	{
		*this = baseObject( origin, points );
		for( int i = 0; i < 4; ++i )
			this->color[i] = color[i];
	}
	baseObject( primitives::vertex origin, float width, float height, GLfloat color[4] ): origin(origin)
	{
		points.push_back(primitives::vertex(origin.x - width/2, origin.y - height/2));
		points.push_back(primitives::vertex(origin.x - width/2, origin.y + height/2));
		points.push_back(primitives::vertex(origin.x + width/2, origin.y + height/2));
		points.push_back(primitives::vertex(origin.x + width/2, origin.y - height/2));
		init();
		for( int i = 0; i < 4; ++i )
			this->color[i] = color[i];
	}
	baseObject( primitives::vertex origin, list<primitives::vertex> points, GLuint texture ): origin(origin)
	{
		*this = baseObject( origin, points );
		this->texture = texture;
	}
	baseObject( primitives::vertex origin, float width, float height, GLuint texture = 0 ): origin(origin)
	{
		points.push_back(primitives::vertex(origin.x - width/2, origin.y - height/2));
		points.push_back(primitives::vertex(origin.x - width/2, origin.y + height/2));
		points.push_back(primitives::vertex(origin.x + width/2, origin.y + height/2));
		points.push_back(primitives::vertex(origin.x + width/2, origin.y - height/2));
		init();
		this->texture = texture;
	}
	bool operator==(const baseObject &other) const{
		bool returnMe = true;
		origin.x == other.origin.x ? returnMe = true: returnMe = false;
		origin.y == other.origin.y ? returnMe = true: returnMe = false;
		width == other.width ? returnMe = true: returnMe = false;
		height == other.height ? returnMe = true: returnMe = false;
		return returnMe;
	}

	primitives::vertex origin;
};
#endif
