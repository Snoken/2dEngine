#define _CRT_SECURE_NO_DEPRECATE
#include "soil\SOIL.h"

#include <string>
#include <map>
#include <iostream>
#include <math.h>
#include <random>
#include <time.h>

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
float aspect, trans = 0.0f, timeToImpact = 0.0f, mouseX, mouseY, drawWidth = 0.0f, drawHeight = 0.0f;
long double elapsed, prevElapsed;
double multiplier, frame = 0;
GLuint tSky, tSkyLower, tDirt, tPaused;
GLuint tCharStand[4];
GLuint tCharRun[8];
GLuint tCharJump[5];
GLuint tWalls[5];
bool drawMenu, bDrawOutline;
baseObject::vertex clickLoc, drawCenter;

#define SPACEBAR 32
#define ESC 27

GLuint loadTexture( const char * filename )
{
	/* load an image file directly as a new OpenGL texture */
	return SOIL_load_OGL_texture
		(
		filename,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS
		);
}

void initKeyMap()
{
	keyMap.insert( make_pair( 'w', false ));
	keyMap.insert( make_pair( 'a', false ));
	keyMap.insert( make_pair( 's', false ));
	keyMap.insert( make_pair( 'd', false ));
	keyMap.insert( make_pair( ESC, false )); //escape key
	keyMap.insert( make_pair( SPACEBAR, false )); //escape key
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

baseObject makeRectangle( baseObject::vertex origin, float width, float height, GLuint texture =  tWalls[rand() % 5])
{
	list<baseObject::vertex> rectVerts;
	rectVerts.push_back(baseObject::vertex(origin.x - width/2, origin.y - height/2));
	rectVerts.push_back(baseObject::vertex(origin.x - width/2, origin.y + height/2));
	rectVerts.push_back(baseObject::vertex(origin.x + width/2, origin.y + height/2));
	rectVerts.push_back(baseObject::vertex(origin.x + width/2, origin.y - height/2));
	return baseObject(origin, rectVerts, texture);
}

void makeObjects()
{
	groundObjs.push_back( baseObject( baseObject::vertex( 0.0f, -0.95f ), 200.0f, .1f, tDirt ) );
	groundObjs.push_back( baseObject( baseObject::vertex( 0.5f, -0.7f ), 0.5f, .075f, tWalls[rand() % 5] ) );
	groundObjs.push_back( baseObject( baseObject::vertex( 0.75f, -0.55f ), .5f, .075f, tWalls[rand() % 5] ) );
	groundObjs.push_back( baseObject( baseObject::vertex( 1.625f, -.775f ), .25f, .25f, tWalls[rand() % 5] ) );
}

void drawSky()
{
	glEnable( GL_TEXTURE_2D );
	glColor3ub(255, 255, 255);
	//big backdrop
	glBindTexture( GL_TEXTURE_2D, tSky );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f); glVertex2f(-aspect+player->origin.x,-1.0);
	glTexCoord2f(0.0f,1.0f); glVertex2f(-aspect+player->origin.x,1.0);
	glTexCoord2f(aspect/0.1f,1.0f); glVertex2f(aspect+player->origin.x,1.0);
	glTexCoord2f(aspect/0.1f,0.0f); glVertex2f(aspect+player->origin.x,-1.0);
	glEnd();

	//sky gradient
	glBindTexture( GL_TEXTURE_2D, tSkyLower );
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f); glVertex2f(-aspect+player->origin.x,-0.9f);
	glTexCoord2f(0.0f,1.0f); glVertex2f(-aspect+player->origin.x,-0.8f);
	glTexCoord2f(aspect/0.1f,1.0f); glVertex2f(aspect+player->origin.x,-0.8f);
	glTexCoord2f(aspect/0.1f,0.0f); glVertex2f(aspect+player->origin.x,-0.9f);
	glEnd();
	glDisable( GL_TEXTURE_2D );
}

