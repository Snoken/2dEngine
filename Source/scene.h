#ifndef SCENE_H
#define SCENE_H
#define GLM_FORCE_RADIANS

#include "actor.h"
#include "ground.h"
#include "prop.h"
#include <GL/freeglut.h>
#include "soil/SOIL.h"
#include <vector>
#include <time.h>
#include <iostream>
#include "selection.h"
#include <cmath>
#include "projectile.h"
#include "bot.h"
#include "renderer.h"
#include "levelReadWrite.h"
#include "navMesh.h"

class scene
{
public:
	scene(float aspect, const string& levelFile) : m_aspect(aspect), player(NULL), selected(NULL)
	{
		//needs to happen first so textures are initialized
		renderEng = new renderer(&backgroundObjs, &foregroundObjs, &groundObjs, &menuItems,
			&overlay, &projectiles);

		player = new actor(primitives::vertex(0.0f, 0.0f), .2f*(2.0f / 3.0f), .195f);
		bot1 = new bot(primitives::vertex(-1.0f, 0.0f), .2f*(2.0f / 3.0f), .195f);

		initObjects(levelFile);
		initOverlay();
		initMenu();

		m_zoom = 1.0;
		primitives::vertex playerSize(player->width, player->height);
		m_mesh = new navMesh(groundObjs, player->getRunSpeed(), (float)player->getJumpSpeed(),
			playerSize);
	}
	void redraw(bool& bEditing, bool& bDrawOutline, bool& bDrawMenu)
	{
		renderEng->redraw(bEditing, bDrawOutline, bDrawMenu, *player, *bot1, m_zoom, 
			*selected, m_aspect, m_mouseLoc, m_clickLoc, m_drawSize, *m_mesh);
	}
	void tryDelete();
	void updateOutline(int x, int y);
	void changeZoom(float diff);

	void addProjectile(actor* owner)
	{
		projectiles.push_back(projectile(player->origin, m_clickLoc, 2.0f, owner));
	}
	void updateProjectiles(long double elapsed);
	bool needsDeleting(const primitives::vertex &loc);

	void updateActorLocations(const long double & elapsed, const long double & prevElapsed, map<int, bool>* keyMap);
	ground* getCurrentGround(baseObject* act);
	ground* getCurrentGround(primitives::vertex loc);
	ground* getCurrentCeiling(baseObject* act);

	void setMouseLoc(primitives::vertex v) { m_mouseLoc = v; }
	void setClickLoc(primitives::vertex v) { m_clickLoc = v; }
	void setCameraOffset(primitives::vertex v) { m_cameraOffset = v; }
	void setAspect(const float& aspect) { m_aspect = aspect; }
	void setDrawSize(primitives::vertex v) { m_drawSize = v; }
	primitives::vertex getMouseLoc() { return m_mouseLoc; }
	primitives::vertex getClickLoc() { return m_clickLoc; }
	primitives::vertex getCameraOffset() { return m_cameraOffset; }
	primitives::vertex getDrawSize() { return m_drawSize; }
	GLuint getSaveTexId(){ return renderEng->tSave; }
	GLuint getLoadTexId(){ return renderEng->tLoad; }
	actor* getPlayer() { return player; }
	list<baseObject>* getMenu()	{ return &menuItems; }
	list<baseObject>* getBg() {	return &backgroundObjs;	}
	list<prop>* getFg()	{ return &foregroundObjs; }
	list<ground>* getGround() {	return &groundObjs; }
	list<baseObject>* getOverlay() { return &overlay; }
	navMesh* getMesh() { return m_mesh; }
	float getZoom()	{ return m_zoom; }

	navNode* checkSelectedNode();

	bot *bot1;

private:
	void initObjects(const string& levelFile);
	void initOverlay();
	void initMenu();
	void loadTextures();

	GLuint loadTexture(const char * filename);

	list<baseObject> backgroundObjs;
	list<prop> foregroundObjs;
	list<ground> groundObjs;
	list<baseObject> menuItems;
	list<baseObject> overlay;
	list<projectile> projectiles;

	navMesh* m_mesh;

	float m_aspect, m_zoom;
	primitives::vertex m_mouseLoc, m_clickLoc, m_drawSize, m_cameraOffset;
	selection selHandler;

	actor *player;
	ground *selected;

	renderer *renderEng;
};
#endif
