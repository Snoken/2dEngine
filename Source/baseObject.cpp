#include "baseObject.h"

void baseObject::setMaxMin()
{
	list<primitives::vertex>::iterator itr = points.begin();
	xMax = xMin = itr->x;
	yMax = yMin = itr->y;
	for (; itr != points.end(); ++itr)
	{
		if (itr->x > xMax) xMax = itr->x;
		else if (itr->x < xMin) xMin = itr->x;

		if (itr->y > yMax) yMax = itr->y;
		else if (itr->y < yMin) yMin = itr->y;
	}
}

void baseObject::init()
{
	setMaxMin();
	width = xMax - xMin;
	height = yMax - yMin;
	texture = 0;
	bSelected = false;
	texRotation = 0;
	for (int i = 0; i < 4; ++i)
		color[i] = 1.0f;
}

ostream& operator<<(ostream& stream, const baseObject& obj)
{
	stream << "Origin: (" << obj.origin.x << ", " << obj.origin.y << ")" << endl;
	stream << "Width: " << obj.width << " Height: " << obj.height << endl;
	return stream;
}