void drawPlayer()
{
	GLfloat *currColor = player->color;
	glEnable(GL_BLEND);
	glEnable( GL_TEXTURE_2D );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	if( !player->bOnGround )
	{
		frame += .15;
		if( frame >= 5 )
			frame = 4;
		glBindTexture( GL_TEXTURE_2D, tCharJump[(int)floor(frame)] );
	}
	else if( multiplier == 0 )
	{
		frame += .05;
		if( frame >= 4 )
			frame = 0;
		glBindTexture( GL_TEXTURE_2D, tCharStand[(int)floor(frame)] );
	}
	else
	{
		frame += .2;
		if( frame >= 8 )
			frame = 0;
		glBindTexture( GL_TEXTURE_2D, tCharRun[(int)floor(frame)] );
	}

	glBegin(GL_QUADS);
	if( multiplier >= 0)
	{
		glTexCoord2f(0.0f,0.0f); glVertex2f(player->origin.x - .1f, player->origin.y - .1f);
		glTexCoord2f(0.0f,1.0f); glVertex2f(player->origin.x - .1f, player->origin.y + .1f);
		glTexCoord2f(1.0f,1.0f); glVertex2f(player->origin.x + .1f, player->origin.y + .1f);
		glTexCoord2f(1.0f,0.0f); glVertex2f(player->origin.x + .1f, player->origin.y - .1f);
	}
	else
	{
		glTexCoord2f(1.0f,0.0f); glVertex2f(player->origin.x - .1f, player->origin.y - .1f);
		glTexCoord2f(1.0f,1.0f); glVertex2f(player->origin.x - .1f, player->origin.y + .1f);
		glTexCoord2f(0.0f,1.0f); glVertex2f(player->origin.x + .1f, player->origin.y + .1f);
		glTexCoord2f(0.0f,0.0f); glVertex2f(player->origin.x + .1f, player->origin.y - .1f);
	}
	glEnd();
	glDisable( GL_TEXTURE_2D );
}
void redraw()
{	
	glClear(GL_COLOR_BUFFER_BIT);
	drawSky();

	//draw background objects
	glEnable( GL_TEXTURE_2D );
	for( list<baseObject>::iterator objItr = backgroundObjs.begin(); objItr != backgroundObjs.end(); ++objItr )
	{
		glBegin(GL_POLYGON);
		glBindTexture( GL_TEXTURE_2D, objItr->texture );
		if( objItr->bSelected )
			glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		else
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f,0.0f); glVertex2f(objItr->xMin,objItr->xMin);
		glTexCoord2f(0.0f,1.0f); glVertex2f(objItr->xMin,objItr->yMax);
		glTexCoord2f(1.0f,1.0f); glVertex2f(objItr->xMax,objItr->xMax);
		glTexCoord2f(1.0f,0.0f); glVertex2f(objItr->xMin,objItr->xMin);
		glEnd();
	}
	glDisable( GL_TEXTURE_2D );

	//draw ground
	for( list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr )
	{
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, objItr->texture );
		glBegin(GL_POLYGON);
		if( objItr->bSelected )
			glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		else
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if( objItr->width > objItr->height )
		{
			glTexCoord2f(0.0f,0.0f); glVertex2f(objItr->xMin,objItr->yMin);
			glTexCoord2f(0.0f,1.0f); glVertex2f(objItr->xMin,objItr->yMax);
			glTexCoord2f(objItr->width/objItr->height,1.0f); glVertex2f(objItr->xMax,objItr->yMax);
			glTexCoord2f(objItr->width/objItr->height,0.0f); glVertex2f(objItr->xMax,objItr->yMin);
		}
		else
		{
			glTexCoord2f(0.0f,0.0f); glVertex2f(objItr->xMin,objItr->yMin);
			glTexCoord2f(0.0f,objItr->height/objItr->width); glVertex2f(objItr->xMin,objItr->yMax);
			glTexCoord2f(1.0f,objItr->height/objItr->width); glVertex2f(objItr->xMax,objItr->yMax);
			glTexCoord2f(1.0f,0.0f); glVertex2f(objItr->xMax,objItr->yMin);
		}
		glEnd();
		glDisable( GL_TEXTURE_2D );
	}

	drawPlayer();

	//draw foreground objs
	for( list<prop>::iterator objItr = foregroundObjs.begin(); objItr != foregroundObjs.end(); ++objItr )
	{
		glBegin(GL_POLYGON);
		glColor4f(objItr->color[0], objItr->color[1], objItr->color[2], objItr->color[3]);
		for( list<baseObject::vertex>::iterator vertItr = objItr->points.begin(); vertItr != objItr->points.end(); ++vertItr )
			glVertex2f(vertItr->x, vertItr->y);
		glEnd();
	}

	if( bDrawOutline )
	{
		if( abs(drawHeight) != 0.0f && abs(drawWidth) != 0.0f )
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glBegin(GL_LINES);

			glVertex2f( clickLoc.x, clickLoc.y );
			glVertex2f( clickLoc.x, clickLoc.y + drawHeight );

			glVertex2f( clickLoc.x, clickLoc.y + drawHeight );
			glVertex2f( clickLoc.x + drawWidth, clickLoc.y + drawHeight );

			glVertex2f( clickLoc.x + drawWidth, clickLoc.y + drawHeight );
			glVertex2f( clickLoc.x + drawWidth, clickLoc.y );

			glVertex2f( clickLoc.x + drawWidth, clickLoc.y );
			glVertex2f( clickLoc.x, clickLoc.y );

			//draw subdivision lines
			if( abs(drawWidth) > abs(drawHeight) )
			{
				float endX = clickLoc.x + drawWidth;
				float incDist;
				drawWidth >= 0 ? incDist = abs(drawHeight) : incDist = -abs(drawHeight);
				while( abs(incDist) <= abs(drawWidth) )
				{
					glVertex2f( clickLoc.x + incDist, clickLoc.y + drawHeight );
					glVertex2f( clickLoc.x + incDist, clickLoc.y );
					if( drawWidth >= 0 )
						incDist += abs(drawHeight);
					else
						incDist -= abs(drawHeight);
				}
			}
			else
			{
				float endY = clickLoc.y + drawHeight;
				float incDist;
				drawHeight >= 0 ? incDist = abs(drawWidth) : incDist = -abs(drawWidth);
				while( abs(incDist) <= abs(drawHeight) )
				{
					glVertex2f( clickLoc.x + drawWidth, clickLoc.y + incDist);
					glVertex2f( clickLoc.x, clickLoc.y + incDist);
					if( drawHeight >= 0 )
						incDist += abs(drawWidth);
					else
						incDist -= abs(drawWidth);
				}
			}
			glEnd();
		}
	}
	
	if( drawMenu )
	{
		glDisable( GL_TEXTURE_2D );
		glColor4f(0.0f, 0.0f, 0.0f, 0.9f);
		glBegin(GL_QUADS);
		glVertex2f(-aspect+player->origin.x,-1.0);
		glVertex2f(-aspect+player->origin.x,1.0);
		glVertex2f(aspect+player->origin.x,1.0);
		glVertex2f(aspect+player->origin.x,-1.0);
		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, tPaused );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex2f( (-aspect + 0.1f),0.65f);
		glTexCoord2f(0.0f,1.0f); glVertex2f( (-aspect + 0.1f),0.9f);
		glTexCoord2f(1.0f,1.0f); glVertex2f( (-aspect + 0.1f) + 1.0f,0.9f);
		glTexCoord2f(1.0f,0.0f); glVertex2f( (-aspect + 0.1f) + 1.0f,0.65f);
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

