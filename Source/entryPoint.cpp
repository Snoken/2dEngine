#define _CRT_SECURE_NO_DEPRECATE
#include "soil\SOIL.h"

#include <string>
#include <map>
#include <iostream>

#include "actor.h"
#include "prop.h"
#include "physics.h"
#include "collision.h"
#include "ground.h"

using namespace std;

//global containers
list<baseObject> backgroundObjs;
list<prop> foregroundObjs;
list<ground> groundObjs;
list<baseObject::vertex> vertices;
map<int, bool> keyMap;

//global vars
int width(1600), height(900);
actor *player = NULL;
float aspect, trans = 0.0f, timeToImpact = 0.0f;
long double elapsed, prevElapsed;
double multiplier;
GLuint tex_2d;

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

baseObject makeRectangle( baseObject::vertex origin, float width, float height, GLfloat color[4] )
{
	list<baseObject::vertex> rectVerts;
	rectVerts.push_back(baseObject::vertex(origin.x - width/2, origin.y - height/2));
	rectVerts.push_back(baseObject::vertex(origin.x - width/2, origin.y + height/2));
	rectVerts.push_back(baseObject::vertex(origin.x + width/2, origin.y + height/2));
	rectVerts.push_back(baseObject::vertex(origin.x + width/2, origin.y - height/2));
	return baseObject(origin, rectVerts, color);
}

baseObject makeRectangle( baseObject::vertex origin, float width, float height )
{
	GLfloat color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	return makeRectangle(origin, width, height, color);
}

void makeObjects()
{
	groundObjs.push_back( makeRectangle( baseObject::vertex( 0.0f, -0.95f ), 20.0f, .1f ) );
	groundObjs.push_back( makeRectangle( baseObject::vertex( 0.5f, -0.7f ), .5f, .05f ) );
	groundObjs.push_back( makeRectangle( baseObject::vertex( 0.75f, -0.55f ), .5f, .05f ) );
	groundObjs.push_back( makeRectangle( baseObject::vertex( 1.625f, -.775f ), .25f, .25f ) );

	//GLfloat color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	//foregroundObjs.push_back( prop(makeRectangle( baseObject::vertex( 1.625f, -.775f ), .25f, .25f, color ), true) );
}

void redraw()
{	
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, tex_2d );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin( GL_QUADS );
	glTexCoord2f(0.0,0.0); glVertex2f(-aspect+player->origin.x,-1.0);
	glTexCoord2f(0.0,1.0); glVertex2f(-aspect+player->origin.x,1.0);
	glTexCoord2f(1.0,1.0); glVertex2f(aspect+player->origin.x,1.0);
	glTexCoord2f(1.0,0.0); glVertex2f(aspect+player->origin.x,-1.0);
	glEnd();

	glBindTexture( GL_TEXTURE_2D, 0 );

	//draw ground
	for( list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr )
	{
		glBegin(GL_POLYGON);
		glColor4f(objItr->color[0], objItr->color[1], objItr->color[2], objItr->color[3]);
		for( list<baseObject::vertex>::iterator vertItr = objItr->points.begin(); vertItr != objItr->points.end(); ++vertItr )
			glVertex2f(vertItr->x, vertItr->y);
		glEnd();
	}

	//draw background backgroundObjs
	for( list<baseObject>::iterator objItr = backgroundObjs.begin(); objItr != backgroundObjs.end(); ++objItr )
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

	//draw foreground backgroundObjs
	for( list<prop>::iterator objItr = foregroundObjs.begin(); objItr != foregroundObjs.end(); ++objItr )
	{
		glBegin(GL_POLYGON);
		glColor4f(objItr->color[0], objItr->color[1], objItr->color[2], objItr->color[3]);
		for( list<baseObject::vertex>::iterator vertItr = objItr->points.begin(); vertItr != objItr->points.end(); ++vertItr )
			glVertex2f(vertItr->x, vertItr->y);
		glEnd();
	}

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

ground *getNearestWall()
{
	//map to store distances to each wall, keyed by distance for auto-sort on distance
	map<float, ground*> distances; 

	//build map
	if( multiplier > 0)
	{
		for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
			distances.insert( make_pair( abs(itr->xMin-player->xMax), &(*itr) ) );
	}
	else
	{
		for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
			distances.insert( make_pair( abs(itr->xMax-player->xMin), &(*itr) ) );
	}
	//if the player is not above the closest object, remove it
	while( !collision::nextTo( *player, *(distances.begin()->second) ) )
	{
		distances.erase(distances.begin());
		//return NULL pointer if the map is now empty
		if( distances.empty() )
			return NULL;
	}
	//return pointer to closest ground object which player is above
	return distances.begin()->second;
}

ground *getCurrentGround()
{
	//map to store max height for each ground object, keyed by height for auto-sort on distance
	map<float, ground*, greater<float>> distances; 

	//build map
	for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
		distances.insert( make_pair( itr->yMax, &(*itr) ) );

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
	actor *temp = new actor(*player);
	if( multiplier != 0.0f )
	{
		ground *closestWall = getNearestWall();

		bool colliding = false;
		if( multiplier > 0 && closestWall != NULL )
		{
			if( abs(closestWall->xMin - player->xMax) < .01 )
				colliding = true;
		}
		else if( multiplier < 0 && closestWall != NULL )
		{
			if( abs(closestWall->xMax - player->xMin) < .01 )
				colliding = true;
		}
		if( colliding )
			return;
		temp->move( multiplier );
		if( closestWall == NULL )
			player = temp;
		else if( collision::areColliding( *temp, *closestWall ) )
		{
			float timeToImpact;
			if( multiplier > 0 )
				timeToImpact = abs(closestWall->xMin - player->xMax) / (player->getRunSpeed()*(float)multiplier);
			else
				timeToImpact = abs(closestWall->xMax - player->xMin) / (player->getRunSpeed()*(float)-multiplier);
			physics::moveByTimeX( player, timeToImpact );
			multiplier = 0.0f;
		}
		else
			player = temp;
	}

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
		temp = new actor(*player);
		//apply gravity to copy to make sure they don't fall through world
		physics::applyGravity( temp, elapsed );
		//if no ground at all, player free falls
		if(belowPlayer == NULL)
			player=temp;
		//if next iter of motion still leaves player above ground, do it
		else if(collision::timeToCollisionY( *temp, *belowPlayer ) > 0)
		{
			player=temp;
			timeToImpact = collision::timeToCollisionY( *temp, *belowPlayer );
		}
		//otherwise, move player just enough to be on ground
		else
			physics::moveByTimeY( player, timeToImpact );
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
	GLfloat color[4] = {0.0f, 1.0f, 0.0f, 1.0f};
	player = new actor(makeRectangle( baseObject::vertex( 0.0f, 0.0f ), 0.05f, .15f, color ));
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

	/* load an image file directly as a new OpenGL texture */
	tex_2d = SOIL_load_OGL_texture
		(
		"../Assets/Textures/texture.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS
		);

	/* check for an error during the load process */
	if( 0 == tex_2d )
	{
		cout << "SOIL loading error: " << SOIL_last_result() << endl;
	}

	glutMainLoop();
	return 0;
}