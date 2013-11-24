#include "selection.h"
#include <iostream>

//All of these functions check for selection of various object types.

ground* selection::checkSelected(primitives::vertex loc, list<ground> &groundObjs, const bool &bEditing)
{
	for (list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr)
	{
		if (collision::inObject(loc, *objItr))
		{
			objItr->bSelected = !objItr->bSelected;
			return &(*objItr);
		}
	}
	return NULL;
}

baseObject* selection::checkSelectedMenu(primitives::vertex loc, list<baseObject> &menuItems)
{
	for (list<baseObject>::iterator objItr = ++(menuItems.begin()); objItr != menuItems.end(); ++objItr)
	{
		if (collision::inObject(loc, *objItr))
			return &(*objItr);
	}
	return NULL;
}

baseObject* selection::checkSelectedOverlay(primitives::vertex loc, list<baseObject> &overlay)
{
	for (list<baseObject>::iterator objItr = overlay.begin(); objItr != overlay.end(); ++objItr)
	{
		if (collision::inObject(loc, *objItr))
			return &(*objItr);
	}
	return NULL;
}