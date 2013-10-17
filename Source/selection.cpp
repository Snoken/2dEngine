#include "selection.h"
#include <iostream>

ground* selection::checkSelected(primitives::vertex loc, list<ground> &groundObjs, const bool &bEditing)
{
	for (list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr)
	{
		if (collision::inObject(loc, *objItr))
		{
			objItr->bSelected = !objItr->bSelected;
			return &(*objItr);
			//Not sure why i made this so complex?
			/*if (&(*objItr) == selected)
			{
				objItr->bSelected = false;
				return NULL;
			}
			else
			{
				if (selected != NULL)
					selected->bSelected = false;
				objItr->bSelected = true;
				return &(*objItr);
			}*/
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
	cout << loc.x << ", " << loc.y << endl;
	for (list<baseObject>::iterator objItr = overlay.begin(); objItr != overlay.end(); ++objItr)
	{
		cout << "\t" << objItr->origin.x << ", " << objItr->origin.y << endl;
		if (collision::inObject(loc, *objItr))
			return &(*objItr);
	}
	return NULL;
}