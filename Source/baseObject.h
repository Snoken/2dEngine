#ifndef BASEOBJECT_H
#define BASEOBJECT_H
#include <list>
#include <gl/glut.h>

using namespace std;
class baseObject
{
public:
	struct vertex{
		GLfloat x, y;
		vertex( GLfloat x, GLfloat y):
		x(x), y(y){}
			vertex():x(0.0f), y(0.0f){}
	};
	list<vertex> points;
	GLfloat color[4];
	GLfloat xMax, yMax, xMin, yMin;

	void updateMaxMin()
	{
		list<vertex>::iterator itr = points.begin();
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

	baseObject( vertex origin, list<vertex> points ):
		origin(origin), points(points)
	{
		updateMaxMin();
		for( int i = 0; i < 4; ++i )
			color[i] = 1.0f;
	}
	baseObject( vertex origin, list<vertex> points, GLfloat color[4] )
	{
		*this = baseObject( origin, points );
		for( int i = 0; i < 4; ++i )
			this->color[i] = color[i];
	}

	baseObject(const baseObject& old)
	{
		*this = old;
	}
	vertex origin;
};
#endif