#include "renderer.h"

void renderer::redraw(bool& bEditing, bool& bDrawOutline, bool& bDrawMenu, actor &player, actor &bot1,
	float zoom, ground &selected, float aspect, primitives::vertex mouseLoc, primitives::vertex clickLoc,
	primitives::vertex drawSize, navMesh &mesh)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//each draw call is essentially a layer, the first call is the furthest back
	drawSky();
	drawBackground(bEditing);
	drawGround(bEditing);
	if (bEditing)
		drawGrid(zoom);
	drawActor(player);
	drawActor(bot1);
	drawProjectiles();
	drawForeground();
	drawNavMesh(mesh);
	if (bDrawOutline)
		drawOutline(clickLoc, drawSize, zoom);
	if (bEditing)
		drawOverlay(player, zoom, aspect, selected);
	if (bDrawMenu)
		drawMenu(zoom, player);
	drawCursor(zoom, mouseLoc);
	glutSwapBuffers();
}

void renderer::drawGrid(float &zoom)
{
	//draw the grid for guides
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
	for (float x = 0.0f; x < 100.0f; x += .05f)
	{
		glVertex2f(x, 1.0f / zoom);
		glVertex2f(x, -1.0f / zoom);
	}
	for (float x = -0.05f; x > -100.0f; x -= .05f)
	{
		glVertex2f(x, 1.0f / zoom);
		glVertex2f(x, -1.0f / zoom);
	}

	for (float y = 0.0f; y < 1 / zoom; y += .05f)
	{
		glVertex2f(-100, y);
		glVertex2f(100, y);
	}
	for (float y = -0.05f; y > -1 / zoom; y -= .05f)
	{
		glVertex2f(-100, y);
		glVertex2f(100, y);
	}
	glEnd();
}

