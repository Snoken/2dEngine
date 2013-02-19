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

	baseObject( vertex origin, list<vertex> points ):
		origin(origin), points(points)
	{
		for( int i = 0; i < 4; ++i )
			color[i] = 1.0f;
	}
	baseObject( vertex origin, list<vertex> points, GLfloat color[4] )
	{
		*this = baseObject( origin, points );
		for( int i = 0; i < 4; ++i )
			this->color[i] = color[i];
	}

protected:
	vertex origin;
};
#endif