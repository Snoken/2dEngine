#define _CRT_SECURE_NO_DEPRECATE
#include "soil\SOIL.h"

#include <Windows.h>
#include <direct.h>

//#include "fmod.hpp"

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
list<baseObject> menuItems; //actors so they can be moved
list<baseObject> overlay;
map<int, bool> keyMap;
bool running = false, bEditing = false;
//FMod Stuff
//FMOD::System     *fSystem; //handle to FMOD engine
//FMOD::Sound      *soundJump, *soundMusic, *soundRun; //sound that will be loaded and played
//FMOD::Channel	 *runChan = NULL;

//global vars
int width(1600), height(900);
actor *player = NULL;
float aspect = (float)width/height, trans = 0.0f, mouseX, 
	mouseY, drawWidth = 0.0f, drawHeight = 0.0f;
long double elapsed, prevElapsed, musicStart;
GLuint tSky, tSkyLower, tDirt, tPaused, tSave, tLoad, tSlide, tCheck;
vector<GLuint> tCharStand, tCharRun, tCharJump, tCharRoll, 
	tWallsStone, tWallsWood;
float wallDistance = 0.0f;
bool bDrawMenu, bDrawOutline;
baseObject::vertex clickLoc, drawCenter;
ground *selected = NULL;

#define SPACEBAR 32
#define ESC 27
#define DEL 127

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

	//basic motion
	keyMap.insert( make_pair( 'w', false ));
	keyMap.insert( make_pair( SPACEBAR, false ));
	keyMap.insert( make_pair( 'a', false ));
	keyMap.insert( make_pair( 's', false ));
	keyMap.insert( make_pair( 'd', false ));

	//drop down through platform
	keyMap.insert( make_pair( 'q', false ));

	//toggle editor
	keyMap.insert( make_pair( 'o', false ));

	keyMap.insert( make_pair( ESC, false ));
	keyMap.insert( make_pair( DEL, false ));

}

//glut needs this even if it's empty
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

	/*list<baseObject::vertex> verts;
	verts.push_back( baseObject::vertex( .1f, -.9f ) );
	verts.push_back( baseObject::vertex( .1f, -.7f ) );
	verts.push_back( baseObject::vertex( -.1f, -.9f ) );
	groundObjs.push_back( baseObject(
		baseObject::vertex( 0.0f, -0.8f ), verts ) );*/

	/*groundObjs.push_back( baseObject( 
		baseObject::vertex( 1.625f, -.775f ), .25f, .25f, tWallsStone[rand() % 5] ) );*/
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

void initOverlay()
{
	float gap = .015f, sqDim = .09f, startX = 0, startY = .775f;
	startX = aspect-gap;
	vector<GLuint> *curr;
	for( int i = 0; i < 2; ++i )
	{
		if( i != 1 )
			curr = &tWallsStone;
		else
			curr = &tWallsWood;
		float currY = startY;
		for( int j = 0; (unsigned)j < curr->size(); ++j )
		{	
			overlay.push_back( baseObject( baseObject::vertex( 
				startX-(sqDim/2), currY + .05f ), sqDim, sqDim, (*curr)[j] ) );
			currY -= sqDim + .005f;
		}
		startX -= sqDim + .005f;
	}

	//boxes for rotation selection, textures are set to angle*10
	overlay.push_back( baseObject( baseObject::vertex( 
				aspect-.344f, .34f + .025f ), .05f, .05f, 3600 ) );

	overlay.push_back( baseObject( baseObject::vertex( 
				aspect-.256f, .34f + .025f ), .05f, .05f, 900 ) );

	overlay.push_back( baseObject( baseObject::vertex( 
				aspect-.163f, .34f + .025f ), .05f, .05f, 1800 ) );

	overlay.push_back( baseObject( baseObject::vertex( 
				aspect-.0635f, .34f + .025f ), .05f, .05f, 2700 ) );

	//platform toggle box, texture used to identify object
	overlay.push_back( baseObject( baseObject::vertex( 
				aspect-.344f, .215f + .025f ), .035f, .035f, 290) );
}

