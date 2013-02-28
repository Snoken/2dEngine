#ifndef PROP_H
#define PROP_H

class prop : public baseObject
{
private:
	bool bCollides;

public:
	prop(vertex origin, list<vertex> points, bool collide ):baseObject(origin, points), bCollides(collide){}
	prop(vertex origin, list<vertex> points, GLfloat color[4], bool collide ):baseObject(origin, points, color), bCollides(collide){}
	prop(const baseObject& base, bool collide = false):baseObject(base), bCollides(collide){}
};
#endif