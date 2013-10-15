#define _CRT_SECURE_NO_DEPRECATE
#include "soil/SOIL.h"
#include "GL/freeglut.h"
#include "GL/glut.h"

#include <string>
#include <map>
#include <iostream>
#include <math.h>
#include <random>
#include <time.h>

#include "actor.h"
#include "ground.h"
#include "prop.h"
#include "collision.h"
#include "levelReadWrite.h"
#include "scene.h"
#include "inputHandler.h"
#include "fmodex/fmod.hpp"
#include "fmodex/fmod.h"

#define LIBGL_ALWAYS_SOFTWARE = 1

using namespace std;

bool bEditing = false;
//FMod Stuff
FMOD::System     *fSystem; //handle to FMOD engine
FMOD::Sound      *soundJump, *soundMusic, *soundRun; //sound that will be loaded and played
FMOD::Channel	 *runChan = NULL;

//global vars
int width(1600), height(900);

float aspect = (float)width/height, trans = 0.0f;
scene* mainScene = NULL;
inputHandler input;
long double elapsed, prevElapsed, musicStart;
bool bDrawMenu, bDrawOutline;

//glut needs this even if it's empty
void disp(){}

/* Handler for window re-size event. Called back when the window first appears and
	whenever the window is re-sized with its new width and height, also called by idle
	function to update viewport*/
void reshape(GLsizei newWidth, GLsizei newHeigth) {  // GLsizei for non-negative integer
	if (newHeigth != height || newWidth != width)
	{
		height = newHeigth;
		width = newWidth;
		// Compute aspect ratio of the new window
		if (height == 0) height = 1; // To prevent divide by 0
		aspect = (GLfloat) width / (GLfloat) height;
		mainScene->setAspect(aspect);

		// Set the viewport to cover the new window
		glViewport(0, 0, width, height);
	}

	// Set the aspect ratio of the clipping area to match the viewport
	glMatrixMode(GL_MODELVIEW); //To operate on the Projection matrix
	glLoadIdentity(); //Reset the projection matrix
	glTranslatef(mainScene->getCameraOffset().x, mainScene->getCameraOffset().y, 0.0f); //move camera to player location
	glPopMatrix();
	if (width >= height)
		// aspect >= 1, set the height from -1 to 1, with larger width
		gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
	else
		// aspect < 1, set the width to -1 to 1, with larger height
		gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
}

void keyUp(unsigned char key, int x, int y)
{
	input.handleKeyUp(key);
}

ground *getCurrentGround( actor one )
{
	//figure out which ground is below given actor
	float lowestDif = 999.99f;
	ground *belowPlayer = NULL;
	list<ground>* allGround = mainScene->getGround();
	for (list<ground>::iterator itr = allGround->begin(); itr != allGround->end(); ++itr)
	{
		if( collision::above( one, *itr ) && one.yMin - itr->yMax < lowestDif )
		{
			lowestDif = one.yMin - itr->yMax;
			belowPlayer = &(*itr);
		}
	}
	return belowPlayer;
}

ground *getCurrentCeiling( actor one )
{
	//figure out which ground is below given actor
	float lowestDif = 999.99f;
	ground *abovePlayer = NULL;
	list<ground>* allGround = mainScene->getGround();
	for (list<ground>::iterator itr = allGround->begin(); itr != allGround->end(); ++itr)
	{
		if( collision::above( *itr, one ) && itr->yMin - one.yMax < lowestDif )
		{
			lowestDif = itr->yMin - one.yMax;
			abovePlayer = &(*itr);
		}
	}
	return abovePlayer;
}

void getNearbyWalls( actor one, const float & maxDistance, list<ground> &nearby )
{
	//get list of walls nearest actor
	list<ground>* allGround = mainScene->getGround();
	for (list<ground>::iterator itr = allGround->begin(); itr != allGround->end(); ++itr)
	{
		float gapSize = 0.0f;
		if( itr->xMin > one.xMax )
			gapSize = itr->xMin - one.xMax;
		else
			gapSize = one.xMin - itr->xMax;

		if( gapSize < maxDistance && collision::nextTo( one, *itr ) && !collision::above( one, *itr ) )
			nearby.push_back( *itr );
	}
}

