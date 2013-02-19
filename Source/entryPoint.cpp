#include <iostream>
#include <math.h>
#include <windows.h>
#include "actor.h"

using namespace std;
GLfloat mouseX, mouseY;
int width(1600), height(900);
list<baseObject> objects;
list<baseObject::vertex> vertices;
actor *player = NULL;
GLfloat aspect, trans = 0.0f;

void mydisplay(){}

void mouse(int btn, int state, int x, int y)
{
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN) 
	{
		//makeSelection(mouseX, mouseY);
	}
}

/* Handler for window re-size event. Called back when the window first appears and
whenever the window is re-sized with its new width and height */
void reshape(GLsizei newWidth, GLsizei newHeigth) {  // GLsizei for non-negative integer
	height = newHeigth;
	width = newWidth;
	// Compute aspect ratio of the new window
	if (height == 0) height = 1;                // To prevent divide by 0
	aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping area to match the viewport
	glMatrixMode(GL_MODELVIEW);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset the projection matrix
	glTranslatef(trans, 0.0f, 0.0f);
	glPopMatrix();
	if (width >= height) {
		// aspect >= 1, set the height from -1 to 1, with larger width
		gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
	} else {
		// aspect < 1, set the width to -1 to 1, with larger height
		gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
	}
}

void makeObjects()
{
	vertices.clear();
	//rectangle platform origin
	baseObject::vertex orig(0.0f, -0.95f);
	//rectangle platform vertices
	vertices.push_back(baseObject::vertex(-10.0f,-1.0f));
	vertices.push_back(baseObject::vertex(-10.0f,-.9f));
	vertices.push_back(baseObject::vertex(10.0f,-.9f));
	vertices.push_back(baseObject::vertex(10.0f,-1.0f));
	objects.push_back(baseObject(orig, vertices));
	actor temp(orig, vertices);

	vertices.clear();
	//square orig
	orig = baseObject::vertex( 1.625f, -.775f );
	//square vertices
	vertices.push_back(baseObject::vertex(1.5f,-.65f));
	vertices.push_back(baseObject::vertex(1.5f,-.9f));
	vertices.push_back(baseObject::vertex(1.75f,-.9f));
	vertices.push_back(baseObject::vertex(1.75f,-.65f));
	GLfloat color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	objects.push_back(baseObject(orig, vertices, color));
}

void redraw()
{	
	glClear(GL_COLOR_BUFFER_BIT);
	//draw objects
	for( list<baseObject>::iterator objItr = objects.begin(); objItr != objects.end(); ++objItr )
	{
		glBegin(GL_POLYGON);
		glColor4f(objItr->color[0], objItr->color[1], objItr->color[2], objItr->color[3]);
		for( list<baseObject::vertex>::iterator vertItr = objItr->points.begin(); vertItr != objItr->points.end(); ++vertItr )
			glVertex2f(vertItr->x, vertItr->y);
		glEnd();
	}

	//draw player
	glBegin(GL_POLYGON);
	GLfloat *currColor = player->getColor();
	glColor4f(currColor[0], currColor[1], currColor[2], currColor[3]);
	list<baseObject::vertex> currPts = player->getPoints();
	for( list<baseObject::vertex>::iterator vertItr = currPts.begin(); vertItr != currPts.end(); ++vertItr )
		glVertex2f(vertItr->x, vertItr->y);
	glEnd();

	//draw mouse
	/*glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex2f(mouseX + .05f, mouseY);
	glVertex2f(mouseX - .05f, mouseY);
	glVertex2f(mouseX, mouseY + .05f);
	glVertex2f(mouseX, mouseY - .05f);
	glEnd();*/

	glutSwapBuffers();
}

void mykey(unsigned char key, int x, int y)
{
	if(key == 'W' || key == 'w')
	{
	}
	if(key == 'A' || key == 'a')
	{
	}
	if(key == 'D' || key == 'd')
	{
	}
	else if(key == 'S' || key == 's')
	{
	}
}

void arrows(int key, int x, int y)
{
	if(key == GLUT_KEY_LEFT)
	{
		player->moveLeft();
		//trans += .05f;
		//reshape(width, height);
	}
	else if(key == GLUT_KEY_RIGHT)
	{
		player->moveRight();
		//trans -= .05f;
		//reshape(width, height);
	}
}

void idleFunction(void)
{
	redraw();
}

/*void timerFunction(int Value)
{
glutTimerFunc(actor::intervalMS, timerFunction, 1);
}*/

void passiveMouse(int x, int y)
{
	//the cursor and the object rendering use different coord systems,
	// cursor treats upper right corner as 0, 0 and measures in pixels
	// rendering treats center of window as 0, 0 and uses floats for 
	// a percentage of the window.
	x -= width/2;
	mouseX = (float)x/(width/2);
	mouseX *= aspect;
	y -= height/2;
	y *= -1;
	mouseY = (float)y/(width/2);
	mouseY *= aspect;
}

int main(int argc, char** argv)
{
	vertices.clear();
	//square orig
	baseObject::vertex orig = baseObject::vertex( 0.0f, 0.0f );
	//square vertices
	vertices.push_back(baseObject::vertex(.075f,-.075f));
	vertices.push_back(baseObject::vertex(.075f,.075f));
	vertices.push_back(baseObject::vertex(-.075f,.075f));
	vertices.push_back(baseObject::vertex(-.075f,-.075f));
	GLfloat color[4] = {0.0f, 1.0f, 0.0f, 1.0f};
	player = new actor(orig, vertices, color);


	makeObjects();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Testing");
	glutDisplayFunc(mydisplay);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(mykey);
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR); 
	//glutTimerFunc(0, timerFunction, 0);
	glutIdleFunc(idleFunction);
	glutPassiveMotionFunc( passiveMouse );
	glutSpecialFunc(arrows);
	glutMainLoop();
	return 0;
}