bool facing( actor one, baseObject two, bool right )
{
	if( two.xMin >= one.xMax && right )
		return true;
	else if( two.xMax <= one.xMin && !right )
		return true;
	return false;
}

ground *getNearestWall()
{
	//map to store distances to each wall, keyed by distance for auto-sort on distance
	map<float, ground*> distances; 
	bool right;

	//build map
	if( multiplier >= 0)
	{
		for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
			distances.insert( make_pair( abs(itr->xMin-player->xMax), &(*itr) ) );
		right = true;
	}
	else
	{
		for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
			distances.insert( make_pair( abs(itr->xMax-player->xMin), &(*itr) ) );
		right = false;
	}
	//if the player is not above the closest object, remove it
	while( !facing( *player, *(distances.begin()->second), right ) )
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
	if( distances.empty() )
		return NULL;
	return distances.begin()->second;
}

ground *checkForWall( const float & maxDistance, float & wallDistance )
{
	ground *closest = NULL;
	float lowestDist = 9999999.9f;
	for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
	{
		float gapSize = 0.0f;
		if( multiplier > 0 )
			gapSize = itr->xMin - player->xMax;
		else
			gapSize = player->xMin - itr->xMax;

		//the negative value is to add a bit of tolerance so the player doesn't run through walls
		if( gapSize > -player->getRunSpeed()/10 && gapSize < lowestDist && gapSize < maxDistance && 
			collision::nextTo( *player, *itr ) && !collision::above( *player, *itr ) )
		{
			lowestDist = gapSize;
			closest = &(*itr);
		}
	}
	wallDistance = lowestDist;
	return closest;
}

