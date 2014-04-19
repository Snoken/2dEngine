#define _CRT_SECURE_NO_DEPRECATE
#define GLM_FORCE_RADIANS

#include "soil/SOIL.h"
//#include "GL/freeglut.h"
//#include "GL/glut.h"

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

using namespace std;

//TODO: move this to scene
//bool determining whether to enable editor
bool bDrawMenu = false, bDrawOutline = false, bEditing = false;

//initial window size, aspect ratio
int width(1600), height(900), framesElapsed(0);
float aspect = (float)width/height, trans = 0.0f;

//scene holds all important data, handles updating almost everything
scene* mainScene = NULL;

//pretty self explanatory, click or press a button, this guy gets used
inputHandler input;

//time keeping
long double elapsed, prevElapsed, musicStart, lastFPSUpdate;

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
	const float zoom = mainScene->getZoom();
	if (width >= height)
		// aspect >= 1, set the height from -1 to 1, with larger width
		gluOrtho2D(-1.0 * aspect / zoom, 1.0 * aspect / zoom, -1.0/zoom, 1.0/zoom);
	else
		// aspect < 1, set the width to -1 to 1, with larger height
		gluOrtho2D(-1.0 / zoom, 1.0 / zoom, -1.0 / aspect / zoom, 1.0 / aspect / zoom);
}

//void function needed for glut
void keyUp(unsigned char key, int x, int y)
{
	input.handleKeyUp(key);
}

void passiveMouse(int x, int y)
{
	//the cursor and the object rendering use different coord systems,
	// cursor treats upper right corner as 0, 0 and measures in pixels
	// rendering treats center of window as 0, 0 and uses floats for 
	// a percentage of the window, so need to convert.

	x -= width / 2;
	float mouseX = (float) x / (width / 2);
	mouseX *= aspect;
	mouseX /= mainScene->getZoom();
	mouseX += mainScene->getPlayer()->origin.x;

	y -= height / 2;
	y *= -1;
	float mouseY = (float) y / (width / 2);
	mouseY *= aspect;
	mouseY /= mainScene->getZoom();
	//mouseX += mainScene->getPlayer()->origin.y;

	primitives::vertex loc(mouseX, mouseY);
	mainScene->setMouseLoc(loc);
}

//void function needed for glut
void keyPress(unsigned char key, int x, int y)
{
	input.handleKeyDown(key, bEditing, bDrawMenu);
}

/* This one gets called every single frame, heavy lifter which executes
	all calls necessarry on a per-frame basis. */
void idleFunction(void)
{
	//save old elapsed time, convert from ms to s
	if (elapsed == 0)
		prevElapsed = lastFPSUpdate = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	else
		prevElapsed = elapsed;

	//get new elapsed time
	elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

	++framesElapsed;
	//react based on which keys are pressed, only mouse is used in menu
	if( !bDrawMenu )
		input.processKeys(*mainScene, bEditing, elapsed, 
			elapsed - prevElapsed, mainScene->getFSys(), mainScene->soundJump);

	//TODO: once MP is impld, updateActorLocations will need to be changed to two funcs
	//	one to move the local player, and one to update AI which is only used by server.
	//if the menu is not up, update all actors and projectiles
	if (!bDrawMenu)
	{
		mainScene->updateActorLocations(elapsed, prevElapsed, input.getKeyMap());
		mainScene->updateProjectiles(elapsed - prevElapsed);
	}	

	//adjust viewpoint offset to follow player
	primitives::vertex camOffset(mainScene->getCameraOffset());
	camOffset.x = -(mainScene->getPlayer()->origin.x);
	camOffset.x /= aspect;
	camOffset.x *= mainScene->getZoom();
	mainScene->setCameraOffset(camOffset);
	//Call reshape to update viewport/camera location
	reshape(width, height);

	//update window title
	if (elapsed - lastFPSUpdate > 0.5)
	{
		string elapsedStr = "FPS: " + to_string(framesElapsed / (elapsed - lastFPSUpdate));
		glutSetWindowTitle(elapsedStr.c_str());
		lastFPSUpdate = elapsed;
		framesElapsed = 0;
	}

	//call redrawing of all elements
	mainScene->redraw(bEditing, bDrawOutline, bDrawMenu);
}

void mouse(int btn, int state, int x, int y)
{
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN) 
	{
		input.mouseDown(*mainScene, bDrawMenu, bEditing, aspect);
	}
	else if(btn==GLUT_LEFT_BUTTON && state==GLUT_UP)
	{
		//no action to take unless editing
		if (bEditing)
		{
			bDrawOutline = false;
			input.mouseUp(*mainScene);
		}
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//this is just for testing, will be changed as more features are added
		mainScene->bot1->setDest(mainScene->getMouseLoc(),
			mainScene->getCurrentGround(mainScene->bot1),
			mainScene->getCurrentGround(mainScene->getMouseLoc()), 
			mainScene->getMesh()->getNavGraph());
	}
	//scroll up
	else if (btn == 3)
                mainScene->changeZoom(0.05f);
        else if (btn == 4)
                mainScene->changeZoom(-0.05f);

}

void wheel(int wheel, int direction, int x, int y)
{
	//pretty simple, change zoom
	if (direction == 1)
		mainScene->changeZoom(0.05f);
	else
		mainScene->changeZoom(-0.05f);
}

//This function is called when the mouse moves
void checkUpdate(int x, int y)
{
	passiveMouse(x, y);
	//if we're editing, update the outline of the shape currently being drawn
	if (bEditing) 
	{
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
	glutMouseWheelFunc(wheel);
	glutPassiveMotionFunc( passiveMouse );
	glutMotionFunc( checkUpdate );
	glutIdleFunc(idleFunction);
	//Set background color (light blue)
	glClearColor(121.0f/255.0f,175.0f/255.0f,222.0f/255.0f, 1.0);
	string levelFile;
	if(argc == 2)
		levelFile = string(argv[1]);
	else
		levelFile = "testlvl.lvl";
	mainScene = new scene(aspect, levelFile);
	//run the main loop, this only ends when the window closes
	glutMainLoop();
	return 0;
}
