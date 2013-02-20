#include <iostream>
#include <math.h>
#include <windows.h>
#include "actor.h"
#include <string>
#include <iostream>
#include <math.h>
#include "physics.h"
#include "collision.h"
#include "ground.h"
#include <map>

using namespace std;

//global containers
list<baseObject> objects;
list<ground> groundObjs;
list<baseObject::vertex> vertices;
map<int, bool> keyMap;

//global vars
int width(1600), height(900);
actor *player = NULL;
float aspect, trans = 0.0f, timeToImpact = 0.0f;
long double elapsed, prevElapsed;
double multiplier;

void initKeyMap()
{
	keyMap.insert( make_pair( 'w', false ));
	keyMap.insert( make_pair( 'a', false ));
	keyMap.insert( make_pair( 's', false ));
	keyMap.insert( make_pair( 'd', false ));
}

//glut needs this even when it's empty for some reason
void disp(){}

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

baseObject makeRectangle( baseObject::vertex origin, float width, float height )
{
	list<baseObject::vertex> rectVerts;
	rectVerts.push_back(baseObject::vertex(origin.x - width/2, origin.y - height/2));
	rectVerts.push_back(baseObject::vertex(origin.x - width/2, origin.y + height/2));
	rectVerts.push_back(baseObject::vertex(origin.x + width/2, origin.y + height/2));
	rectVerts.push_back(baseObject::vertex(origin.x + width/2, origin.y - height/2));
	return baseObject(origin, rectVerts);
}

void makeObjects()
{
	groundObjs.push_back( makeRectangle( baseObject::vertex( 0.5f, -0.7f ), .5f, .05f ) );
	groundObjs.push_back( makeRectangle( baseObject::vertex( 0.75f, -0.55f ), .5f, .05f ) );

	vertices.clear();
	//rectangle platform origin
	baseObject::vertex orig(-6.5f, -0.725f);
	//rectangle platform vertices
	vertices.push_back(baseObject::vertex(-8.0f,-.75f));
	vertices.push_back(baseObject::vertex(-8.0f,-.7f));
	vertices.push_back(baseObject::vertex(-5.0f,-.7f));
	vertices.push_back(baseObject::vertex(-5.0f,-.75f));
	groundObjs.push_back(ground(orig, vertices));

	vertices.clear();
	//rectangle platform origin
	orig = baseObject::vertex(0.0f, -0.95f);
	//rectangle platform vertices
	vertices.push_back(baseObject::vertex(-10.0f,-1.0f));
	vertices.push_back(baseObject::vertex(-10.0f,-.9f));
	vertices.push_back(baseObject::vertex(10.0f,-.9f));
	vertices.push_back(baseObject::vertex(10.0f,-1.0f));
	groundObjs.push_back(ground(orig, vertices));

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

	//draw ground
	for( list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr )
	{
		glBegin(GL_POLYGON);
		glColor4f(objItr->color[0], objItr->color[1], objItr->color[2], objItr->color[3]);
		for( list<baseObject::vertex>::iterator vertItr = objItr->points.begin(); vertItr != objItr->points.end(); ++vertItr )
			glVertex2f(vertItr->x, vertItr->y);
		glEnd();
	}

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

	glutSwapBuffers();
}

void keyUp(unsigned char key, int x, int y)
{
	map<int, bool>::iterator currKey = keyMap.find(key);
	//if the released key is in the map unset pressed
	if( currKey != keyMap.end() )
	{
		currKey->second = false;
	}
}

//find the linear distance between two points
float pointDistance( const baseObject::vertex & one, const baseObject::vertex & two )
{
	return sqrt( pow(two.x - one.x, 2) + pow(two.y - one.y, 2) );
}

ground *getCurrentGround()
{
	//map to store distances to each ground object, keyed by distance for auto-sort on distance
	map<float, ground*> distances;

	//build map
	for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
		distances.insert( make_pair( pointDistance( player->origin, itr->origin ), &(*itr) ) );

	//if the player is not above the closest object, remove it
	while( !collision::above( *player, *(distances.begin()->second) ) )
	{
		distances.erase(distances.begin());
		//return NULL pointer if the map is now empty
		if( distances.empty() )
			return NULL;
	}
	//return pointer to closest ground object which player is above
	return distances.begin()->second;
}