//handle motion of player
void updatePlayerLocation( const long double & elapsed )
{
	if( !bDrawMenu )
	{
		//figure out which ground object the player is currently above
		ground *belowPlayer = getCurrentGround( *mainScene->getPlayer() );
		ground *abovePlayer = getCurrentCeiling( *mainScene->getPlayer() );
		float maxDistance = 0.5f;
		list<ground> nearby;
		getNearbyWalls( *mainScene->getPlayer(), maxDistance, nearby);
		if (mainScene->getPlayer()->getMult() != 0.0 && mainScene->getPlayer()->m_bOnGround)
		{
			if( runChan == NULL )
				#ifdef WIN32
				fSystem->playSound(soundRun, 0, false, &runChan);
                        	#else
                                fSystem->playSound(FMOD_CHANNEL_FREE, soundRun, false, &runChan);
                        	#endif
		}
		else
		{
			if( runChan != NULL )
				runChan->stop();
			runChan = NULL;
		}
		mainScene->getPlayer()->updateLocation(elapsed, belowPlayer, abovePlayer, 
			&nearby, input.getKeyMap());
	}
}

void keyPress(unsigned char key, int x, int y)
{
	input.handleKeyDown(key, bEditing, bDrawMenu);
}

void idleFunction(void)
{
	//react based on which keys are pressed
	if( !bDrawMenu )
		input.processKeys(*mainScene, bEditing, elapsed, fSystem, soundJump);

	//save old elapsed time
	if (elapsed == 0)
		prevElapsed = glutGet(GLUT_ELAPSED_TIME)/1000.0;
	else
		prevElapsed = elapsed;
	//get new elapsed time
	elapsed = glutGet(GLUT_ELAPSED_TIME)/1000.0;
	updatePlayerLocation( elapsed - prevElapsed );

	//adjust viewpoint offset to follow player
	primitives::vertex camOffset(mainScene->getCameraOffset());
	camOffset.x = -(mainScene->getPlayer()->origin.x);
	camOffset.x /= aspect;
	mainScene->setCameraOffset(camOffset);
	reshape(width, height);

	//update windows title
	string elapsedStr = "Elapsed: " + to_string(elapsed);
	glutSetWindowTitle( elapsedStr.c_str() );

	//call redrawing of elements
	mainScene->redraw(bEditing, bDrawOutline, bDrawMenu);
	fSystem->update();
}

void initSounds()
{
	//init FMOD
	FMOD::System_Create(&fSystem);// create an instance of the game engine
	fSystem->init(32, FMOD_INIT_NORMAL, 0);// initialise the game engine with 32 channels

	//load sounds
	fSystem->createSound("../Assets/Sounds/jump.wav", FMOD_HARDWARE, 0, &soundJump);
	soundJump->setMode(FMOD_LOOP_OFF);

	fSystem->createSound("../Assets/Sounds/run.mp3", FMOD_HARDWARE, 0, &soundRun);
	soundRun->setMode(FMOD_LOOP_OFF);

	fSystem->createSound("../../Assets/Sounds/ambient.mp3", FMOD_HARDWARE, 0, &soundMusic);
	soundMusic->setMode(FMOD_LOOP_NORMAL);
}
void passiveMouse(int x, int y)
{
	//the cursor and the object rendering use different coord systems,
	// cursor treats upper right corner as 0, 0 and measures in pixels
	// rendering treats center of window as 0, 0 and uses floats for 
	// a percentage of the window.
	x -= width/2;
	float mouseX = (float)x/(width/2);
	mouseX *= aspect;
	mouseX += mainScene->getPlayer()->origin.x;
	y -= height/2;
	y *= -1;
	float mouseY = (float)y/(width/2);
	mouseY *= aspect;
	mainScene->setMouseLoc(primitives::vertex(mouseX, mouseY));
}

void mouse(int btn, int state, int x, int y)
{
	passiveMouse(x, y);
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN) 
	{
		input.mouseDown(*mainScene, bDrawMenu, bEditing);
	}
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_UP)
	{
		if (bEditing)
		{
			bDrawOutline = false;
			input.mouseUp(*mainScene);
		}
	}
}

void checkUpdate(int x, int y)
{
	if (bEditing) 
	{
		passiveMouse(x, y);
		bDrawOutline = true;
		mainScene->updateOutline(x, y);
	}
}

int main(int argc, char** argv)
{
	//glut, glut, and more glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Platformer");
	glutDisplayFunc(disp);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyUp);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc( passiveMouse );
	glutMotionFunc( checkUpdate );
	glutIdleFunc(idleFunction);

	mainScene = new scene(aspect);
	initSounds();
	#ifdef WIN32
		fSystem->playSound(soundMusic, 0, false, 0);
       	#else
                fSystem->playSound(FMOD_CHANNEL_FREE, soundMusic, false, 0);
       	#endif

	glutMainLoop();
	return 0;
}
