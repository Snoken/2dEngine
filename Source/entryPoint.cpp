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

#define LIBGL_ALWAYS_SOFTWARE = 1

using namespace std;

bool bEditing = false;

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
	const float zoom = mainScene->getZoom();
	if (width >= height)
		// aspect >= 1, set the height from -1 to 1, with larger width
		gluOrtho2D(-1.0 * aspect / zoom, 1.0 * aspect / zoom, -1.0/zoom, 1.0/zoom);
	else
		// aspect < 1, set the width to -1 to 1, with larger height
		gluOrtho2D(-1.0 / zoom, 1.0 / zoom, -1.0 / aspect / zoom, 1.0 / aspect / zoom);
}

void keyUp(unsigned char key, int x, int y)
{
	input.handleKeyUp(key);
}

void keyPress(unsigned char key, int x, int y)
{
	input.handleKeyDown(key, bEditing, bDrawMenu);
}

void idleFunction(void)
{
	//react based on which keys are pressed
	if( !bDrawMenu )
		input.processKeys(*mainScene, bEditing, elapsed, 
			mainScene->getFSys(), mainScene->soundJump);

	//save old elapsed time
	if (elapsed == 0)
		prevElapsed = glutGet(GLUT_ELAPSED_TIME)/1000.0;
	else
		prevElapsed = elapsed;
	//get new elapsed time
	elapsed = glutGet(GLUT_ELAPSED_TIME)/1000.0;
	if (!bDrawMenu)
	{
		mainScene->updateActorLocations(elapsed - prevElapsed, input.getKeyMap());
		mainScene->updateProjectiles(elapsed - prevElapsed);
	}	

	//adjust viewpoint offset to follow player
	primitives::vertex camOffset(mainScene->getCameraOffset());
	camOffset.x = -(mainScene->getPlayer()->origin.x);
	camOffset.x /= aspect;
	camOffset.x *= mainScene->getZoom();
	mainScene->setCameraOffset(camOffset);
	reshape(width, height);

	//update windows title
	string elapsedStr = "Elapsed: " + to_string(elapsed);
	glutSetWindowTitle( elapsedStr.c_str() );

	//call redrawing of elements
	mainScene->redraw(bEditing, bDrawOutline, bDrawMenu);
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
	mouseX /= mainScene->getZoom();
	y -= height/2;
	y *= -1;
	float mouseY = (float)y/(width/2);
	mouseY *= aspect;
	mouseY /= mainScene->getZoom();
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
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		mainScene->bot1->setDest(mainScene->getMouseLoc());
	}
}

void wheel(int wheel, int direction, int x, int y)
{
	if (direction == 1)
		mainScene->changeZoom(0.05f);
	else
		mainScene->changeZoom(-0.05f);
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
	glutMouseWheelFunc(wheel);
	glutPassiveMotionFunc( passiveMouse );
	glutMotionFunc( checkUpdate );
	glutIdleFunc(idleFunction);
	//Set background color
	glClearColor(121.0f/255.0f,175.0f/255.0f,222.0f/255.0f, 1.0);
	mainScene = new scene(aspect);
	glutMainLoop();
	return 0;
}