#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#define GLM_FORCE_RADIANS

#if defined(WIN32)
	#include <Windows.h>
	#include <direct.h>
	#include "fmod/fmod.hpp"
	#include <Commdlg.h>
#else
	#include "fmodex/fmod.hpp"
#endif
#include <map>
#include "scene.h"
#include "primitives.h"
#include "levelReadWrite.h"
#include <cmath>
using namespace std;

class inputHandler
{
public:
	#define SPACEBAR 32
	#define ESC 27
	#define DEL 127
	inputHandler()
	{
		initKeyMap();
	}
	void handleKeyUp(unsigned char key);
	void handleKeyDown(unsigned char key, bool& bEditing, bool& bDrawMenu);
	void processKeys(scene &mainScene, const bool& bEditing, const long double& elapsed, 
		const double &timeDiff, FMOD::System* fSystem, FMOD::Sound* soundJump);
	void mouseDown(scene &mainScene, const bool &bDrawMenu, const bool &bEditing, float aspect);
	void mouseUp(scene &mainScene);
	map<int, bool>* getKeyMap()
	{
		return &keyMap;
	}
private:
	map<int, bool> keyMap;
	void initKeyMap();
#ifdef WIN32
	void getFileWin(OPENFILENAME & ofn);
#endif
	primitives::vertex drawCenter;
};

#endif
