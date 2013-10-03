#ifndef SELECTION_H
#define SELECTION_H

#include "ground.h"
#include "primitives.h"
#include "baseObject.h"
#include "collision.h"
#include <list>

class selection
{
public:
	static ground* checkSelected(primitives::vertex loc, list<ground> &groundObjs, const bool &bEditing);
	static baseObject* selection::checkSelectedMenu(primitives::vertex loc, list<baseObject> &menuItems);
	static baseObject* selection::checkSelectedOverlay(primitives::vertex loc, list<baseObject> &overlay);
};

#endif