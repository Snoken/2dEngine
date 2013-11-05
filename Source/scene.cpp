#include "scene.h"

void scene::redraw(bool& bEditing, bool& bDrawOutline, bool& bDrawMenu)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//each draw call is essentially a layer, the first call is the furthest back
	//drawSky();
	drawBackground(bEditing);
	drawGround(bEditing);
	if (bEditing)
		drawGrid();
	drawActor(player);
	drawActor(bot1);
	drawProjectiles();
	drawForeground();
	if (bDrawOutline)
		drawOutline();
	if (bEditing)
		drawOverlay();
	if (bDrawMenu)
		drawMenu();
	drawCursor();
	glutSwapBuffers();
	fSystem->update();
}

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

void scene::drawGrid()
{
	//draw the grid for guides
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
	for (float x = 0.0f; x < 100.0f; x += .05f)
	{
		glVertex2f(x, 1.0f / m_zoom);
		glVertex2f(x, -1.0f / m_zoom);
	}
	for (float x = -0.05f; x > -100.0f; x -= .05f)
	{
		glVertex2f(x, 1.0f / m_zoom);
		glVertex2f(x, -1.0f / m_zoom);
	}

	for (float y = 0.0f; y < 1 / m_zoom; y += .05f)
	{
		glVertex2f(-100, y);
		glVertex2f(100, y);
	}
	for (float y = -0.05f; y > -1 / m_zoom; y -= .05f)
	{
		glVertex2f(-100, y);
		glVertex2f(100, y);
	}
	glEnd();
}