//handle motion of player
void updatePlayerLocation( const long double & elapsed )
{
	if( !drawMenu )
	{
		//figure out which ground object the player is currently above
		actor *temp;
		ground *belowPlayer = getCurrentGround();
		//if no valid ground is found or player is no longer above current,
		//	let player fall
		if( player->yMin > belowPlayer->yMax)
			player->bOnGround = false;

		//if the player is currently in the air, apply gravity
		if( !player->bOnGround )
		{
			temp = new actor(*player);
			//apply gravity to copy to make sure they don't fall through world
			physics::applyGravity( temp, elapsed );
			//if next iter of motion still leaves player above ground, do it			
			if(collision::timeToCollisionY( *temp, *belowPlayer ) > 0)
			{
				player=temp;
				timeToImpact = collision::timeToCollisionY( *temp, *belowPlayer );
			}
			//otherwise, move player just enough to be on ground
			else
			{
				physics::moveByTimeY( player, timeToImpact );
			}
		}

		if( multiplier != 0.0f )
		{
			temp = new actor(*player);
			//check if there is a wall within 1.0f
			float maxDistance = 1.0f, wallDistance;
			ground *nearby = checkForWall( maxDistance, wallDistance );
			if( nearby != NULL )
			{
				nearby->color[1] = 0.0f;
				nearby->color[2] = 0.0f;
			}
			if( nearby == NULL )
			{
				player->move( multiplier );
			}
			//try moving
			else if( wallDistance > .01 )
			{
				temp->move( multiplier );
				//if we moved too far don't use temp, move player by needed distance, also set mult to 0 since a collision happened
				if( collision::areColliding( *temp, *nearby ) )
				{
					player->moveX( wallDistance );
					multiplier = 0.0f;
				}
				else
					player = temp;
			}
		}
	}
}

void keyPress(unsigned char key, int x, int y)
{
	//set appropriate key to pressed
	if (keyMap.find(key) != keyMap.end())
	{
		if( key == ESC ) //escape key
			drawMenu = !drawMenu;
		map<int, bool>::iterator currKey = keyMap.find( key );
		currKey->second = true;
	}
}