void drawMenu()
{
	list<baseObject>::iterator itr = menuItems.begin();
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
	glDisable( GL_TEXTURE_2D );
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
	/*glBindTexture( GL_TEXTURE_2D, tSkyLower );
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f); glVertex2f(-aspect+player->origin.x,-0.9f);
	glTexCoord2f(0.0f,1.0f); glVertex2f(-aspect+player->origin.x,-0.8f);
	glTexCoord2f(aspect/0.1f,1.0f); glVertex2f(aspect+player->origin.x,-0.8f);
	glTexCoord2f(aspect/0.1f,0.0f); glVertex2f(aspect+player->origin.x,-0.9f);
	glEnd();
	glDisable( GL_TEXTURE_2D );*/
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
	if( !player->m_bOnGround )
	{
		if( player->m_bOnWall )
			glBindTexture( GL_TEXTURE_2D, tSlide );
		else
			glBindTexture( GL_TEXTURE_2D, tCharJump[(int)floor(player->m_frame)] );
	}
	else
	{
		if( player->m_state == actor::RUNNING )
		{
			if( (unsigned int)floor(player->m_frame) > tCharRun.size() - 1 )
				player->m_frame = 0;
			glBindTexture( GL_TEXTURE_2D, tCharRun[(int)floor(player->m_frame)] );
		}
		else if( player->m_state == actor::ROLLING )
		{
			if( (unsigned int)floor(player->m_frame) > tCharRoll.size() - 1 )
				player->m_frame = 0;
			glBindTexture( GL_TEXTURE_2D, tCharRoll[(int)floor(player->m_frame)] );
		}
		else if( player->m_state == actor::IDLE )
		{
			if( (unsigned int)floor(player->m_frame) > tCharStand.size() - 1 )
				player->m_frame = 0;
			glBindTexture( GL_TEXTURE_2D, tCharStand[(int)floor(player->m_frame)] );
		}
	}

	glBegin(GL_QUADS);
	//draw textures appropriately based on movement direction
	baseObject::vertex center = player->origin;
	float w = player->width, h = player->height;
	//compensate for changed player dimensions if rolling
	//NOTE: This is a hack, fix this
	if( player->m_bIsRolling )
	{
		center.y = player->yMin + h;
		w*=2; h*=2;
	}
	if( player->m_bFacingRight )
	{
		glTexCoord2f(0.0f,0.0f); glVertex2f(center.x - h/2, player->yMin -.01f);
		glTexCoord2f(0.0f,1.0f); glVertex2f(center.x - h/2, player->yMin + h);
		glTexCoord2f(1.0f,1.0f); glVertex2f(center.x + h/2, player->yMin + h);
		glTexCoord2f(1.0f,0.0f); glVertex2f(center.x + h/2, player->yMin -.01f);
	}
	else
	{
		glTexCoord2f(1.0f,0.0f); glVertex2f(center.x - h/2, player->yMin -.01f);
		glTexCoord2f(1.0f,1.0f); glVertex2f(center.x - h/2, player->yMin + h);
		glTexCoord2f(0.0f,1.0f); glVertex2f(center.x + h/2, player->yMin + h);
		glTexCoord2f(0.0f,0.0f); glVertex2f(center.x + h/2, player->yMin -.01f);
	}
	glEnd();
	glDisable( GL_TEXTURE_2D );

	//draw health bar
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
	for( list<ground>::reverse_iterator objItr = groundObjs.rbegin(); objItr != groundObjs.rend(); ++objItr )
	{
		glEnable( GL_TEXTURE_2D );
		if( objItr->texture == 0 )
			objItr->texture = tWallsStone[rand() % 5];

		glBindTexture( GL_TEXTURE_2D, objItr->texture );
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5,0.5,0.0);
		glRotatef(objItr->texRotation,0.0,0.0,1.0);
		glTranslatef(-0.5,-0.5,0.0);
		glMatrixMode(GL_MODELVIEW);
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
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBegin(GL_LINES);
	glVertex2f(mouseX - .025f, mouseY);
	glVertex2f(mouseX + .025f, mouseY);

	glVertex2f(mouseX, mouseY - .025f);
	glVertex2f(mouseX, mouseY + .025f);
	glEnd();
}