void scene::drawActor(actor* act)
{
	GLfloat *currColor = act->color;
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//bind appropriate texture to player, increment m_frame as appropriate based 
	//		if player is in air on player state
	if (!act->m_bOnGround)
	{
		if (act->m_bOnWall)
			glBindTexture(GL_TEXTURE_2D, tSlide);
		else
			glBindTexture(GL_TEXTURE_2D, tCharJump[(int) floor(act->m_frame)]);
	}
	else
	{
		if (act->m_state == actor::RUNNING)
		{
			if ((unsigned int) floor(act->m_frame) > tCharRun.size() - 1)
				act->m_frame = 0;
			glBindTexture(GL_TEXTURE_2D, tCharRun[(int) floor(act->m_frame)]);
		}
		else if (act->m_state == actor::ROLLING)
		{
			if ((unsigned int) floor(act->m_frame) > tCharRoll.size() - 1)
				act->m_frame = 0;
			glBindTexture(GL_TEXTURE_2D, tCharRoll[(int) floor(act->m_frame)]);
		}
		else if (act->m_state == actor::IDLE)
		{
			if ((unsigned int) floor(act->m_frame) > tCharStand.size() - 1)
				act->m_frame = 0;
			glBindTexture(GL_TEXTURE_2D, tCharStand[(int) floor(act->m_frame)]);
		}
	}

	glBegin(GL_QUADS);
	//draw textures appropriately based on movement direction
	primitives::vertex center = act->origin;
	float w = act->width, h = act->height;
	//compensate for changed player dimensions if rolling
	//NOTE: This is a hack, fix this
	if (act->m_bIsRolling)
	{
		center.y = act->yMin + h;
		w *= 2; h *= 2;
	}
	if (act->m_bFacingRight)
	{
		glTexCoord2f(0.0f, 0.0f); glVertex2f(center.x - h / 2, act->yMin - .01f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(center.x - h / 2, act->yMin + h);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(center.x + h / 2, act->yMin + h);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(center.x + h / 2, act->yMin - .01f);
	}
	else
	{
		glTexCoord2f(1.0f, 0.0f); glVertex2f(center.x - h / 2, act->yMin - .01f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(center.x - h / 2, act->yMin + h);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(center.x + h / 2, act->yMin + h);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(center.x + h / 2, act->yMin - .01f);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//draw health bar
	float width = ((act->xMax - .01f) - (act->xMin + .01f))*act->getHealth() / 100.0f;
	if (width < 0.0f)
		width = 0.0f;

	glBegin(GL_QUADS);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(act->xMin + .01f, act->yMax + .01f);
	glVertex2f(act->xMin + .01f, act->yMax + .02f);
	glVertex2f(act->xMin + .01f + width, act->yMax + .02f);
	glVertex2f(act->xMin + .01f + width, act->yMax + .01f);
	glEnd();

	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(act->xMin + .01f, act->yMax + .01f);
	glVertex2f(act->xMin + .01f, act->yMax + .02f);

	glVertex2f(act->xMin + .01f, act->yMax + .02f);
	glVertex2f(act->xMax - .01f, act->yMax + .02f);

	glVertex2f(act->xMax - .01f, act->yMax + .02f);
	glVertex2f(act->xMax - .01f, act->yMax + .01f);

	glVertex2f(act->xMax - .01f, act->yMax + .01f);
	glVertex2f(act->xMin + .01f, act->yMax + .01f);
	glEnd();
}

void scene::drawMenu()
{
	list<baseObject>::iterator itr = menuItems.begin();
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.0f, 0.0f, 0.0f, 0.9f);
	//faded black overlay
	glBegin(GL_QUADS);
	glVertex2f((itr->xMin + player->origin.x) / m_zoom, itr->yMin / m_zoom);
	glVertex2f((itr->xMin + player->origin.x) / m_zoom, itr->yMax / m_zoom);
	glVertex2f((itr->xMax + player->origin.x) / m_zoom, itr->yMax / m_zoom);
	glVertex2f((itr->xMax + player->origin.x) / m_zoom, itr->yMin / m_zoom);
	glEnd();
	++itr;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	for (; itr != menuItems.end(); ++itr)
	{
		//draw menu objects
		glBindTexture(GL_TEXTURE_2D, itr->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f((itr->xMin + player->origin.x) / m_zoom, itr->yMin / m_zoom);
		glTexCoord2f(0.0f, 1.0f); glVertex2f((itr->xMin + player->origin.x) / m_zoom, itr->yMax / m_zoom);
		glTexCoord2f(1.0f, 1.0f); glVertex2f((itr->xMax + player->origin.x) / m_zoom, itr->yMax / m_zoom);
		glTexCoord2f(1.0f, 0.0f); glVertex2f((itr->xMax + player->origin.x) / m_zoom, itr->yMin / m_zoom);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void scene::drawSky()
{
	//glEnable(GL_TEXTURE_2D);
	//glColor3ub(255, 255, 255);
	//TODO: implement clouds
	//glEnd();
}

GLuint scene::loadTexture(const char * filename)
{
	/* load an image file directly as a new OpenGL texture */
	return SOIL_load_OGL_texture
		(
		filename,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS
		);
}

void scene::loadTextures()
{
	srand((unsigned) time(NULL));

	//load textures
	tDirtUpper = loadTexture("../Assets/Textures/platforms/dirt.png");
	tSky = loadTexture("../Assets/Textures/sky.jpg");
	tSkyLower = loadTexture("../Assets/Textures/skyLower.jpg");
	tPaused = loadTexture("../Assets/Textures/paused.png");
	tSlide = loadTexture("../Assets/Textures/character/slide/1.png");
	tLoad = loadTexture("../Assets/Textures/load.png");
	tSave = loadTexture("../Assets/Textures/save.png");
	tCheck = loadTexture("../Assets/Textures/check.png");

	for (int i = 1; i <= 4; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int), "%d", i);
		string location = "../Assets/Textures/character/stand/";
		location.append(num);
		location.append(".png");
		tCharStand.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 8; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int), "%d", i);
		string location = "../Assets/Textures/character/walk/";
		location.append(num);
		location.append(".png");
		tCharRun.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 5; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int), "%d", i);
		string location = "../Assets/Textures/character/jump/";
		location.append(num);
		location.append(".png");
		tCharJump.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 4; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int), "%d", i);
		string location = "../Assets/Textures/character/roll/";
		location.append(num);
		location.append(".png");
		tCharRoll.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 5; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int), "%d", i);
		string location = "../Assets/Textures/walls/stone/";
		location.append(num);
		location.append(".png");
		tWallsStone.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 4; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int), "%d", i);
		string location = "../Assets/Textures/walls/wood/";
		location.append(num);
		location.append(".png");
		tWallsWood.push_back(loadTexture(location.c_str()));
	}
}