void renderer::drawActor(actor &act)
{
	GLfloat *currColor = act.color;
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//bind appropriate texture to player, increment m_frame as appropriate based 
	//		if player is in air on player state
	if (!act.m_bOnGround)
	{
		if (act.m_bOnWall)
			glBindTexture(GL_TEXTURE_2D, tSlide);
		else
			glBindTexture(GL_TEXTURE_2D, tCharJump[(int) floor(act.m_frame)]);
	}
	else
	{
		if (act.m_state == actor::RUNNING)
		{
			if ((unsigned int) floor(act.m_frame) > tCharRun.size() - 1)
				act.m_frame = 0;
			glBindTexture(GL_TEXTURE_2D, tCharRun[(int) floor(act.m_frame)]);
		}
		else if (act.m_state == actor::ROLLING)
		{
			if ((unsigned int) floor(act.m_frame) > tCharRoll.size() - 1)
				act.m_frame = 0;
			glBindTexture(GL_TEXTURE_2D, tCharRoll[(int) floor(act.m_frame)]);
		}
		else if (act.m_state == actor::IDLE)
		{
			if ((unsigned int) floor(act.m_frame) > tCharStand.size() - 1)
				act.m_frame = 0;
			glBindTexture(GL_TEXTURE_2D, tCharStand[(int) floor(act.m_frame)]);
		}
	}

	glBegin(GL_QUADS);
	//draw textures appropriately based on movement direction
	primitives::vertex center = act.origin;
	float w = act.width, h = act.height;
	//compensate for changed player dimensions if rolling
	//NOTE: This is a hack, fix this
	if (act.m_bIsRolling)
	{
		center.y = act.yMin + h;
		w *= 2; h *= 2;
	}
	if (act.m_bFacingRight)
	{
		glTexCoord2f(0.0f, 0.0f); glVertex2f(center.x - h / 2, act.yMin - .01f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(center.x - h / 2, act.yMin + h);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(center.x + h / 2, act.yMin + h);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(center.x + h / 2, act.yMin - .01f);
	}
	else
	{
		glTexCoord2f(1.0f, 0.0f); glVertex2f(center.x - h / 2, act.yMin - .01f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(center.x - h / 2, act.yMin + h);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(center.x + h / 2, act.yMin + h);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(center.x + h / 2, act.yMin - .01f);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//draw health bar
	float width = ((act.xMax - .01f) - (act.xMin + .01f))*act.getHealth() / 100.0f;
	if (width < 0.0f)
		width = 0.0f;

	glBegin(GL_QUADS);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(act.xMin + .01f, act.yMax + .01f);
	glVertex2f(act.xMin + .01f, act.yMax + .02f);
	glVertex2f(act.xMin + .01f + width, act.yMax + .02f);
	glVertex2f(act.xMin + .01f + width, act.yMax + .01f);
	glEnd();

	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(act.xMin + .01f, act.yMax + .01f);
	glVertex2f(act.xMin + .01f, act.yMax + .02f);

	glVertex2f(act.xMin + .01f, act.yMax + .02f);
	glVertex2f(act.xMax - .01f, act.yMax + .02f);

	glVertex2f(act.xMax - .01f, act.yMax + .02f);
	glVertex2f(act.xMax - .01f, act.yMax + .01f);

	glVertex2f(act.xMax - .01f, act.yMax + .01f);
	glVertex2f(act.xMin + .01f, act.yMax + .01f);
	glEnd();
}

void renderer::drawMenu(float &zoom, actor &player)
{
	list<baseObject>::iterator itr = menuItems->begin();
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.0f, 0.0f, 0.0f, 0.9f);
	//faded black overlay
	glBegin(GL_QUADS);
	glVertex2f((itr->xMin + player.origin.x) / zoom, itr->yMin / zoom);
	glVertex2f((itr->xMin + player.origin.x) / zoom, itr->yMax / zoom);
	glVertex2f((itr->xMax + player.origin.x) / zoom, itr->yMax / zoom);
	glVertex2f((itr->xMax + player.origin.x) / zoom, itr->yMin / zoom);
	glEnd();
	++itr;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	for (; itr != menuItems->end(); ++itr)
	{
		//draw menu objects
		glBindTexture(GL_TEXTURE_2D, itr->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f((itr->xMin + player.origin.x) / zoom, itr->yMin / zoom);
		glTexCoord2f(0.0f, 1.0f); glVertex2f((itr->xMin + player.origin.x) / zoom, itr->yMax / zoom);
		glTexCoord2f(1.0f, 1.0f); glVertex2f((itr->xMax + player.origin.x) / zoom, itr->yMax / zoom);
		glTexCoord2f(1.0f, 0.0f); glVertex2f((itr->xMax + player.origin.x) / zoom, itr->yMin / zoom);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void renderer::drawSky()
{
	//glEnable(GL_TEXTURE_2D);
	//glColor3ub(255, 255, 255);
	//TODO: implement clouds
	//glEnd();
}

void renderer::drawOutline(primitives::vertex clickLoc,	primitives::vertex drawSize, float &zoom)
{
	//only draw if there's an outline to draw
	if (abs(drawSize.x) != 0.0f && abs(drawSize.y) != 0.0f)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);

		glVertex2f(clickLoc.x, clickLoc.y);
		glVertex2f(clickLoc.x, (clickLoc.y + drawSize.y));

		glVertex2f(clickLoc.x, (clickLoc.y + drawSize.y));
		glVertex2f((clickLoc.x + drawSize.x), (clickLoc.y + drawSize.y));

		glVertex2f((clickLoc.x + drawSize.x), clickLoc.y + drawSize.y);
		glVertex2f((clickLoc.x + drawSize.x), clickLoc.y);

		glVertex2f((clickLoc.x + drawSize.x), clickLoc.y);
		glVertex2f(clickLoc.x, clickLoc.y);

		//draw subdivision lines
		if (abs(drawSize.x) > abs(drawSize.y))
		{
			float endX = clickLoc.x + drawSize.x;
			float incDist;
			drawSize.x >= 0 ? incDist = abs(drawSize.y) : incDist = -abs(drawSize.y);
			while (abs(incDist) <= abs(drawSize.x))
			{
				glVertex2f((clickLoc.x + incDist), (clickLoc.y + drawSize.y));
				glVertex2f((clickLoc.x + incDist), clickLoc.y);
				if (drawSize.x >= 0)
					incDist += abs(drawSize.y);
				else
					incDist -= abs(drawSize.y);
			}
		}
		else
		{
			float endY = clickLoc.y + drawSize.y;
			float incDist;
			drawSize.y >= 0 ? incDist = abs(drawSize.x) : incDist = -abs(drawSize.x);
			while (abs(incDist) <= abs(drawSize.y))
			{
				glVertex2f(clickLoc.x + drawSize.x, clickLoc.y + incDist);
				glVertex2f(clickLoc.x, clickLoc.y + incDist);
				if (drawSize.y >= 0)
					incDist += abs(drawSize.x);
				else
					incDist -= abs(drawSize.x);
			}
		}
		glEnd();
	}
}

void renderer::drawBackground(const bool& bEditing)
{
	for (list<baseObject>::iterator objItr = backgroundObjs->begin(); objItr != backgroundObjs->end(); ++objItr)
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

void renderer::drawGround(const bool& bEditing)
{
	for (list<ground>::reverse_iterator objItr = groundObjs->rbegin(); objItr != groundObjs->rend(); ++objItr)
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

void renderer::drawForeground()
{
	for (list<prop>::iterator objItr = foregroundObjs->begin(); objItr != foregroundObjs->end(); ++objItr)
	{
		glBegin(GL_POLYGON);
		glColor4f(objItr->color[0], objItr->color[1], objItr->color[2], objItr->color[3]);
		for (list<primitives::vertex>::iterator vertItr = objItr->points.begin(); vertItr != objItr->points.end(); ++vertItr)
			glVertex2f(vertItr->x, vertItr->y);
		glEnd();
	}
}

void renderer::drawCursor(float &zoom, primitives::vertex mouseLoc)
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex2f(mouseLoc.x - .025f/zoom, mouseLoc.y);
	glVertex2f(mouseLoc.x + .025f/zoom, mouseLoc.y);

	glVertex2f(mouseLoc.x, mouseLoc.y + .025f/zoom);
	glVertex2f(mouseLoc.x, mouseLoc.y - .025f/zoom);
	glEnd();
}

void renderer::drawProjectiles()
{
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	for (list<projectile>::iterator itr = projectiles->begin(); itr != projectiles->end();
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

void renderer::drawOverlay(actor &player, float &zoom, float &aspect, ground &selected)
{
	float gap = .015f, sqWidth = .09f, startX = 0,
		overWidth = .505f, startY = .395f, offset = player.origin.x*zoom;
	glColor4f(0, 0, 0, 0.9f);
	glBegin(GL_QUADS);
	glVertex2f((aspect + offset) / zoom, 1 / zoom);
	glVertex2f((aspect + offset) / zoom, -1 / zoom);
	glVertex2f((aspect - overWidth + offset) / zoom, -1 / zoom);
	glVertex2f((aspect - overWidth + offset) / zoom, 1 / zoom);

	//sidebar
	glColor4f(1, 1, 1, 1);
	glVertex2f((aspect - overWidth + offset) / zoom, 1 / zoom);
	glVertex2f((aspect - overWidth + offset) / zoom, -1 / zoom);
	glVertex2f((aspect - overWidth - .005f + offset) / zoom, -1 / zoom);
	glVertex2f((aspect - overWidth - .005f + offset) / zoom, 1 / zoom);

	glEnd();

	glColor4f(1, 1, 1, 1);
	string temp = "Textures";
	glRasterPos2f((aspect - .325f + offset) / zoom, .925f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) temp.c_str());

	temp = "Stone";
	glRasterPos2f((aspect - .0925f + offset) / zoom, .885f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char*) temp.c_str());

	temp = "Wood";
	glRasterPos2f((aspect - .1825f + offset) / zoom, .885f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char*) temp.c_str());

	glEnable(GL_TEXTURE_2D);
	startX = aspect - gap;
	list<baseObject>::iterator itr = overlay->begin();
	for (int i = 0; (unsigned) i < tWallsStone.size() + tWallsWood.size(); ++i, ++itr)
	{
		glBindTexture(GL_TEXTURE_2D, itr->texture);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f((itr->xMin + offset) / zoom, itr->yMin / zoom);
		glTexCoord2f(0, 1); glVertex2f((itr->xMin + offset) / zoom, itr->yMax / zoom);
		glTexCoord2f(1, 1); glVertex2f((itr->xMax + offset) / zoom, itr->yMax / zoom);
		glTexCoord2f(1, 0); glVertex2f((itr->xMax + offset) / zoom, itr->yMin / zoom);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	temp = "Rotation:";
	glRasterPos2f((aspect - .485f + offset) / zoom, .355f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	while (itr != overlay->end())
	{
		glBegin(GL_QUADS);
		glColor4f(1, 1, 1, 1);
		glVertex2f((itr->xMin + offset) / zoom, itr->yMin / zoom);
		glVertex2f((itr->xMin + offset) / zoom, itr->yMax / zoom);
		glVertex2f((itr->xMax + offset) / zoom, itr->yMax / zoom);
		glVertex2f((itr->xMax + offset) / zoom, itr->yMin / zoom);
		glEnd();
		if (itr->texture == 290)
		{
			//TODO: re-work selections to get rid of this pointer stuff, possibly use
			//        std::set, will need some sort of hashing function to sort by
			if (&selected != NULL && selected.bIsPlatform)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, tCheck);
				glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f((itr->xMin + offset - .01f) / zoom, (itr->yMin - .01f) / zoom);
				glTexCoord2f(0, 1); glVertex2f((itr->xMin + offset - .01f) / zoom, (itr->yMax + .01f) / zoom);
				glTexCoord2f(1, 1); glVertex2f((itr->xMax + offset + .01f) / zoom, (itr->yMax + .01f) / zoom);
				glTexCoord2f(1, 0); glVertex2f((itr->xMax + offset + .01f) / zoom, (itr->yMin - .01f) / zoom);
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
		}
		++itr;
	}

	glColor4f(0, 0, 0, 1);
	temp = "0";
	glRasterPos2f((aspect - .35f + offset) / zoom, .355f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	temp = "90";
	glRasterPos2f((aspect - .27f + offset) / zoom, .355f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	temp = "180";
	glRasterPos2f((aspect - .185f + offset) / zoom, .355f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	temp = "270";
	glRasterPos2f((aspect - .085f + offset) / zoom, .355f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());

	glColor4f(1, 1, 1, 1);
	temp = "Properties";
	glRasterPos2f((aspect - .325f + offset) / zoom, .275f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) temp.c_str());

	temp = "Platform?";
	glRasterPos2f((aspect - .485f + offset) / zoom, .23f / zoom);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*) temp.c_str());
}

void renderer::drawNavMesh(navMesh &m_mesh)
{
	for (list<ground>::iterator objItr = groundObjs->begin(); objItr != groundObjs->end(); ++objItr)
	{
		list<navNode> m_pNodes = m_mesh.getNodesForPlatform(&(*objItr));
		for (list<navNode>::iterator nodeItr = m_pNodes.begin(); nodeItr != m_pNodes.end(); ++nodeItr)
			drawNode(*nodeItr);
	}
}

void renderer::drawNode(navNode &node)
{
	glBegin(GL_POLYGON);
	glColor4f(node.color[0], node.color[1], node.color[2], 1.0f);
	for (list<primitives::vertex>::iterator vertItr = node.points.begin(); 
		vertItr != node.points.end(); ++vertItr)
		glVertex2f(vertItr->x, vertItr->y);
	glEnd();

	list<navNode::navInfo> info = node.getDests();
	for (list<navNode::navInfo>::iterator pathItr = info.begin();
		pathItr != info.end(); ++pathItr)
	{
		glBegin(GL_LINES);
		primitives::vertex loc = node.origin;
		physics::vector moveVec = pathItr->moveVector;
		int segments = 10;
		for (int i = 0; i < segments; ++i)
		{
			glVertex2f(loc.x, loc.y);
			loc.x += (float)pathItr->travelTime / (float) segments * (float)moveVec.getHorizComp();
			float vi = (float)moveVec.getVertComp();
			moveVec.applyGravity(pathItr->travelTime / (float) segments);
			loc.y += (vi + moveVec.getVertComp())/2 * (pathItr->travelTime / (float) segments);
			glVertex2f(loc.x, loc.y);
		}
		glEnd();
		float endX = node.origin.x + (pathItr->moveVector.getHorizComp() * pathItr->travelTime);
		float endY = node.origin.y +
			pathItr->moveVector.getVertComp()*pathItr->travelTime +
			.5f*aGravity*pow((float)pathItr->travelTime, 2.0f);
		/*glBegin(GL_POLYGON);
		glVertex2f(endX + .01, endY +.01);
		glVertex2f(endX + .01, endY -.01);
		glVertex2f(endX - .01, endY -.01);
		glVertex2f(endX - .01, endY +.01);
		glEnd();*/
	}
}

GLuint renderer::loadTexture(const char * filename)
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

void renderer::loadTextures()
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
		snprintf(num, sizeof(int) , "%d", i);
		string location = "../Assets/Textures/character/stand/";
		location.append(num);
		location.append(".png");
		tCharStand.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 8; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int) , "%d", i);
		string location = "../Assets/Textures/character/walk/";
		location.append(num);
		location.append(".png");
		tCharRun.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 5; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int) , "%d", i);
		string location = "../Assets/Textures/character/jump/";
		location.append(num);
		location.append(".png");
		tCharJump.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 4; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int) , "%d", i);
		string location = "../Assets/Textures/character/roll/";
		location.append(num);
		location.append(".png");
		tCharRoll.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 5; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int) , "%d", i);
		string location = "../Assets/Textures/walls/stone/";
		location.append(num);
		location.append(".png");
		tWallsStone.push_back(loadTexture(location.c_str()));
	}
	for (int i = 1; i <= 4; ++i)
	{
		char* num = new char;
		snprintf(num, sizeof(int) , "%d", i);
		string location = "../Assets/Textures/walls/wood/";
		location.append(num);
		location.append(".png");
		tWallsWood.push_back(loadTexture(location.c_str()));
	}
}