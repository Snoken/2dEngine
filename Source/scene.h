#ifndef SCENE_H
#define SCENE_H
#include "actor.h"
#include "ground.h"
#include "prop.h"
#include <GL/freeglut.h>
#include "soil/SOIL.h"
#include <vector>
#include <time.h>
#include <iostream>
#include "primitives.h"
#include "selection.h"
#include <cmath>

#ifdef WIN32
	#define snprintf _snprintf
#endif

class scene
{
public:
	scene(float aspect) : m_aspect(aspect), player(NULL), selected(NULL)
	{
		player = new actor(actor(primitives::vertex(0.0f, 0.0f), .2f*(2.0f / 3.0f), .195f));
		loadTextures();
		initObjects();
		initOverlay();
		initMenu();
		m_zoom = 1.0;
	}
	void redraw(bool& bEditing, bool& bDrawOutline, bool& bDrawMenu);
	void tryDelete();
	void updateOutline(int x, int y);
	void changeZoom(float diff);

	void setMouseLoc(primitives::vertex v)
	{
		m_mouseLoc = v;
	}
	void setClickLoc(primitives::vertex v)
	{
		m_clickLoc = v;
	}
	void setCameraOffset(primitives::vertex v)
	{
		m_cameraOffset = v;
	}
	void setAspect(const float& aspect)
	{
		m_aspect = aspect;
	}
	void setDrawSize(primitives::vertex v)
	{
		m_drawSize = v;
	}
	primitives::vertex getMouseLoc()
	{
		return m_mouseLoc;
	}
	primitives::vertex getClickLoc()
	{
		return m_clickLoc;
	}
	primitives::vertex getCameraOffset()
	{
		return m_cameraOffset;
	}
	primitives::vertex getDrawSize()
	{
		return m_drawSize;
	}
	actor* getPlayer()
	{
		return player;
	}
	list<baseObject>* getMenu()
	{
		return &menuItems;
	}
	list<baseObject>* getBg()
	{
		return &backgroundObjs;
	}
	list<prop>* getFg()
	{
		return &foregroundObjs;
	}
	list<ground>* getGround()
	{
		return &groundObjs;
	}
	list<baseObject>* getOverlay()
	{
		return &overlay;
	}
	float getZoom()
	{
		return m_zoom;
	}
	GLuint tPaused, tSave, tLoad, tCheck;

private:
	void initObjects();
	void initOverlay();
	void initMenu();
	void loadTextures();

	void drawSky();
	void drawGrid();
	void drawPlayer();
	void drawGround(const bool& bEditing);
	void drawBackground(const bool& bEditing);
	void drawForeground();
	void drawOutline();
	void drawOverlay();
	void drawMenu();
	void drawCursor();

	GLuint loadTexture(const char * filename);

	list<baseObject> backgroundObjs;
	list<prop> foregroundObjs;
	list<ground> groundObjs;
	list<baseObject> menuItems;
	list<baseObject> overlay;

	float m_aspect, m_zoom;
	primitives::vertex m_mouseLoc, m_clickLoc, m_drawSize, m_cameraOffset;
	selection selHandler;

	actor *player;
	ground *selected;

	GLuint tSky, tSkyLower, tDirtUpper, tDirtLower, tSlide;
	vector<GLuint> tCharStand, tCharRun, tCharJump, tCharRoll,
		tWallsStone, tWallsWood;
};
#endif