void idleFunction(void)
{
	//react based on which keys are pressed
	if( !drawMenu )
	{
		if (keyMap.find('a')->second == false && keyMap.find('d')->second == false && multiplier != 0)
		{
			//decay the multiplier if no keys are being pressed
			if( multiplier < .05 && multiplier > -.05 )
			{
				frame = 0;
				multiplier = 0;
			}
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
		if (keyMap.find('w')->second == true || keyMap.find(SPACEBAR)->second == true)
		{
			//if player is on the ground, allow them to jump
			if( player->bOnGround )
			{
				frame = 0;
				player->jump();
			}
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
	player = new actor(makeRectangle( baseObject::vertex( 0.0f, 0.0f ), .2f*(2.0f/3.0f), .2f));
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
	mouseX += player->origin.x;
	y -= height/2;
	y *= -1;
	mouseY = (float)y/(width/2);
	mouseY *= aspect;
}

void drawOutline(int x, int y)
{
	passiveMouse( x, y );
	bDrawOutline = true;
	drawWidth = -(clickLoc.x - mouseX);
	drawCenter.x = clickLoc.x - (clickLoc.x - mouseX)/2;

	drawHeight = -(clickLoc.y - mouseY);
	drawCenter.y = clickLoc.y - (clickLoc.y - mouseY)/2;
}

baseObject *checkSelected( baseObject::vertex loc )
{
	for( list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr )
	{
		if( collision::inObject( loc, *objItr ) )
			return &(*objItr);
	}
	return NULL;
}

void mouse(int btn, int state, int x, int y)
{
	baseObject::vertex center;
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN) 
	{
		clickLoc.x = mouseX;
		clickLoc.y = mouseY;
		baseObject *selected = checkSelected( clickLoc );
		if( selected != NULL )
			selected->bSelected = !selected->bSelected;
	}
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_UP) 
	{
		bDrawOutline = false;
		float distanceToMult;

		if( abs(drawWidth) > abs(drawHeight) )
		{
			//if the width isn't a multiple of the height
			if( fmod(drawWidth/drawHeight, 1.0f) != 0.0f )
			{
				distanceToMult = fmod( drawWidth, drawHeight );
				drawWidth -= distanceToMult;
			}
		}
		else
		{
			//if the height isn't a multiple of the width
			if( fmod(drawHeight/drawWidth, 1.0f) != 0.0f )
			{
				distanceToMult = fmod( drawHeight, drawWidth );
				drawHeight -= distanceToMult;
			}
		}
		//prevent accidental creating
		if( abs(drawWidth) > .025 && abs(drawHeight) > .025 )
			groundObjs.push_back( makeRectangle( drawCenter, abs(drawWidth), abs(drawHeight) ) );
	}
}

void loadTextures()
{
	srand( (unsigned)time(NULL) );

	//load textures
	tDirt = loadTexture("../Assets/Textures/dirt.jpg");
	tSky = loadTexture("../Assets/Textures/sky.jpg");
	tSkyLower = loadTexture("../Assets/Textures/skyLower.jpg");
	tPaused = loadTexture("../Assets/Textures/paused.png");

	for( int i = 1; i <= 4; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/character/stand/";
		location.append(num);
		location.append(".png");
		tCharStand[i-1] = loadTexture(location.c_str());
	}
	for( int i = 1; i <= 8; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/character/walk/";
		location.append(num);
		location.append(".png");
		tCharRun[i-1] = loadTexture(location.c_str());
	}
	for( int i = 1; i <= 5; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/character/jump/";
		location.append(num);
		location.append(".png");
		tCharJump[i-1] = loadTexture(location.c_str());
	}
	for( int i = 1; i <= 5; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/walls/";
		location.append(num);
		location.append(".png");
		tWalls[i-1] = loadTexture(location.c_str());
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
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc( passiveMouse );
	glutMotionFunc( drawOutline );
	glutIdleFunc(idleFunction);

	loadTextures();

	//initialize needed data
	initKeyMap();
	initPlayer();	
	makeObjects();

	glutMainLoop();
	return 0;
}