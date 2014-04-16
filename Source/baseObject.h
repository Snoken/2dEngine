#ifndef BASEOBJECT_H
#define BASEOBJECT_H
#include <list>
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

	void setMaxMin();
	void init();
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
		bool returnMe = origin.x == other.origin.x;
		returnMe = returnMe && (origin.y == other.origin.y);
		returnMe = returnMe && (width == other.width);
		returnMe = returnMe && (height == other.height);
		return returnMe;
	}

	primitives::vertex origin;
	friend ostream& operator<<(ostream& stream, const baseObject& obj);
};

#endif