void scene::drawOutline()
{
	//only draw if there's an outline to draw
	if (abs(m_drawSize.x) != 0.0f && abs(m_drawSize.y) != 0.0f)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);

		glVertex2f(m_clickLoc.x, m_clickLoc.y);
		glVertex2f(m_clickLoc.x, m_clickLoc.y + m_drawSize.y);

		glVertex2f(m_clickLoc.x, m_clickLoc.y + m_drawSize.y);
		glVertex2f(m_clickLoc.x + m_drawSize.x, m_clickLoc.y + m_drawSize.y);

		glVertex2f(m_clickLoc.x + m_drawSize.x, m_clickLoc.y + m_drawSize.y);
		glVertex2f(m_clickLoc.x + m_drawSize.x, m_clickLoc.y);

		glVertex2f(m_clickLoc.x + m_drawSize.x, m_clickLoc.y);
		glVertex2f(m_clickLoc.x, m_clickLoc.y);

		//draw subdivision lines
		if (abs(m_drawSize.x) > abs(m_drawSize.y))
		{
			float endX = m_clickLoc.x + m_drawSize.x;
			float incDist;
			m_drawSize.x >= 0 ? incDist = abs(m_drawSize.y) : incDist = -abs(m_drawSize.y);
			while (abs(incDist) <= abs(m_drawSize.x))
			{
				glVertex2f(m_clickLoc.x + incDist, m_clickLoc.y + m_drawSize.y);
				glVertex2f(m_clickLoc.x + incDist, m_clickLoc.y);
				if (m_drawSize.x >= 0)
					incDist += abs(m_drawSize.y);
				else
					incDist -= abs(m_drawSize.y);
			}
		}
		else
		{
			float endY = m_clickLoc.y + m_drawSize.y;
			float incDist;
			m_drawSize.y >= 0 ? incDist = abs(m_drawSize.x) : incDist = -abs(m_drawSize.x);
			while (abs(incDist) <= abs(m_drawSize.y))
			{
				glVertex2f(m_clickLoc.x + m_drawSize.x, m_clickLoc.y + incDist);
				glVertex2f(m_clickLoc.x, m_clickLoc.y + incDist);
				if (m_drawSize.y >= 0)
					incDist += abs(m_drawSize.x);
				else
					incDist -= abs(m_drawSize.x);
			}
		}
		glEnd();
	}
}

void scene::drawBackground(const bool& bEditing)
{
	for (list<baseObject>::iterator objItr = backgroundObjs.begin(); objItr != backgroundObjs.end(); ++objItr)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_POLYGON);
		glBindTexture(GL_TEXTURE_2D, objItr->texture);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(objItr->xMin, objItr->xMin);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(objItr->xMin, objItr->yMax);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(objItr->xMax, objItr->xMax);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(objItr->xMin, objItr->xMin);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		if (objItr->bSelected && bEditing)
		{
			glBegin(GL_POLYGON);
			glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
			glVertex2f(objItr->xMin, objItr->xMin);
			glVertex2f(objItr->xMin, objItr->yMax);
			glVertex2f(objItr->xMax, objItr->xMax);
			glVertex2f(objItr->xMin, objItr->xMin);
		}
	}
}

