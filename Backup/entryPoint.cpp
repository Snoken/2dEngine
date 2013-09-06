#define _CRT_SECURE_NO_DEPRECATE
#include "soil\SOIL.h"
#include <Windows.h>

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

using namespace std;

//global containers
list<baseObject> backgroundObjs;
list<prop> foregroundObjs;
list<ground> groundObjs;
list<baseObject::vertex> vertices;
list<actor> menuItems; //actors so they can be moved
map<int, bool> keyMap;
bool running = false, bSliding = false, bEditing = false;

//global vars
int width(1600), height(900);
actor *player = NULL;
float aspect = (float)width/height, trans = 0.0f, mouseX, 
	mouseY, drawWidth = 0.0f, drawHeight = 0.0f;
long double elapsed, prevElapsed;
GLuint tSky, tSkyLower, tDirt, tPaused, tSave, tLoad, tSlide;
GLuint tCharStand[4];
GLuint tCharRun[8];
GLuint tCharJump[5];
GLuint tWalls[5];
float wallDistance = 0.0f;
bool bDrawMenu, bDrawOutline;
baseObject::vertex clickLoc, drawCenter;

#define SPACEBAR 32
#define ESC 27

void drawBitmapText(char *string,float x,float y,float z) 
{  
	char *c;
	glRasterPos3f(x, y,z);
	for (c=string; *c != '\0'; c++) 
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
}

