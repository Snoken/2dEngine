#ifndef LEVELREADWRITE_H
#define LEVELREADWRITE_H

#include <list>
#include <string>
#include <fstream>

#include "baseObject.h"
#include "actor.h"
#include "ground.h"
#include "prop.h"

class levelReadWrite 
{
public:
	static bool writeLevel(const char* file, const list<baseObject> &backgroundObjs, const list<prop> &foregroundObjs, const list<ground> &groundObjs);
	static string readLevel(const char* file, list<baseObject> &backgroundObjs, list<prop> &foregroundObjs, list<ground> &groundObjs);
};

#endif