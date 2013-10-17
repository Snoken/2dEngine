#include "physics.h"

void physics::vector::applyGravity(double elapsed)
{
	if (elapsed == 0.0f)
		return;
	double vert = this->getVertComp();
	double horiz = this->getHorizComp();
	vert += elapsed * aGravity;
	this->angle = atan2(vert, horiz) * 180.0 / PI;
	this->magnitude = sqrt(pow(horiz, 2) + pow(vert, 2));
}
void physics::vector::changeVerticalComp(double addAmount)
{
	double vert = this->getVertComp();
	double horiz = this->getHorizComp();
	vert += addAmount;
	this->angle = atan2(vert, horiz) * 180.0 / PI;
	this->magnitude = sqrt(pow(horiz, 2) + pow(vert, 2));
}
void physics::vector::changeHorizontalComp(double addAmount)
{
	double vert = this->getVertComp();
	double horiz = this->getHorizComp();
	horiz += addAmount;
	this->angle = atan2(vert, horiz) * 180.0 / PI;
	this->magnitude = sqrt(pow(horiz, 2) + pow(vert, 2));
}
void physics::vector::setVerticalComp(double value)
{
	double vert = value;
	double horiz = this->getHorizComp();
	this->angle = atan2(vert, horiz) * 180.0 / PI;
	this->magnitude = sqrt(pow(horiz, 2) + pow(vert, 2));
}
void physics::vector::setHorizontalComp(double value)
{
	double vert = this->getVertComp();
	double horiz = value;
	this->angle = atan2(vert, horiz) * 180.0 / PI;
	this->magnitude = sqrt(pow(horiz, 2) + pow(vert, 2));
}