void drawStrokeText(char*string,int x,int y,int z)
{
	char *c;
	glPushMatrix();
	glTranslatef(x, y+8,z);
	glScalef(.09f,-0.08f,z);
	for (c=string; *c != '\0'; c++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	glPopMatrix();
}

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

void getFileWin( OPENFILENAME & ofn )
{
	char szFile[260];       // buffer for file name
	HWND hwnd = NULL;              // owner window

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Level Files (*.lvl)\0*.lvl\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST;

	// Display the Open dialog box. 
	GetOpenFileName(&ofn);
}

void initKeyMap()
{
	//add all keys to map
	keyMap.insert( make_pair( 'w', false ));
	keyMap.insert( make_pair( 'a', false ));
	keyMap.insert( make_pair( 's', false ));
	keyMap.insert( make_pair( 'd', false ));
	keyMap.insert( make_pair( 'o', false )); //toggle editor
	keyMap.insert( make_pair( ESC, false )); //escape key
	keyMap.insert( make_pair( SPACEBAR, false )); //escape key
}

//glut needs this even if it's empty for some reason
void disp(){}

/* Handler for window re-size event. Called back when the window first appears and
whenever the window is re-sized with its new width and height */
void reshape(GLsizei newWidth, GLsizei newHeigth) {  // GLsizei for non-negative integer
	height = newHeigth;
	width = newWidth;
	// Compute aspect ratio of the new window
	if (height == 0) height = 1; // To prevent divide by 0
	aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping area to match the viewport
	glMatrixMode(GL_MODELVIEW); //To operate on the Projection matrix
	glLoadIdentity(); //Reset the projection matrix
	glTranslatef(trans, 0.0f, 0.0f); //moveByTimeX camera to player location
	glPopMatrix();
	if (width >= height)
		// aspect >= 1, set the height from -1 to 1, with larger width
		gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
	else
		// aspect < 1, set the width to -1 to 1, with larger height
		gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
}

void initObjects()
{
	//make basic objects for start
	groundObjs.push_back( baseObject( 
		baseObject::vertex( 0.0f, -0.95f ), 200.0f, .1f, tDirt ) );

	list<baseObject::vertex> verts;
	verts.push_back( baseObject::vertex( .1f, -.9f ) );
	verts.push_back( baseObject::vertex( .1f, -.7f ) );
	verts.push_back( baseObject::vertex( -.1f, -.9f ) );
	groundObjs.push_back( baseObject(
		baseObject::vertex( 0.0f, -0.8f ), verts ) );

	groundObjs.push_back( baseObject( 
		baseObject::vertex( 1.625f, -.775f ), .25f, .25f, tWalls[rand() % 5] ) );
}

void initMenu()
{
	//background overlay
	GLfloat color[4] = { 0.0f, 0.0f, 0.0f, 0.9f };
	menuItems.push_back( baseObject( 
		baseObject::vertex( 0.0f, 0.0f ), aspect*2, 2.0f, color ) );
	//pause text
	menuItems.push_back( baseObject( 
		baseObject::vertex( (-aspect + 0.1f) + .5f, 0.775f ), 1.0f, .25f, tPaused ) );
	//save text
	menuItems.push_back( baseObject( 
		baseObject::vertex( (-aspect + 0.25f) + .25f, 0.525f ), .5f, .25f, tSave ) );
	//load text
	menuItems.push_back( baseObject( 
		baseObject::vertex( (-aspect + 0.25f) + .25f, 0.325f ), .5f, .25f, tLoad ) );
}

void drawMenu()
{
	list<actor>::iterator itr = menuItems.begin();
	glDisable( GL_TEXTURE_2D );
	glColor4f(0.0f, 0.0f, 0.0f, 0.9f);
	//faded black overlay
	glBegin(GL_QUADS);
	glVertex2f(itr->xMin+player->origin.x,itr->yMin);
	glVertex2f(itr->xMin+player->origin.x,itr->yMax);
	glVertex2f(itr->xMax+player->origin.x,itr->yMax);
	glVertex2f(itr->xMax+player->origin.x,itr->yMin);
	glEnd();
	++itr;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable( GL_TEXTURE_2D );
	for( ; itr!=menuItems.end() ; ++itr )
	{
		//draw menu objects
		glBindTexture( GL_TEXTURE_2D, itr->texture );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex2f(itr->xMin+player->origin.x,itr->yMin);
		glTexCoord2f(0.0f,1.0f); glVertex2f(itr->xMin+player->origin.x,itr->yMax);
		glTexCoord2f(1.0f,1.0f); glVertex2f(itr->xMax+player->origin.x,itr->yMax);
		glTexCoord2f(1.0f,0.0f); glVertex2f(itr->xMax+player->origin.x,itr->yMin);
		glEnd();
	}
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
	//bind appropriate texture to player, increment m_frame as appropriate based 
	//		if player is in air on player state
	if( !bDrawMenu && !player->m_bOnGround )
		player->airFrameUpdate(bSliding);
	if( !player->m_bOnGround )
	{
		if( bSliding )
			glBindTexture( GL_TEXTURE_2D, tSlide );
		else
			glBindTexture( GL_TEXTURE_2D, tCharJump[(int)floor(player->m_frame)] );
	}
	else
	{
		if( !bDrawMenu )
			player->groundFrameUpdate( running );
		if( running )
			glBindTexture( GL_TEXTURE_2D, tCharRun[(int)floor(player->m_frame)] );
		else
			glBindTexture( GL_TEXTURE_2D, tCharStand[(int)floor(player->m_frame)] );
	}

	glBegin(GL_QUADS);
	//draw textures appropriately based on movement direction
	if( player->m_bFacingRight )
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

	//draw m_health bar
	float width = ((player->xMax - .01f) - (player->xMin + .01f))*player->getHealth()/100.0f;
	if( width < 0.0f )
		width = 0.0f;

	glBegin( GL_QUADS );
	glColor4f(1.0f, 0.0f,0.0f,1.0f);
	glVertex2f( player->xMin + .01f, player->yMax +.01f );
	glVertex2f( player->xMin + .01f, player->yMax + .02f );
	glVertex2f( player->xMin + .01f + width, player->yMax +.02f );
	glVertex2f( player->xMin + .01f + width, player->yMax + .01f );
	glEnd();

	glBegin( GL_LINES );
	glColor4f(0.0f, 0.0f,0.0f,1.0f);
	glVertex2f( player->xMin + .01f, player->yMax +.01f );
	glVertex2f( player->xMin + .01f, player->yMax + .02f );

	glVertex2f( player->xMin + .01f, player->yMax + .02f );
	glVertex2f( player->xMax - .01f, player->yMax +.02f );

	glVertex2f( player->xMax - .01f, player->yMax +.02f );
	glVertex2f( player->xMax - .01f, player->yMax + .01f );

	glVertex2f( player->xMax - .01f, player->yMax + .01f );
	glVertex2f( player->xMin + .01f, player->yMax +.01f );
	glEnd();
}

void drawGrid()
{
	//draw the grid for guides
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
	for( float x = 0.0f; x < 100.0f; x += .05f )
	{
		glVertex2f( x, 1.0f );
		glVertex2f( x, -1.0f );
	}
	for( float x = -0.05f; x > -100.0f; x -= .05f )
	{
		glVertex2f( x, 1.0f );
		glVertex2f( x, -1.0f );
	}

	for( float y = 0.0f; y < 1; y += .05f )
	{
		glVertex2f( -100, y );
		glVertex2f( 100, y );
	}
	for( float y = -0.05f; y > -1; y -= .05f )
	{
		glVertex2f( -100, y );
		glVertex2f( 100, y );
	}
	glEnd();
}

void drawOutline()
{
	//only draw if there's an outline to draw
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

void drawBackground()
{
	for( list<baseObject>::iterator objItr = backgroundObjs.begin(); objItr != backgroundObjs.end(); ++objItr )
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable( GL_TEXTURE_2D );
		glBegin(GL_POLYGON);
		glBindTexture( GL_TEXTURE_2D, objItr->texture );
		glTexCoord2f(0.0f,0.0f); glVertex2f(objItr->xMin,objItr->xMin);
		glTexCoord2f(0.0f,1.0f); glVertex2f(objItr->xMin,objItr->yMax);
		glTexCoord2f(1.0f,1.0f); glVertex2f(objItr->xMax,objItr->xMax);
		glTexCoord2f(1.0f,0.0f); glVertex2f(objItr->xMin,objItr->xMin);
		glEnd();
		glDisable( GL_TEXTURE_2D );

		if( objItr->bSelected && bEditing )
		{
			glBegin(GL_POLYGON);
			glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
			glVertex2f(objItr->xMin,objItr->xMin);
			glVertex2f(objItr->xMin,objItr->yMax);
			glVertex2f(objItr->xMax,objItr->xMax);
			glVertex2f(objItr->xMin,objItr->xMin);
		}
	}
}

void drawGround()
{
	for( list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr )
	{
		glEnable( GL_TEXTURE_2D );
		if( objItr->texture == 0 )
			objItr->texture = tWalls[rand() % 5];

		glBindTexture( GL_TEXTURE_2D, objItr->texture );
		glBegin(GL_POLYGON);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if( objItr->points.size() == 4 )
		{
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
		}
		else if( objItr->points.size() == 3 )
		{
			list<baseObject::vertex>::iterator itr = objItr->points.begin();
			glTexCoord2f(0.0f,0.0f); glVertex2f(itr->x,itr->y);
			++itr;
			glTexCoord2f(0.0f,1.0f); glVertex2f(itr->x,itr->y);
			++itr;
			glTexCoord2f(1.0f,1.0f); glVertex2f(itr->x,itr->y);
		}
		glEnd();
		glDisable( GL_TEXTURE_2D );

		if( objItr->bSelected && bEditing )
		{
			glBegin(GL_POLYGON);
			glColor4f(1.0f, 0.0f, 0.0f, 0.25f);
			if( objItr->points.size() == 3 )
			{
				list<baseObject::vertex>::iterator itr = objItr->points.begin();
				glVertex2f(itr->x,itr->y);
				++itr;
				glVertex2f(itr->x,itr->y);
				++itr;
				glVertex2f(itr->x,itr->y);
			}
			else
			{
				glVertex2f(objItr->xMin,objItr->yMin);
				glVertex2f(objItr->xMin,objItr->yMax);
				glVertex2f(objItr->xMax,objItr->yMax);
				glVertex2f(objItr->xMax,objItr->yMin);
			}
			glEnd();
		}
	}
}

void drawForeground()
{
	for( list<prop>::iterator objItr = foregroundObjs.begin(); objItr != foregroundObjs.end(); ++objItr )
	{
		glBegin(GL_POLYGON);
		glColor4f(objItr->color[0], objItr->color[1], objItr->color[2], objItr->color[3]);
		for( list<baseObject::vertex>::iterator vertItr = objItr->points.begin(); vertItr != objItr->points.end(); ++vertItr )
			glVertex2f(vertItr->x, vertItr->y);
		glEnd();
	}
}

void drawCursor()
{
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glBegin(GL_LINES);
	glVertex2f(mouseX - .025, mouseY);
	glVertex2f(mouseX + .025, mouseY);

	glVertex2f(mouseX, mouseY - .025);
	glVertex2f(mouseX, mouseY + .025);
	glEnd();
}

void redraw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//each draw call is essentially a layer, the first call is the furthest back
	drawSky();
	drawBackground();
	if( bEditing )
		drawGrid();
	drawGround();
	drawPlayer();
	drawForeground();
	if( bDrawOutline )
		drawOutline();
	if( bDrawMenu )
		drawMenu();
	drawCursor();

	//drawBitmapText("Osama Hosam's OpenGL Tutorials",200,200,0);
	//glutBitmapString(GLUT_BITMAP_HELVETICA_10, "hello world");
	
	glutSwapBuffers();
}