void scene::drawGround(const bool& bEditing)
{
	for (list<ground>::reverse_iterator objItr = groundObjs.rbegin(); objItr != groundObjs.rend(); ++objItr)
	{
		glEnable(GL_TEXTURE_2D);
		if (objItr->texture == 0)
			objItr->texture = tWallsStone[rand() % 5];

		glBindTexture(GL_TEXTURE_2D, objItr->texture);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5, 0.5, 0.0);
		glRotatef(objItr->texRotation, 0.0, 0.0, 1.0);
		glTranslatef(-0.5, -0.5, 0.0);
		glMatrixMode(GL_MODELVIEW);
		glBegin(GL_POLYGON);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if (objItr->points.size() == 4)
		{
			if (objItr->width > objItr->height)
			{
				glTexCoord2f(0.0f, 0.0f); glVertex2f(objItr->xMin, objItr->yMin);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(objItr->xMin, objItr->yMax);
				glTexCoord2f(objItr->width / objItr->height, 1.0f); glVertex2f(objItr->xMax, objItr->yMax);
				glTexCoord2f(objItr->width / objItr->height, 0.0f); glVertex2f(objItr->xMax, objItr->yMin);
			}
			else
			{
				glTexCoord2f(0.0f, 0.0f); glVertex2f(objItr->xMin, objItr->yMin);
				glTexCoord2f(0.0f, objItr->height / objItr->width); glVertex2f(objItr->xMin, objItr->yMax);
				glTexCoord2f(1.0f, objItr->height / objItr->width); glVertex2f(objItr->xMax, objItr->yMax);
				glTexCoord2f(1.0f, 0.0f); glVertex2f(objItr->xMax, objItr->yMin);
			}
		}
		else if (objItr->points.size() == 3)
		{
			list<primitives::vertex>::iterator itr = objItr->points.begin();
			glTexCoord2f(0.0f, 0.0f); glVertex2f(itr->x, itr->y);
			++itr;
			glTexCoord2f(0.0f, 1.0f); glVertex2f(itr->x, itr->y);
			++itr;
			glTexCoord2f(1.0f, 1.0f); glVertex2f(itr->x, itr->y);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);

		if (objItr->bSelected && bEditing)
		{
			glBegin(GL_POLYGON);
			glColor4f(1.0f, 0.0f, 0.0f, 0.25f);
			if (objItr->points.size() == 3)
			{
				list<primitives::vertex>::iterator itr = objItr->points.begin();
				glVertex2f(itr->x, itr->y);
				++itr;
				glVertex2f(itr->x, itr->y);
				++itr;
				glVertex2f(itr->x, itr->y);
			}
			else
			{
				glVertex2f(objItr->xMin, objItr->yMin);
				glVertex2f(objItr->xMin, objItr->yMax);
				glVertex2f(objItr->xMax, objItr->yMax);
				glVertex2f(objItr->xMax, objItr->yMin);
			}
			glEnd();
		}
	}
}

void scene::drawForeground()
{
	for (list<prop>::iterator objItr = foregroundObjs.begin(); objItr != foregroundObjs.end(); ++objItr)
	{
		glBegin(GL_POLYGON);
		glColor4f(objItr->color[0], objItr->color[1], objItr->color[2], objItr->color[3]);
		for (list<primitives::vertex>::iterator vertItr = objItr->points.begin(); vertItr != objItr->points.end(); ++vertItr)
			glVertex2f(vertItr->x, vertItr->y);
		glEnd();
	}
}

void scene::drawCursor()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex2f((m_mouseLoc.x - .025f) / m_zoom, m_mouseLoc.y / m_zoom);
	glVertex2f((m_mouseLoc.x + .025f) / m_zoom, m_mouseLoc.y / m_zoom);

	glVertex2f(m_mouseLoc.x / m_zoom, (m_mouseLoc.y + .025f) / m_zoom);
	glVertex2f(m_mouseLoc.x / m_zoom, (m_mouseLoc.y - .025f) / m_zoom);
	glEnd();
}

void scene::drawProjectiles()
{
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	for (list<projectile>::iterator itr = projectiles.begin(); itr != projectiles.end();
		++itr)
	{
		//create line of length .05 rotated itr->getRotation() degrees
		physics::vector vec(.025, itr->getRotation());
		double horiz = vec.getHorizComp();
		double vert = vec.getVertComp();
		glVertex2f((GLfloat) (itr->getLoc().x + horiz), (GLfloat) (itr->getLoc().y + vert));
		glVertex2f((GLfloat) (itr->getLoc().x - horiz), (GLfloat) (itr->getLoc().y - vert));
	}
	glEnd();
	glLineWidth(1.0f);
}