void drawOverlay()
{
	float gap = .015f, sqWidth = .09f, startX = 0, 
		overWidth = .505f, startY = .395f, offset = player->origin.x;
	glColor4f(0,0,0,0.9f);
	glBegin(GL_QUADS);
	glVertex2f(aspect+offset, 1);
	glVertex2f(aspect+offset, -1);
	glVertex2f(aspect-overWidth+offset, -1);
	glVertex2f(aspect-overWidth+offset, 1);

	//sidebar
	glColor4f(1,1,1,1);
	glVertex2f(aspect-overWidth+offset, 1);
	glVertex2f(aspect-overWidth+offset, -1);
	glVertex2f(aspect-overWidth-.005f+offset, -1);
	glVertex2f(aspect-overWidth-.005f+offset, 1);

	//white backdrop
	/*glColor4f(1,1,1,1);
	glVertex2f(aspect-.01f+offset,startY);
	glVertex2f(aspect-.49f+offset,startY);
	glVertex2f(aspect-.49f+offset,startY+.48f);
	glVertex2f(aspect-.01f+offset,startY+.48f);*/
	glEnd();

	glColor4f(1,1,1,1);
	string temp = "Textures";
	glRasterPos2f(aspect-.325f+offset,.925f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)temp.c_str());

	temp = "Stone";
	glRasterPos2f(aspect-.0925f+offset,.885f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char*)temp.c_str());

	temp = "Wood";
	glRasterPos2f(aspect-.1825f+offset,.885f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_10, (unsigned char*)temp.c_str());

	glEnable(GL_TEXTURE_2D);
	startX = aspect-gap;
	list<baseObject>::iterator itr = overlay.begin();
	for( int i = 0; (unsigned)i < tWallsStone.size() + tWallsWood.size(); ++i, ++itr )
	{
		glBindTexture( GL_TEXTURE_2D, itr->texture );
		glBegin(GL_QUADS);
		glTexCoord2f(0,0); glVertex2f(itr->xMin+offset,itr->yMin);
		glTexCoord2f(0,1); glVertex2f(itr->xMin+offset,itr->yMax);
		glTexCoord2f(1,1); glVertex2f(itr->xMax+offset,itr->yMax);
		glTexCoord2f(1,0); glVertex2f(itr->xMax+offset,itr->yMin);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	temp = "Rotation:";
	glRasterPos2f(aspect-.485f+offset,.355f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*)temp.c_str());

	while (itr != overlay.end())
	{
		glBegin(GL_QUADS);
		glColor4f(1,1,1,1);
		glVertex2f(itr->xMin+offset,itr->yMin);
		glVertex2f(itr->xMin+offset,itr->yMax);
		glVertex2f(itr->xMax+offset,itr->yMax);
		glVertex2f(itr->xMax+offset,itr->yMin);
		glEnd();
		if( itr->texture == 290 )
		{	
			if( selected != NULL && selected->bIsPlatform )
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture( GL_TEXTURE_2D, tCheck );
				glBegin(GL_QUADS);
				glTexCoord2f(0,0); glVertex2f(itr->xMin+offset-.01f,itr->yMin-.01f);
				glTexCoord2f(0,1); glVertex2f(itr->xMin+offset-.01f,itr->yMax+.01f);
				glTexCoord2f(1,1); glVertex2f(itr->xMax+offset+.01f,itr->yMax+.01f);
				glTexCoord2f(1,0); glVertex2f(itr->xMax+offset+.01f,itr->yMin-.01f);
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
		}
		++itr;
	}

	glColor4f(0,0,0,1);
	temp = "0";
	glRasterPos2f(aspect-.35f+offset,.355f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*)temp.c_str());

	temp = "90";
	glRasterPos2f(aspect-.27f+offset,.355f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*)temp.c_str());

	temp = "180";
	glRasterPos2f(aspect-.185f+offset,.355f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*)temp.c_str());

	temp = "270";
	glRasterPos2f(aspect-.085f+offset,.355f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*)temp.c_str());

	glColor4f(1,1,1,1);
	temp = "Properties";
	glRasterPos2f(aspect-.325f+offset,.275f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)temp.c_str());

	temp = "Platform?";
	glRasterPos2f(aspect-.485f+offset,.23f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*)temp.c_str());
}