void keyUp(unsigned char key, int x, int y)
{
	map<int, bool>::iterator currKey = keyMap.find(key);
	//if the released key is in the map unset pressed
	if( currKey != keyMap.end() )
		currKey->second = false;
}

//find the linear distance between two points
float pointDistance( const baseObject::vertex & one, const baseObject::vertex & two )
{
	return sqrt( pow(two.x - one.x, 2) + pow(two.y - one.y, 2) );
}

ground *getCurrentGround( actor one )
{
	//figure out which ground is below given actor
	float lowestDif = 999.99f;
	ground *belowPlayer = NULL;
	for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
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
	for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
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
	for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
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
		ground *belowPlayer = getCurrentGround( *player );
		ground *abovePlayer = getCurrentCeiling( *player );
		float maxDistance = 0.5f;
		list<ground> nearby;
		getNearbyWalls( *player, maxDistance, nearby );
		player->updateLocation( elapsed, belowPlayer, abovePlayer, &nearby, &keyMap );
	}
}

void keyPress(unsigned char key, int x, int y)
{
	//set appropriate key to pressed
	if (keyMap.find(key) != keyMap.end())
	{
		if( key == ESC ) //escape key
			bDrawMenu = !bDrawMenu;
		else if( key == 'o' )
			bEditing = !bEditing;
		map<int, bool>::iterator currKey = keyMap.find( key );
		currKey->second = true;
	}
}

