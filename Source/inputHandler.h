#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <Windows.h>
#include <Commdlg.h>
#include <direct.h>
#include "fmod\fmod.hpp"

#include <map>
#include "scene.h"
#include "primitives.h"
#include "levelReadWrite.h"

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
		FMOD::System *fSystem, FMOD::Sound *soundJump);
	void mouseDown(scene &mainScene, const bool &bDrawMenu, const bool &bEditing);
	void mouseUp(scene &mainScene);
	map<int, bool> getKeyMap()
	{
		return keyMap;
	}
private:
	map<int, bool> keyMap;
	void initKeyMap();
	void getFileWin(OPENFILENAME & ofn);
	primitives::vertex clickLoc, drawCenter;
};

#endif