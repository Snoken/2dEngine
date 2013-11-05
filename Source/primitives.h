#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include <iostream>
#include "GL/freeglut.h"

namespace primitives {
	struct vertex{
		GLfloat x, y;
		vertex(GLfloat x, GLfloat y) :
			x(x), y(y){}
		vertex() : x(0.0f), y(0.0f){}
		bool operator==(const vertex& rhs)
		{
			return x == rhs.x && y == rhs.y;
		}
		bool operator!=(const vertex& rhs)
		{
			return !(*this==rhs);
		}
	};
}
#endif