void idleFunction(void)
{
	//react based on which keys are pressed
	if( !bDrawMenu )
	{
		if (keyMap.find('a')->second == false && keyMap.find('d')->second == false)
			//decay the multiplier if no keys are being pressed
			player->decayMult();
		if (keyMap.find('a')->second == true)
		{
			if(!bSliding)
				player->m_bFacingRight = false;
			player->updateMult();
		}
		else if (keyMap.find('d')->second == true)
		{
			if(!bSliding)
				player->m_bFacingRight = true;
			player->updateMult();
		}
		if (keyMap.find('w')->second == true || keyMap.find(SPACEBAR)->second == true)
			//if player is on the ground, allow them to jump
			player->jump();
	}

	//save old elapsed time
	if (elapsed == 0)
		prevElapsed = glutGet(GLUT_ELAPSED_TIME)/1000.0;
	else
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
	player = new actor( actor( baseObject::vertex( 0.0f, 0.0f ), .2f*(2.0f/3.0f), .195f));
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

void updateOutline(int x, int y)
{
	passiveMouse( x, y );
	if( bEditing )
	{
		bDrawOutline = true;
		mouseX = floor(mouseX * 100.0f)/100.0f;
		float proximity = fmod( mouseX, .05f );
		if( abs(proximity) < .025f )
			mouseX -= proximity;
		else
			mouseX >= 0 ? mouseX += .05f - proximity: mouseX += -.05f - proximity;

		mouseY = floor(mouseY * 100.0f)/100.0f;
		proximity = fmod( mouseY, .05f );
		if( abs(proximity) < .025f )
			mouseY -= proximity;
		else
			mouseY >= 0 ? mouseY += .05f - proximity: mouseY += -.05f - proximity;
		drawWidth = -(clickLoc.x - mouseX);
		drawCenter.x = clickLoc.x - (clickLoc.x - mouseX)/2;

		drawHeight = -(clickLoc.y - mouseY);
		drawCenter.y = clickLoc.y - (clickLoc.y - mouseY)/2;
	}
}

ground *checkSelected( baseObject::vertex loc )
{
	for( list<ground>::iterator objItr = groundObjs.begin(); objItr != groundObjs.end(); ++objItr )
	{
		if( collision::inObject( loc, *objItr ) )
			return &(*objItr);
	}
	return NULL;
}

baseObject *checkSelectedMenu( baseObject::vertex loc )
{
	for( list<actor>::iterator objItr = ++(menuItems.begin()); objItr != menuItems.end(); ++objItr )
	{
		if( collision::inObject( loc, *objItr ) )
			return &(*objItr);
	}
	return NULL;
}

void mouse(int btn, int state, int x, int y)
{
	baseObject::vertex center;
	passiveMouse(x, y);
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN) 
	{
		clickLoc.x = mouseX;
		clickLoc.y = mouseY;
		if( bDrawMenu )
		{
			baseObject::vertex adjusted( clickLoc );
			adjusted.x -= player->origin.x;
			baseObject *selected = checkSelectedMenu( adjusted );
			if( selected != NULL && selected->texture == tSave )
			{
				OPENFILENAME fm;
				getFileWin( fm );
				char str[260];
				strcpy(str, fm.lpstrFile);
				levelReadWrite::writeLevel( str, backgroundObjs, foregroundObjs, groundObjs );
			}
			else if( selected != NULL && selected->texture == tLoad )
			{
				OPENFILENAME fm;
				getFileWin( fm );
				char str[260];
				strcpy(str, fm.lpstrFile);
				string temp = levelReadWrite::readLevel( str, backgroundObjs, foregroundObjs, groundObjs );
			}
		}
		else if( bEditing )
		{
			ground *selected = checkSelected( clickLoc );
			if( selected != NULL )
			{
				selected->bSelected = !selected->bSelected;
				selected->bIsPlatform = !selected->bIsPlatform;
			}

			mouseX = floor(mouseX * 100.0f)/100.0f;
			float proximity = fmod( mouseX, .05f );
			if( abs(proximity) < .025f )
				mouseX -= proximity;
			else
				mouseX >= 0 ? mouseX += .05f - proximity: mouseX += -.05f - proximity;

			mouseY = floor(mouseY * 100.0f)/100.0f;
			proximity = fmod( mouseY, .05f );
			if( abs(proximity) < .025f )
				mouseY -= proximity;
			else
				mouseY >= 0 ? mouseY += .05f - proximity: mouseY += -.05f - proximity;

			clickLoc.x = mouseX;
			clickLoc.y = mouseY;
		}
	}
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_UP) 
	{
		if( bEditing )
		{
			bDrawOutline = false;

			drawCenter.x = clickLoc.x + drawWidth/2;
			drawCenter.y = clickLoc.y + drawHeight/2;
			//prevent accidental creating
			if( abs(drawWidth) > .025 && abs(drawHeight) > .025 )
			{
				groundObjs.push_back( baseObject( drawCenter, abs(drawWidth), abs(drawHeight) ) );
				drawWidth = drawHeight = 0.0f;
			}
		}
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
	tSlide = loadTexture("../Assets/Textures/character/slide/1.png");
	tLoad = loadTexture("../Assets/Textures/load.png");
	tSave = loadTexture("../Assets/Textures/save.png");

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
	glutSetCursor(GLUT_CURSOR_NONE);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc( passiveMouse );
	glutMotionFunc( updateOutline );
	glutIdleFunc(idleFunction);

	loadTextures();

	//initialize needed data
	initKeyMap();
	initMenu();
	initPlayer();	
	initObjects();

	glutMainLoop();
	return 0;
}