//handle motion of player
void updatePlayerLocation( const long double & elapsed )
{
	//respond to input if needed
	if( multiplier != 0.0f )
		player->move( multiplier );

	//figure out which ground object the player is currently above
	ground *belowPlayer = getCurrentGround();
	//if no valid ground is found or player is no longer above current,
	//	let player fall
	if( belowPlayer == NULL || player->yMin > belowPlayer->yMax)
		player->bOnGround = false;

	//if the player is currently in the air, apply gravity
	if( !player->bOnGround )
	{
		//temporary copy of player to stop player
		actor *temp = new actor(*player);
		//apply gravity to copy to make sure they don't fall through world
		physics::applyGravity( temp, elapsed );
		//if no ground at all, player free falls
		if(belowPlayer == NULL)
			player=temp;
		//if next iter of motion still leaves player above ground, do it
		else if(collision::timeToCollision( *temp, *belowPlayer ) > 0)
		{
			player=temp;
			timeToImpact = collision::timeToCollision( *temp, *belowPlayer );
		}
		//otherwise, move player just enough to be on ground
		else
			physics::moveByTime( player, timeToImpact );
	}
}

void keyPress(unsigned char key, int x, int y)
{
	//set appropriate key to pressed
	if(key == 'W' || key == 'w')
	{
		map<int, bool>::iterator currKey = keyMap.find('w');
		currKey->second = true;
	}
	if(key == 'A' || key == 'a')
	{
		map<int, bool>::iterator currKey = keyMap.find('a');
		currKey->second = true;
	}
	if(key == 'D' || key == 'd')
	{
		map<int, bool>::iterator currKey = keyMap.find('d');
		currKey->second = true;
	}
	else if(key == 'S' || key == 's')
	{
		map<int, bool>::iterator currKey = keyMap.find('s');
		currKey->second = true;
	}
}

void idleFunction(void)
{
	//react based on which keys are pressed
	if (keyMap.find('a')->second == false && keyMap.find('d')->second == false && multiplier != 0)
	{
		//decay the multiplier if no keys are being pressed
		if( multiplier < .05 && multiplier > -.05 )
			multiplier = 0;
		else
			multiplier /= 1.2;
	}
	if (keyMap.find('a')->second == true)
	{
		//logic for ramping up move speed
		if( multiplier >= 0 )
			multiplier = -.1;
		else if( multiplier > -1.0 )
			multiplier *= 1.2;
		else
			multiplier = -1.0;
	}
	if (keyMap.find('d')->second == true)
	{
		//logic for ramping up move speed
		if( multiplier <= 0 )
			multiplier = .1;
		else if( multiplier < 1.0 )
			multiplier *= 1.2;
		else
			multiplier = 1.0;
	}
	if (keyMap.find('w')->second == true)
	{
		//if player is on the ground, allow them to jump
		if( player->bOnGround )
		{
			player->jump();
		}
	}

	//save old elapsed time
	prevElapsed = elapsed;
	//get new elapsed time
	elapsed = glutGet(GLUT_ELAPSED_TIME)/1000.0;
	updatePlayerLocation( elapsed - prevElapsed );

	//adjust viewpoint offset to follow player
	trans = -(player->origin.x);
	trans /= aspect;
	reshape(width, height);

	//update windows title
	string elapsedStr = "Elapsed: " + to_string(elapsed);
	glutSetWindowTitle( elapsedStr.c_str() );

	//call redrawing of elements
	redraw();
}

void initPlayer()
{
	//square orig
	baseObject::vertex orig = baseObject::vertex( 0.0f, 0.0f );
	//square vertices
	vertices.push_back(baseObject::vertex(.075f,-.075f));
	vertices.push_back(baseObject::vertex(.075f,.075f));
	vertices.push_back(baseObject::vertex(-.075f,.075f));
	vertices.push_back(baseObject::vertex(-.075f,-.075f));
	GLfloat color[4] = {0.0f, 1.0f, 0.0f, 1.0f};
	player = new actor(orig, vertices, color);
}

int main(int argc, char** argv)
{
	//initialize needed data
	initKeyMap();
	initPlayer();	
	makeObjects();

	//glut, glut, and more glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Platformer");
	glutDisplayFunc(disp);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyUp);
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
	glutIdleFunc(idleFunction);
	glutMainLoop();
	return 0;
}