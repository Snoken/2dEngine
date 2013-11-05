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
#include "selection.h"
#include <cmath>
#include "projectile.h"
#include "bot.h"

#define WIN = WIN32 || WIN64
#ifdef WIN
	#include "fmod/fmod.hpp"
#else
	#include "fmodex/fmod.hpp"
#endif

#ifdef WIN
	#define snprintf _snprintf
#endif

class scene
{
public:
	scene(float aspect) : m_aspect(aspect), player(NULL), selected(NULL)
	{
		player = new actor(primitives::vertex(0.0f, 0.0f), .2f*(2.0f / 3.0f), .195f);
		bot1 = new bot(primitives::vertex(-1.0f, 0.0f), .2f*(2.0f / 3.0f), .195f);
		loadTextures();
		initObjects();
		initOverlay();
		initMenu();
		m_zoom = 1.0;
		initSounds();
		#ifdef WIN32
				//fSystem->playSound(soundMusic, 0, false, 0);
		#else
				fSystem->playSound(FMOD_CHANNEL_FREE, soundMusic, false, 0);
		#endif
	}
	void redraw(bool& bEditing, bool& bDrawOutline, bool& bDrawMenu);
	void initSounds();
	void tryDelete();
	void updateOutline(int x, int y);
	void changeZoom(float diff);

	void addProjectile()
	{
		projectiles.push_back(projectile(player->origin, m_clickLoc, 1.0f));
	}
	void updateProjectiles(long double elapsed);
	void damageActor(actor* victim, projectile &proj)
	{
		victim->takeDamage((float)proj.getDamage());
	}

	void updateActorLocations(const long double & elapsed, map<int, bool>* keyMap);
	ground* getCurrentGround(actor* act);
	ground* getCurrentCeiling(actor* act);

	void setMouseLoc(primitives::vertex v) { m_mouseLoc = v; }
	void setClickLoc(primitives::vertex v) { m_clickLoc = v; }
	void setCameraOffset(primitives::vertex v) { m_cameraOffset = v; }
	void setAspect(const float& aspect) { m_aspect = aspect; }
	void setDrawSize(primitives::vertex v) { m_drawSize = v; }
	primitives::vertex getMouseLoc() { return m_mouseLoc; }
	primitives::vertex getClickLoc() { return m_clickLoc; }
	primitives::vertex getCameraOffset() { return m_cameraOffset; }
	primitives::vertex getDrawSize() { return m_drawSize; }
	actor* getPlayer() { return player; }
	list<baseObject>* getMenu()	{ return &menuItems; }
	list<baseObject>* getBg() {	return &backgroundObjs;	}
	list<prop>* getFg()	{ return &foregroundObjs; }
	list<ground>* getGround() {	return &groundObjs; }
	list<baseObject>* getOverlay() { return &overlay; }
	float getZoom()	{ return m_zoom; }
	FMOD::System* getFSys() { return fSystem; }

	bot *bot1;
	GLuint tPaused, tSave, tLoad, tCheck;
	FMOD::Sound      *soundJump, *soundMusic, *soundRun; //sound that will be loaded and played

private:
	void initObjects();
	void initOverlay();
	void initMenu();
	void loadTextures();

	void drawSky();
	void drawGrid();
	void drawActor(actor* act);
	void drawGround(const bool& bEditing);
	void drawBackground(const bool& bEditing);
	void drawProjectiles();
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
	list<projectile> projectiles;

	float m_aspect, m_zoom;
	primitives::vertex m_mouseLoc, m_clickLoc, m_drawSize, m_cameraOffset;
	selection selHandler;

	actor *player;
	ground *selected;

	GLuint tSky, tSkyLower, tDirtUpper, tDirtLower, tSlide;
	vector<GLuint> tCharStand, tCharRun, tCharJump, tCharRoll,
		tWallsStone, tWallsWood;

	//FMod Stuff
	FMOD::System     *fSystem; //handle to FMOD engine
	FMOD::Channel	 *runChan;
};
#endif