void redraw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//each draw call is essentially a layer, the first call is the furthest back
	drawSky();
	drawBackground();
	drawGround();
	if( bEditing )
		drawGrid();
	drawPlayer();
	drawForeground();
	if( bDrawOutline )
		drawOutline();
	if( bEditing )
		drawOverlay();
	if( bDrawMenu )
		drawMenu();
	drawCursor();
	
	glutSwapBuffers();
}

void tryDelete()
{
	for(list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
	{
		if( itr->bSelected && itr != groundObjs.begin())
		{
			groundObjs.remove(*itr);
			return;
		}
	}
}

void keyUp(unsigned char key, int x, int y)
{
	//adjust to lower case if needed
	if( key >= 'A' && key <= 'Z' )
		key += 32;
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
		if( player->getMult() != 0.0 && player->m_bOnGround)
		{
			//if( runChan == NULL )
				//fSystem->playSound(FMOD_CHANNEL_FREE, soundRun, false, &runChan);
		}
		else
		{
			//if( runChan != NULL )
			//	runChan->stop();
			//runChan = NULL;
		}
		player->updateLocation( elapsed, belowPlayer, abovePlayer, &nearby, &keyMap );
	}
}

void keyPress(unsigned char key, int x, int y)
{
	//adjust to lower case if needed
	if( key >= 'A' && key <= 'Z' )
		key += 32;
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
		if( bEditing && keyMap.find(DEL)->second == true )
			tryDelete();
		if (keyMap.find('a')->second == false && keyMap.find('d')->second == false && !player->m_bIsRolling )
			//decay the multiplier if no keys are being pressed
			player->decayMult();
		if (keyMap.find('s')->second == true)
		{
			if(player->m_bOnGround && !player->m_bIsRolling && player->isMoving() )
				player->startRoll( elapsed );
		}
		else if (keyMap.find('a')->second == true)
		{
			if(!player->m_bOnWall)
				player->m_bFacingRight = false;
			player->updateMult();
		}
		else if (keyMap.find('d')->second == true)
		{
			if(!player->m_bOnWall)
				player->m_bFacingRight = true;
			player->updateMult();
		}
		if (keyMap.find('w')->second == true || keyMap.find(SPACEBAR)->second == true)
			if( player->m_bOnGround )
			{
				//runChan->getChannelGroup(&runChan);
				//fSystem->playSound(soundJump, 0, false, &runChan);
				player->jump();
			}
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
//	fSystem->update();
}

void initPlayer()
{
	//square orig
	player = new actor( actor( baseObject::vertex( 0.0f, 0.0f ), .2f*(2.0f/3.0f), .195f));
}

