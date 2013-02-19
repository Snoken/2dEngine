#include <iostream>
#include <math.h>
#include <windows.h>
#include "actor.h"
#include <string>
#include <iostream>
#include <math.h>
#include "physics.h"
#include "collision.h"
#include <map>

using namespace std;
GLfloat mouseX, mouseY;
int width(1600), height(900);
list<baseObject> objects;
list<baseObject::vertex> vertices;
actor *player = NULL;
GLfloat aspect, trans = 0.0f;
long double elapsed, prevElapsed;
int lastKeyPress;
double multiplier, timeOnKey, timeStartOnKey;
string lastKey = "none";
float timeToImpact = 0;
map<int, bool> keyMap;

void initKeyMap()
{
	keyMap.insert( make_pair( 'w', false ));
	keyMap.insert( make_pair( 'a', false ));
	keyMap.insert( make_pair( 's', false ));
	keyMap.insert( make_pair( 'd', false ));
}
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
	GLfloat *currColor = player->color;
	glColor4f(currColor[0], currColor[1], currColor[2], currColor[3]);
	for( list<baseObject::vertex>::iterator vertItr = player->points.begin(); vertItr != player->points.end(); ++vertItr )
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

void keyUp(unsigned char key, int x, int y)
{
	timeOnKey = 0;
	map<int, bool>::iterator currKey = keyMap.find(key);
	if( currKey != keyMap.end() )
	{
		currKey->second = false;
		if( key == 'd' || key == 'D' || key == 'a' || key == 'A' )
		{
			multiplier = 0;
			cout << endl;
		}
	}
}

void updatePlayerLocation( const long double & elapsed )
{
	if( multiplier != 0.0f )
	{
		player->move( multiplier );
		cout << multiplier << endl;
	}
	if( !player->bOnGround )
	{
		actor *temp = new actor(*player);
		physics::applyGravity( temp, elapsed );
		if(collision::timeToCollision( *temp, *(objects.begin()) ) > 0)
		{
			player=temp;
			timeToImpact = collision::timeToCollision( *temp, *(objects.begin()) );
		}
		else
			physics::moveByTime( player, timeToImpact );
	}
}

void mykey(unsigned char key, int x, int y)
{
	if(key == 'W' || key == 'w')
	{
		map<int, bool>::iterator currKey = keyMap.find('w');
		currKey->second = true;
	}
	if(key == 'A' || key == 'a')
	{
		map<int, bool>::iterator currKey = keyMap.find('a');
		currKey->second = true;
		//trans += .05f;
		//reshape(width, height);
	}
	if(key == 'D' || key == 'd')
	{
		map<int, bool>::iterator currKey = keyMap.find('d');
		currKey->second = true;
		//trans -= .05f;
		//reshape(width, height);
	}
	else if(key == 'S' || key == 's')
	{
		map<int, bool>::iterator currKey = keyMap.find('s');
		currKey->second = true;
	}
}

void arrows(int key, int x, int y)
{
	if(key == GLUT_KEY_LEFT)
	{
		
	}
	else if(key == GLUT_KEY_RIGHT)
	{
		
	}
}

void idleFunction(void)
{
	prevElapsed = elapsed;
	elapsed = glutGet(GLUT_ELAPSED_TIME)/1000.0;
	if (keyMap.find('a')->second == true)
	{
		if( multiplier == 0 )
			multiplier = -.1;
		else if( multiplier > -1.0 )
			multiplier *= 1.2;
		else
			multiplier = -1.0;
	}
	if (keyMap.find('d')->second == true)
	{
		if( multiplier == 0 )
			multiplier = .1;
		else if( multiplier < 1.0 )
			multiplier *= 1.2;
		else
			multiplier = 1.0;
	}
	if (keyMap.find('w')->second == true)
	{
		if( player->bOnGround )
		{
			player->jump();
		}
	}

	updatePlayerLocation( elapsed - prevElapsed );

	string elapsedStr = "Elapsed: " + to_string(elapsed);
	glutSetWindowTitle( elapsedStr.c_str() );
	redraw();
}

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
	initKeyMap();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Testing");
	glutDisplayFunc(mydisplay);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(mykey);
	glutKeyboardUpFunc(keyUp);
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
	glutIdleFunc(idleFunction);
	glutPassiveMotionFunc( passiveMouse );
	glutSpecialFunc(arrows);
	glutMainLoop();
	return 0;
}