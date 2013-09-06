#ifndef BASEOBJECT_H
#define BASEOBJECT_H
#include <list>
#include <gl/freeglut.h>

using namespace std;

//This class is the base from which all other game objects inherit
class baseObject
{
protected:
	//iterate through points, set max/min values
	void setMaxMin()
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

	//always call this first, or some value will be overwritten with 0s
	void init()
	{
		setMaxMin();
		width = xMax - xMin;
		height = yMax - yMin;
		texture = 0;
		bSelected = false;
		texRotation = 0;
		//set to opaque white
		for( int i = 0; i < 4; ++i )
			color[i] = 1.0f;
	}

public:
	//
	struct vertex{
		float x, y;
		vertex( float x, float y):
			x(x), y(y){}
		//default to origin (center of window)
		vertex():x(0), y(0){}
	};
	vertex origin;
	//This list is used for keeping track of objects, as well as drawing them
	list<vertex> points;
	//draw color of object
	//TODO: check if this is used after textures added, if not add feature to 
		//fall back to color if no texture specified
	float color[4];
	//These vars are kept track of mostly for speeding up collision calculations
	float xMax, yMax, xMin, yMin, width, height;
	//specify the texture id (generated at run-time)
	GLuint texture;
	//rotation of texture in degrees
	float texRotation;
	//is this obj selected?
	bool bSelected;

	//NOTE: This class contains no empty constructor since that isn't really a logical feature
		//for a class creating shapes in the world.

	//----------------- CONSTRUCTORS -----------------
	//simplest version, just the origin and a set of points
	baseObject( vertex origin, list<vertex> points ):
		origin(origin), points(points)
	{
		init();
	}

	//allows color specification, see use examples
	baseObject( vertex origin, list<vertex> points, float color[4] ): origin(origin)
	{
		*this = baseObject( origin, points );
		for( int i = 0; i < 4; ++i )
			this->color[i] = color[i];
	}

	//create a quadrilateral based on height, width, origin, color
	baseObject( vertex origin, float width, float height, float color[4] ): origin(origin)
	{
		points.push_back(vertex(origin.x - width/2, origin.y - height/2));
		points.push_back(vertex(origin.x - width/2, origin.y + height/2));
		points.push_back(vertex(origin.x + width/2, origin.y + height/2));
		points.push_back(vertex(origin.x + width/2, origin.y - height/2));
		init();
		for( int i = 0; i < 4; ++i )
			this->color[i] = color[i];
	}

	//create a shape w/ a specific texture
	baseObject( vertex origin, list<vertex> points, GLuint texture ): origin(origin)
	{
		*this = baseObject( origin, points );
		this->texture = texture;
	}

	//create a quadrilateral with a specific texture
	baseObject( vertex origin, float width, float height, GLuint texture = 0 ): origin(origin)
	{
		points.push_back(vertex(origin.x - width/2, origin.y - height/2));
		points.push_back(vertex(origin.x - width/2, origin.y + height/2));
		points.push_back(vertex(origin.x + width/2, origin.y + height/2));
		points.push_back(vertex(origin.x + width/2, origin.y - height/2));
		init();
		this->texture = texture;
	}

	//for use by comparison functions, currently only checks width, height, and origin
	//TODO: update to check point by point when impl irregular geometry
	bool operator==(const baseObject &other) const{
		bool returnMe = true;
		origin.x == other.origin.x ? returnMe = true: returnMe = false;
		origin.y == other.origin.y ? returnMe = true: returnMe = false;
		width == other.width ? returnMe = true: returnMe = false;
		height == other.height ? returnMe = true: returnMe = false;
		return returnMe;
	}
};
#endif