void initSounds()
{
	/*//init FMOD
	FMOD::System_Create(&fSystem);// create an instance of the game engine
	fSystem->init(32, FMOD_INIT_NORMAL, 0);// initialise the game engine with 32 channels

	//load sounds
	fSystem->createSound("../Assets/Sounds/jump.wav", FMOD_HARDWARE, 0, &soundJump);
	soundJump->setMode(FMOD_LOOP_OFF);

	fSystem->createSound("../Assets/Sounds/run.mp3", FMOD_HARDWARE, 0, &soundRun);
	soundRun->setMode(FMOD_LOOP_OFF);

	fSystem->createSound("../Assets/Sounds/ambient.mp3", FMOD_HARDWARE, 0, &soundMusic);
	soundMusic->setMode(FMOD_LOOP_NORMAL);*/
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
	if( bEditing )
	{
		passiveMouse( x, y );
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
		{
			if( &(*objItr) == selected )
			{
				objItr->bSelected = false;
				return NULL;
			}
			else
			{
				if( selected != NULL )
					selected->bSelected = false;
				objItr->bSelected = true;
				return &(*objItr);
			}
		}
	}
	if(selected == NULL) 
		return NULL;
	else
		return selected;
}

baseObject *checkSelectedMenu( baseObject::vertex loc )
{
	for( list<baseObject>::iterator objItr = ++(menuItems.begin()); objItr != menuItems.end(); ++objItr )
	{
		if( collision::inObject( loc, *objItr ) )
			return &(*objItr);
	}
	return NULL;
}

baseObject *checkSelectedOverlay( baseObject::vertex loc )
{
	for( list<baseObject>::iterator objItr = overlay.begin(); objItr != overlay.end(); ++objItr )
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
		else
		{
			selected = checkSelected( clickLoc );
			if( bEditing )
			{
				baseObject::vertex adjusted( clickLoc );
				adjusted.x -= player->origin.x;
				baseObject *overlaySelected = checkSelectedOverlay( adjusted );
				if( overlaySelected != NULL )
				{
					for( list<ground>::iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
					{
						if( itr->bSelected )
						{
							if( overlaySelected->texture == 290 && selected != NULL )
								itr->bIsPlatform = !itr->bIsPlatform;
							else if( overlaySelected->texture >= 900 )
								itr->texRotation = (float)(overlaySelected->texture/10 % 360);
							else
								itr->texture = overlaySelected->texture;
						}
					}
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
	tDirt = loadTexture("../Assets/Textures/test.png");
	tSky = loadTexture("../Assets/Textures/sky.jpg");
	tSkyLower = loadTexture("../Assets/Textures/skyLower.jpg");
	tPaused = loadTexture("../Assets/Textures/paused.png");
	tSlide = loadTexture("../Assets/Textures/character/slide/1.png");
	tLoad = loadTexture("../Assets/Textures/load.png");
	tSave = loadTexture("../Assets/Textures/save.png");
	tCheck = loadTexture("../Assets/Textures/check.png");

	for( int i = 1; i <= 4; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/character/stand/";
		location.append(num);
		location.append(".png");
		tCharStand.push_back(loadTexture(location.c_str()));
	}
	for( int i = 1; i <= 8; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/character/walk/";
		location.append(num);
		location.append(".png");
		tCharRun.push_back(loadTexture(location.c_str()));
	}
	for( int i = 1; i <= 5; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/character/jump/";
		location.append(num);
		location.append(".png");
		tCharJump.push_back(loadTexture(location.c_str()));
	}
	for( int i = 1; i <= 4; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/character/roll/";
		location.append(num);
		location.append(".png");
		tCharRoll.push_back(loadTexture(location.c_str()));
	}
	for( int i = 1; i <= 5; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/walls/stone/";
		location.append(num);
		location.append(".png");
		tWallsStone.push_back(loadTexture(location.c_str()));
	}
	for( int i = 1; i <= 4; ++i )
	{
		char *num = new char;
		_itoa( i, num, 10);
		string location = "../Assets/Textures/walls/wood/";
		location.append(num);
		location.append(".png");
		tWallsWood.push_back(loadTexture(location.c_str()));
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
	initOverlay();
	//initSounds();
	//fSystem->playSound(FMOD_CHANNEL_FREE, soundMusic, false, 0);

	glutMainLoop();
	return 0;
}