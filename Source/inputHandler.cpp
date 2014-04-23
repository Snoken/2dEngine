#include "inputHandler.h"

void inputHandler::initKeyMap()
{
	//add all keys to map

	//basic motion
	keyMap.insert(make_pair('w', false));
	keyMap.insert(make_pair(SPACEBAR, false));
	keyMap.insert(make_pair('a', false));
	keyMap.insert(make_pair('s', false));
	keyMap.insert(make_pair('d', false));

	//set bot to follow
	keyMap.insert(make_pair('f', false));

	//drop down through platform
	keyMap.insert(make_pair('q', false));

	//toggle editor
	keyMap.insert(make_pair('o', false));

	keyMap.insert(make_pair(ESC, false));
	keyMap.insert(make_pair(DEL, false));

}

void inputHandler::handleKeyUp(unsigned char key)
{
	//adjust to lower case if needed
	if (key >= 'A' && key <= 'Z')
		key += 32;
	map<int, bool>::iterator currKey = keyMap.find(key);
	//if the released key is in the map unset pressed
	if (currKey != keyMap.end())
		currKey->second = false;
}

void inputHandler::handleKeyDown(unsigned char key, bool& bEditing, bool& bDrawMenu)
{
	//adjust to lower case if needed
	if (key >= 'A' && key <= 'Z')
		key += 32;
	//set appropriate key to pressed
	if (keyMap.find(key) != keyMap.end())
	{
		if (key == ESC) //escape key
			bDrawMenu = !bDrawMenu;
		else if (key == 'o')
			bEditing = !bEditing;
		map<int, bool>::iterator currKey = keyMap.find(key);
		currKey->second = true;
	}
}

void inputHandler::processKeys(scene &mainScene, const bool& bEditing, 
	const long double& elapsed, const double &timeDiff)
{
	if (bEditing && keyMap.find(DEL)->second == true)
		mainScene.tryDelete();
	actor* player = mainScene.getPlayer();
	if (keyMap.find('a')->second == false && keyMap.find('d')->second == false && !player->m_bIsRolling)
		//decay the multiplier if no keys are being pressed
		player->decayMult(timeDiff);
	
	if (keyMap.find('s')->second == true)
	{
		if (player->m_bOnGround && !player->m_bIsRolling && player->isMoving())
			player->startRoll(elapsed);
	}
	else if (keyMap.find('a')->second == true)
	{
		if (!player->m_bOnWall)
			player->m_bFacingRight = false;
		player->updateMult(timeDiff, "left");
	}
	else if (keyMap.find('d')->second == true)
	{
		if (!player->m_bOnWall)
			player->m_bFacingRight = true;
		player->updateMult(timeDiff, "right");
	}
	if (keyMap.find('w')->second == true || keyMap.find(SPACEBAR)->second == true)
	{
		if (player->m_bOnGround || player->m_bOnWall)
			player->m_bJump = true;
	}
	if (keyMap.find('f')->second == true)
	{
		mainScene.bot1->toggleFollow(elapsed);
		keyMap.find('f')->second = false;
	}
}

void inputHandler::mouseDown(scene &mainScene, const bool &bDrawMenu, const bool &bEditing, float aspect)
{
	primitives::vertex clickLoc = mainScene.getMouseLoc();
	primitives::vertex overlayLoc = clickLoc;
	overlayLoc.x -= mainScene.getPlayer()->origin.x;
	overlayLoc.x *= mainScene.getZoom();
	overlayLoc.y *= mainScene.getZoom();

	if (bDrawMenu)
	{
		primitives::vertex loc = clickLoc;
		clickLoc.x -= mainScene.getPlayer()->origin.x;
		baseObject *selected = selection::checkSelectedMenu(overlayLoc, *mainScene.getMenu());
		if (selected != NULL && selected->texture == mainScene.getSaveTexId())
		{
			#ifdef WIN32
				OPENFILENAME fm;
				getFileWin(fm);
				char str[260];
				strcpy_s(str, fm.lpstrFile);
			#else
				//TODO: Implement linux file handler
				char str[]="empty";
			#endif
			levelReadWrite::writeLevel(str, *mainScene.getBg(), 
				*mainScene.getFg(), *mainScene.getGround());
		}
		else if (selected != NULL && selected->texture == mainScene.getLoadTexId())
		{
			#ifdef WIN32
				OPENFILENAME fm;
				getFileWin(fm);
				char str[260];
				strcpy_s(str, fm.lpstrFile);
			#else
				//TODO: Implement linux file handler
				char str[]="empty";
			#endif
			string result = levelReadWrite::readLevel(str, *mainScene.getBg(),
				*mainScene.getFg(), *mainScene.getGround());
			if (result != "success")
				cout << result << endl;

		}
	}
	else if (bEditing)
	{
		ground* selected = selection::checkSelected(clickLoc, *mainScene.getGround(), bEditing);
		baseObject *overlaySelected = selection::checkSelectedOverlay(overlayLoc,
			*mainScene.getOverlay());
		if (overlaySelected != NULL)
		{
			list<ground>* groundObjs = mainScene.getGround();
			for (list<ground>::iterator itr = groundObjs->begin(); itr != groundObjs->end(); ++itr)
			{
				if (itr->bSelected)
				{
					//This bit of nonsense is a hack to use texture plat numbers to
					//  detect which button has been clicked
					//TODO: find a better way, enum of buttons?
					if (overlaySelected->texture == 290)
						itr->bIsPlatform = !itr->bIsPlatform;
					else if (overlaySelected->texture >= 900)
						itr->texRotation = (float) (overlaySelected->texture / 10 % 360);
					else
						itr->texture = overlaySelected->texture;
				}
			}
		}
		/*clickLoc.x /= mainScene.getZoom();
		clickLoc.y /= mainScene.getZoom();*/
		clickLoc.roundToNearest(.05f);
		mainScene.setClickLoc(clickLoc);
	}
	else
	{
		mainScene.setClickLoc(clickLoc);
		navNode *node = mainScene.checkSelectedNode();
		if (node != NULL)
		{
			cout << *node << endl;
		}
		else
			mainScene.addProjectile(mainScene.getPlayer());
	}
}

void inputHandler::mouseUp(scene &mainScene, const bool &bEditing)
{
	if (bEditing)
	{
		primitives::vertex clickLoc = mainScene.getClickLoc();
		primitives::vertex mouseLoc = mainScene.getMouseLoc();

		primitives::vertex drawSize;
		drawSize.x = mouseLoc.x - clickLoc.x;
		drawSize.y = mouseLoc.y - clickLoc.y;
		//drawSize.roundToNearest(.05f);
		drawCenter.x = clickLoc.x + drawSize.x / 2;
		drawCenter.y = clickLoc.y + drawSize.y / 2;
		//drawCenter.roundToNearest(.025f);
		//prevent accidental creating
		if (abs(drawSize.x) > .025 && abs(drawSize.y) > .025)
		{
			mainScene.getGround()->push_back(baseObject(drawCenter, drawSize.x, drawSize.y));
			drawSize.x = drawSize.y = 0.0f;
			mainScene.setDrawSize(drawSize);
			mainScene.setClickLoc(primitives::vertex());
		}
	}
}

#ifdef _WIN32
void inputHandler::getFileWin(OPENFILENAME & ofn)
{
	char szFile[260];       // buffer for file name
	HWND hwnd = NULL;              // owner window

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Level Files (*.lvl)\0*.lvl\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST;

	// Display the Open dialog box. 
	GetOpenFileName(&ofn);
}
#endif
