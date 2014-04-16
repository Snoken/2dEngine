#ifndef RENDERER_H
#define RENDERER_H
#include <vector>
#include <time.h>

// OpenGL and GLEW Header Files and Libraries
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/freeglut.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// GLM include files
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  

#include "soil/SOIL.h"

#include "actor.h"
#include "prop.h"
#include "ground.h"
#include "projectile.h"
#include "navMesh.h"

#define WIN = WIN32 || WIN64
#ifdef WIN
	#define snprintf _snprintf
#endif

class renderer {
public:
	//renderer needs to know where all object lists are stored
	renderer(list<baseObject>* backgroundObjs, list<prop>* foregroundObjs, list<ground>* groundObjs,
		list<baseObject>* menuItems, list<baseObject>* overlay, list<projectile>* projectiles): 
		backgroundObjs(backgroundObjs), foregroundObjs(foregroundObjs), groundObjs(groundObjs),
		menuItems(menuItems), overlay(overlay), projectiles(projectiles)
	{
		loadTextures();
	}
	void redraw(bool& bEditing, bool& bDrawOutline, bool& bDrawMenu, actor &player, actor &bot1,
		float zoom, ground &selected, float aspect, primitives::vertex mouseLoc, primitives::vertex clickLoc,
		primitives::vertex drawSize, navMesh &m_mesh);

	//scene needs access to textures, TODO: fix this encapsulation
	friend class scene;
private:
	void drawSky();
	void drawGrid(float &zoom);
	void drawActor(actor &act);
	void drawGround(const bool& bEditing);
	void drawBackground(const bool& bEditing);
	void drawProjectiles();
	void drawForeground();
	void drawOutline(primitives::vertex clickLoc, primitives::vertex drawSize, float &zoom);
	void drawOverlay(actor &player, float &zoom, float &aspect, ground &selected);
	void drawMenu(float &zoom, actor &player);
	void drawCursor(float &zoom, primitives::vertex mouseLoc);
	void drawNavMesh(navMesh &m_mesh);
	void drawNode(navNode &node);

	list<baseObject>* backgroundObjs;
	list<prop>* foregroundObjs;
	list<ground>* groundObjs;
	list<baseObject>* menuItems;
	list<baseObject>* overlay;
	list<projectile>* projectiles;

	GLuint loadTexture(const char * filename);
	void loadTextures();

	GLuint tSky, tSkyLower, tDirtLower, tSlide, tPaused, tSave, tLoad, tCheck, tDirtUpper;
	vector<GLuint> tCharStand, tCharRun, tCharJump, tCharRoll,
		tWallsStone, tWallsWood;
};
#endif