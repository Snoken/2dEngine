#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include <iostream>
#if _WIN32
	#include "GL/freeglut.h"
#else
	#include "GL/glew.h"
#endif
#include <cmath>

namespace primitives {
	class vertex{
	public:
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
		void roundToNearest(float interval)
		{
			x = floor(x * 100.0f) / 100.0f;
			float proximity = fmod(x, interval);
			if (abs(proximity) < .025f)
				x -= proximity;
			else
				x >= 0 ? x += interval - proximity : x += -interval - proximity;

			y = floor(y * 100.0f) / 100.0f;
			proximity = fmod(y, interval);
			if (abs(proximity) < .025f)
				y -= proximity;
			else
				y >= 0 ? y += interval - proximity : y += -interval - proximity;
		}
	};
}
#endif
