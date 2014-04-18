#include "scene.h"

void scene::initSounds()
{
	//init FMOD
	FMOD::System_Create(&fSystem);// create an instance of the game engine
	fSystem->init(32, FMOD_INIT_NORMAL, 0);// initialise the game engine with 32 channels

	//load sounds
	fSystem->createSound("../Assets/Sounds/jump.wav", FMOD_HARDWARE, 0, &soundJump);
	soundJump->setMode(FMOD_LOOP_OFF);

	fSystem->createSound("../Assets/Sounds/run.mp3", FMOD_HARDWARE, 0, &soundRun);
	soundRun->setMode(FMOD_LOOP_OFF);

	//fSystem->createSound("../Assets/Sounds/ambient.mp3", FMOD_HARDWARE, 0, &soundMusic);
	//soundMusic->setMode(FMOD_LOOP_NORMAL);
}

void scene::tryDelete()
{
	for (list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr)
	{
		if (itr->bSelected && itr != groundObjs.begin())
		{
			m_mesh->removeEntry(&(*itr));
			groundObjs.remove(*itr);
			return;
		}
		else
			cout << "Cannot delete first ground object." << endl;
	}
}

void scene::initObjects(const string& levelFile)
{
	levelReadWrite::readLevel(levelFile.c_str(), backgroundObjs, foregroundObjs,
		groundObjs);
}

void scene::initMenu()
{
	//background overlay
	GLfloat color[4] = { 0.0f, 0.0f, 0.0f, 0.9f };
	menuItems.push_back(baseObject(
		primitives::vertex(0.0f, 0.0f), m_aspect * 2, 2.0f, color));
	//pause text
	menuItems.push_back(baseObject(
		primitives::vertex((-m_aspect + 0.1f) + .5f, 0.775f), 1.0f, .25f, renderEng->tPaused));
	//save text
	menuItems.push_back(baseObject(
		primitives::vertex((-m_aspect + 0.25f) + .25f, 0.525f), .5f, .25f, renderEng->tSave));
	//load text
	menuItems.push_back(baseObject(
		primitives::vertex((-m_aspect + 0.25f) + .25f, 0.325f), .5f, .25f, renderEng->tLoad));
}

void scene::initOverlay()
{
	float gap = .015f, sqDim = .09f, startX = 0, startY = .775f;
	startX = m_aspect - gap;
	vector<GLuint> *curr;
	for (int i = 0; i < 2; ++i)
	{
		if (i != 1)
			curr = &renderEng->tWallsStone;
		else
			curr = &renderEng->tWallsWood;
		float currY = startY;
		for (int j = 0; (unsigned) j < curr->size(); ++j)
		{
			overlay.push_back(baseObject(primitives::vertex(
				startX - (sqDim / 2), currY + .05f), sqDim, sqDim, (*curr)[j]));
			currY -= sqDim + .005f;
		}
		startX -= sqDim + .005f;
	}

	//boxes for rotation selection, textures are set to angle*10
	overlay.push_back(baseObject(primitives::vertex(
		m_aspect - .344f, .34f + .025f), .05f, .05f, 3600));

	overlay.push_back(baseObject(primitives::vertex(
		m_aspect - .256f, .34f + .025f), .05f, .05f, 900));

	overlay.push_back(baseObject(primitives::vertex(
		m_aspect - .163f, .34f + .025f), .05f, .05f, 1800));

	overlay.push_back(baseObject(primitives::vertex(
		m_aspect - .0635f, .34f + .025f), .05f, .05f, 2700));

	//platform toggle box, texture used to identify object
	overlay.push_back(baseObject(primitives::vertex(
		m_aspect - .344f, .215f + .025f), .035f, .035f, 290));
}

void scene::updateOutline(int x, int y)
{
	if (m_clickLoc == primitives::vertex())
		setClickLoc(m_mouseLoc);
	m_drawSize.x = -(m_clickLoc.x - m_mouseLoc.x);
	m_drawSize.y = -(m_clickLoc.y - m_mouseLoc.y);
}

void scene::changeZoom(float diff)
{
	m_zoom += diff;
	if (m_zoom < 0.25f)
		m_zoom = 0.25f;
}