void scene::drawOverlay()
{
	float gap = .015f, sqWidth = .09f, startX = 0,
		overWidth = .505f, startY = .395f, offset = player->origin.x*m_zoom;
	glColor4f(0, 0, 0, 0.9f);
	glBegin(GL_QUADS);
	glVertex2f((m_aspect + offset) / m_zoom, 1 / m_zoom);
	glVertex2f((m_aspect + offset) / m_zoom, -1 / m_zoom);
	glVertex2f((m_aspect - overWidth + offset) / m_zoom, -1 / m_zoom);
	glVertex2f((m_aspect - overWidth + offset) / m_zoom, 1 / m_zoom);

	//sidebar
	glColor4f(1, 1, 1, 1);
	glVertex2f((m_aspect - overWidth + offset) / m_zoom, 1 / m_zoom);
	glVertex2f((m_aspect - overWidth + offset) / m_zoom, -1 / m_zoom);
	glVertex2f((m_aspect - overWidth - .005f + offset) / m_zoom, -1 / m_zoom);
	glVertex2f((m_aspect - overWidth - .005f + offset) / m_zoom, 1 / m_zoom);

	glEnd();

	glColor4f(1, 1, 1, 1);
	string temp = "Textures";
	glRasterPos2f((m_aspect - .325f + offset) / m_zoom, .925f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) temp.c_str());

	temp = "Stone";
	glRasterPos2f((m_aspect - .0925f + offset) / m_zoom, .885f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char*) temp.c_str());

	temp = "Wood";
	glRasterPos2f((m_aspect - .1825f + offset) / m_zoom, .885f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char*) temp.c_str());

	glEnable(GL_TEXTURE_2D);
	startX = m_aspect - gap;
	list<baseObject>::iterator itr = overlay.begin();
	for (int i = 0; (unsigned) i < tWallsStone.size() + tWallsWood.size(); ++i, ++itr)
	{
		glBindTexture(GL_TEXTURE_2D, itr->texture);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f((itr->xMin + offset) / m_zoom, itr->yMin / m_zoom);
		glTexCoord2f(0, 1); glVertex2f((itr->xMin + offset) / m_zoom, itr->yMax / m_zoom);
		glTexCoord2f(1, 1); glVertex2f((itr->xMax + offset) / m_zoom, itr->yMax / m_zoom);
		glTexCoord2f(1, 0); glVertex2f((itr->xMax + offset) / m_zoom, itr->yMin / m_zoom);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	temp = "Rotation:";
	glRasterPos2f((m_aspect - .485f + offset) / m_zoom, .355f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	while (itr != overlay.end())
	{
		glBegin(GL_QUADS);
		glColor4f(1, 1, 1, 1);
		glVertex2f((itr->xMin + offset) / m_zoom, itr->yMin / m_zoom);
		glVertex2f((itr->xMin + offset) / m_zoom, itr->yMax / m_zoom);
		glVertex2f((itr->xMax + offset) / m_zoom, itr->yMax / m_zoom);
		glVertex2f((itr->xMax + offset) / m_zoom, itr->yMin / m_zoom);
		glEnd();
		if (itr->texture == 290)
		{
			//TODO: re-work selections to get rid of this pointer stuff, possibly use
			//	std::set, will need some sort of hashing function to sort by
			if (selected != NULL && selected->bIsPlatform)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, tCheck);
				glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f((itr->xMin + offset - .01f) / m_zoom, (itr->yMin - .01f) / m_zoom);
				glTexCoord2f(0, 1); glVertex2f((itr->xMin + offset - .01f) / m_zoom, (itr->yMax + .01f) / m_zoom);
				glTexCoord2f(1, 1); glVertex2f((itr->xMax + offset + .01f) / m_zoom, (itr->yMax + .01f) / m_zoom);
				glTexCoord2f(1, 0); glVertex2f((itr->xMax + offset + .01f) / m_zoom, (itr->yMin - .01f) / m_zoom);
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
		}
		++itr;
	}

	glColor4f(0, 0, 0, 1);
	temp = "0";
	glRasterPos2f((m_aspect - .35f + offset) / m_zoom, .355f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	temp = "90";
	glRasterPos2f((m_aspect - .27f + offset) / m_zoom, .355f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	temp = "180";
	glRasterPos2f((m_aspect - .185f + offset) / m_zoom, .355f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	temp = "270";
	glRasterPos2f((m_aspect - .085f + offset) / m_zoom, .355f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	glColor4f(1, 1, 1, 1);
	temp = "Properties";
	glRasterPos2f((m_aspect - .325f + offset) / m_zoom, .275f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) temp.c_str());

	temp = "Platform?";
	glRasterPos2f((m_aspect - .485f + offset) / m_zoom, .23f / m_zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());
}

void scene::tryDelete()
{
	for (list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr)
	{
		if (itr->bSelected && itr != groundObjs.begin())
		{
			groundObjs.remove(*itr);
			return;
		}
		else
			cout << "Cannot delete first ground object." << endl;
	}
}

void scene::initObjects()
{
	//make basic objects for start
	groundObjs.push_back(baseObject(
		primitives::vertex(0.0f, -1.0f), 200.0f, .2f, tDirtUpper));

	groundObjs.push_back( baseObject(
	primitives::vertex( 1.625f, -.775f ), .25f, .25f, tWallsStone[rand() % 5] ) );
}

void scene::initMenu()
{
	//background overlay
	GLfloat color[4] = { 0.0f, 0.0f, 0.0f, 0.9f };
	menuItems.push_back(baseObject(
		primitives::vertex(0.0f, 0.0f), m_aspect * 2, 2.0f, color));
	//pause text
	menuItems.push_back(baseObject(
		primitives::vertex((-m_aspect + 0.1f) + .5f, 0.775f), 1.0f, .25f, tPaused));
	//save text
	menuItems.push_back(baseObject(
		primitives::vertex((-m_aspect + 0.25f) + .25f, 0.525f), .5f, .25f, tSave));
	//load text
	menuItems.push_back(baseObject(
		primitives::vertex((-m_aspect + 0.25f) + .25f, 0.325f), .5f, .25f, tLoad));
}

void scene::initOverlay()
{
	float gap = .015f, sqDim = .09f, startX = 0, startY = .775f;
	startX = m_aspect - gap;
	vector<GLuint> *curr;
	for (int i = 0; i < 2; ++i)
	{
		if (i != 1)
			curr = &tWallsStone;
		else
			curr = &tWallsWood;
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
	m_mouseLoc.x = floor(m_mouseLoc.x * 100.0f) / 100.0f;
	float proximity = fmod(m_mouseLoc.x, .05f);
	if (abs(proximity) < .025f)
		m_mouseLoc.x -= proximity;
	else
		m_mouseLoc.x >= 0 ? m_mouseLoc.x += .05f - proximity : m_mouseLoc.x += -.05f - proximity;

	m_mouseLoc.y = floor(m_mouseLoc.y * 100.0f) / 100.0f;
	proximity = fmod(m_mouseLoc.y, .05f);
	if (abs(proximity) < .025f)
		m_mouseLoc.y -= proximity;
	else
		m_mouseLoc.y >= 0 ? m_mouseLoc.y += .05f - proximity : m_mouseLoc.y += -.05f - proximity;
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
		if (temp.timedOut())
		{
			projectiles.remove(*itr++);
			deleted = true;
		}
		//TODO: once multiple players are impld, need to check if projectile
		//	is colliding with them too
		for (list<ground>::iterator gItr = groundObjs.begin();
			gItr != groundObjs.end(); ++gItr)
		{
			//check if colliding currently
			if (collision::inObject(temp.getLoc(), *gItr))
			{
				projectiles.remove(*itr++);
				deleted = true;
				break;
			}
			//check if went straight through using center point
			//NOTE: This is rather lacking for fast objects, but is the speed worth
			//	the trade off?
			else
			{
				primitives::vertex center(temp.getLoc().x - itr->getLoc().x,
					temp.getLoc().y - itr->getLoc().y);
				if (collision::inObject(center, *gItr))
				{
					projectiles.remove(*itr++);
					deleted = true;
					break;
				}
			}
		}
		if (!deleted)
		{
			*itr = temp;
			++itr;
		}
	}
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
				fSystem->playSound(FMOD_CHANNEL_FREE, soundRun, false, &runChan);
			#endif
		}
		else
		{
			if (runChan != NULL)
				runChan->stop();
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

ground* scene::getCurrentGround(actor* act)
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

ground* scene::getCurrentCeiling(actor* act)
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