void scene::updateProjectiles(long double elapsed)
{
	for (list<projectile>::iterator itr = projectiles.begin();
		itr != projectiles.end(); )
	{
		bool deleted = false;
		projectile temp = projectile(*itr);
		temp.update(elapsed);
		//This variable defines how many intervals on the trajectory to check
		//Instead of using a constant, this varies with the speed of the projectile
		//	This should work since slow things won't need to be checked as thoroughly
		int pointsToCheck = (int)ceil(itr->m_movement.magnitude * 2);
		for (int i = 0; i < pointsToCheck; ++i)
		{
			//move projectile by one interval
			temp.update(elapsed / pointsToCheck);
			if (temp.timedOut() || needsDeleting(temp.getLoc()))
			{
				projectiles.remove(*itr++);
				deleted = true;
				break;
			}
		}
		if (!deleted)
		{
			*itr = temp;
			++itr;
		}
	}
}

bool scene::needsDeleting(const primitives::vertex &loc)
{
	//go through all ground objects, see if collision
	for (list<ground>::iterator gItr = groundObjs.begin();
		gItr != groundObjs.end(); ++gItr)
	{
		if (collision::inObject(loc, *gItr))
		{
			return true;
		}
	}
	return false;
}

//handle motion of player
void scene::updateActorLocations(const long double & elapsed, map<int, bool>* keyMap)
{
	//figure out which ground object the player is currently above
	ground *belowPlayer = getCurrentGround(player);
	ground *abovePlayer = getCurrentCeiling(player);
	float maxDistance = 0.5f;
	map<float, ground*> nearby;
	player->getNearbyWalls(maxDistance, nearby, &groundObjs);
	if (player->isMoving() && player->m_bOnGround)
	{
		if (runChan == NULL)
		{
			#ifdef WIN32
				fSystem->playSound(soundRun, 0, false, &runChan);
			#else
				//fSystem->playSound(FMOD_CHANNEL_FREE, soundRun, false, &runChan);
			#endif
		}
		else
		{
			#ifdef WIN32
			if (runChan != NULL)
				runChan->stop();
			#endif
			runChan = NULL;
		}
	}
	player->updateLocation(elapsed, belowPlayer, abovePlayer, &nearby, keyMap);

	ground *belowbot1 = getCurrentGround(bot1);
	ground *abovebot1 = getCurrentCeiling(bot1);
	maxDistance = 0.5f;
	map<float, ground*> nearbyBot;
	bot1->getNearbyWalls(maxDistance, nearbyBot, &groundObjs);
	bot1->updateLocation(elapsed, belowbot1, abovebot1, &nearbyBot, keyMap);
}

ground* scene::getCurrentGround(baseObject* act)
{
	//figure out which ground is below given actor
	float lowestDif = 999.99f;
	ground *belowPlayer = NULL;
	for (list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr)
	{
		if (collision::above(*act, *itr) && act->yMin - itr->yMax < lowestDif)
		{
			lowestDif = act->yMin - itr->yMax;
			belowPlayer = &(*itr);
		}
	}
	return belowPlayer;
}

ground* scene::getCurrentGround(primitives::vertex loc)
{
	baseObject obj(loc, .01, .01);
	return getCurrentGround(&obj);
}

ground* scene::getCurrentCeiling(baseObject* act)
{
	//figure out which ground is below given actor
	float lowestDif = 999.99f;
	ground *abovePlayer = NULL;
	for (list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr)
	{
		if (collision::above(*itr, *act) && itr->yMin - act->yMax < lowestDif)
		{
			lowestDif = itr->yMin - act->yMax;
			abovePlayer = &(*itr);
		}
	}
	return abovePlayer;
}

navNode* scene::checkSelectedNode()
{
	for (list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr)
	{
		list<navNode> nodes = m_mesh->getNodesForPlatform(&(*objItr));
		for (list<navNode>::iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
		{
			if (collision::inObject(m_clickLoc, (baseObject)*itr))
			{
				return &(*itr);
			}
		}
	}
	return NULL;
}
