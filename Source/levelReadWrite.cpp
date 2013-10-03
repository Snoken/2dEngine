#include "levelReadWrite.h"
#include <typeinfo>
#include <iostream>
#include <sstream>

#define NONE -1
enum objGroups {BG, FG, GR};
enum objTypes {BASE, PROP, GROUND};
enum texTypes {STONE/*TODO: add more and add support for texture type*/};

bool levelReadWrite::writeLevel(const char* file, const list<baseObject> &backgroundObjs, const list<prop> &foregroundObjs, const list<ground> &groundObjs)
{
	ofstream out(file);
	if( !out.good() )
		return false;

	if( !backgroundObjs.empty() )
	{
		out << "<backgroundObjs>" << endl;
		for( list<baseObject>::const_iterator itr = backgroundObjs.begin(); itr != backgroundObjs.end(); ++itr )
		{
			out << "\t<" << typeid(*itr).name() << ">" << endl;
			out << "\t\t<origin>(" << itr->origin.x << "," << itr->origin.y << ")</origin>" << endl;
			out << "\t\t<width>" << itr->width << "</width>" << endl;
			out << "\t\t<height>" << itr->height << "</height>" << endl;
			out << "\t\t<color>(" << itr->color[0] << "," << itr->color[1] << "," << itr->color[2] << "," << itr->color[3] << ")</color>" << endl;
			out << "\t\t<textureType>stone</textureType>" << endl;
			out << "\t</" << typeid(*itr).name() << ">" << endl;
		}
		out << "</backgroundObjs>" << endl;
	}

	if( !foregroundObjs.empty() )
	{
		out << "<foregroundObjs>" << endl;
		for( list<prop>::const_iterator itr = foregroundObjs.begin(); itr != foregroundObjs.end(); ++itr )
		{
			out << "\t<" << typeid(*itr).name() << ">" << endl;
			out << "\t\t<origin>(" << itr->origin.x << "," << itr->origin.y << ")</origin>" << endl;
			out << "\t\t<width>" << itr->width << "</width>" << endl;
			out << "\t\t<height>" << itr->height << "</height>" << endl;
			out << "\t\t<color>(" << itr->color[0] << "," << itr->color[1] << "," << itr->color[2] << "," << itr->color[3] << ")</color>" << endl;
			out << "\t\t<textureType>stone</textureType>" << endl;
			out << "\t</" << typeid(*itr).name() << ">" << endl;
		}
		out << "</foregroundObjs>" << endl;
	}

	if( !groundObjs.empty() )
	{
		out << "<groundObjs>" << endl;
		for( list<ground>::const_iterator itr = groundObjs.begin(); itr != groundObjs.end(); ++itr )
		{
			out << "\t<" << typeid(*itr).name() << ">" << endl;
			out << "\t\t<origin>(" << itr->origin.x << "," << itr->origin.y << ")</origin>" << endl;
			out << "\t\t<width>" << itr->width << "</width>" << endl;
			out << "\t\t<height>" << itr->height << "</height>" << endl;
			if( itr->bIsPlatform )
				out << "\t\t<platform>yes</platform>" << endl;
			else
				out << "\t\t<platform>no</platform>" << endl;
			out << "\t\t<color>(" << itr->color[0] << "," << itr->color[1] << "," << itr->color[2] << "," << itr->color[3] << ")</color>" << endl;
			out << "\t\t<textureType>stone</textureType>" << endl;
			out << "\t</" << typeid(*itr).name() << ">" << endl;
		}
		out << "</groundObjs>" << endl;
	}
	out.close();
	return true;
}

string levelReadWrite::readLevel(const char* file, list<baseObject> &backgroundObjs, list<prop> &foregroundObjs, list<ground> &groundObjs)
{
	fstream in(file);
	if( !in.good() )
		return "File failed to open.";

	string line;
	int lineNumber = 1;
	int objGroup = NONE;
	int objType = NONE;
	int textureType = NONE;
	float height = 0.0f, width = 0.0f;
	primitives::vertex origin( 0.0f, 0.0f );
	GLfloat color[4]={1.0f, 1.0f, 1.0f, 1.0f};
	bool bIsPlatform = false;

	//temporary lists so that we can abort if the load goes wrong
	list<baseObject> backgroundObjsTemp;
	list<prop> foregroundObjsTemp;
	list<ground> groundObjsTemp;

	while( getline( in, line ) )
	{
		//initial get of opening tag for group
		if( objGroup == NONE )
		{
			if( line.find("<backgroundObjs>") != string::npos )
				objGroup = BG;
			else if( line.find("<foregroundObjs>") != string::npos )
				objGroup = FG;
			else if( line.find("<groundObjs>") != string::npos )
				objGroup = GR;
			else
				return "Invalid Object Group: line " + lineNumber;
		}
		//get start of object
		else if( objGroup != NONE && objType == NONE )
		{
			if( line.find("ground") != string::npos && objGroup == GR )
				objType = GROUND;
			else if( line.find("prop") != string::npos && objGroup == FG )
				objType = PROP;
			else if( line.find("baseObject") != string::npos && objGroup == BG )
				objType = BASE;
			//if not valid obj type, return failed
			else
				return "Invalid Object Type: line " + lineNumber;
		}
		//get attribute
		else if( objGroup != NONE && objType != NONE && line.find("</class") == string::npos )
		{
			//tag must be opened and closed on line
			if( line.find("<origin>(") != string::npos )
			{
				if( line.find(")</origin>") == string::npos )
					return "Missing closing tag: line " + lineNumber;
				try
				{
					stringstream tokenizer( line.substr( line.find("(")+1, line.find(")") ) );
					tokenizer >> origin.x;
					tokenizer.get();
					tokenizer >> origin.y;
				}
				catch(...)
				{
					return "Invalid input format in origin tag: line " + lineNumber;
				}
			}
			else if( line.find("<width>") != string::npos )
			{
				if( line.find("</width>") == string::npos )
					return "Missing closing tag: line " + lineNumber;
				try
				{
					stringstream tokenizer( line.substr( line.find(">")+1, line.rfind("</") - (line.find(">")+1) ) );
					tokenizer >> width;
				}
				catch(...)
				{
					return "Invalid input format in width tag: line " + lineNumber;
				}
			}
			else if( line.find("<height>") != string::npos )
			{
				if( line.find("</height>") == string::npos )
					return "Missing closing tag: line " + lineNumber;
				try
				{
					stringstream tokenizer( line.substr( line.find(">")+1, line.rfind("</") - (line.find(">")+1) ) );
					tokenizer >> height;
				}
				catch(...)
				{
					return "Invalid input format in height tag: line " + lineNumber;
				}
			}
			else if( line.find("<platform>") != string::npos )
			{
				if( line.find("</platform>") == string::npos )
					return "Missing closing tag: line " + lineNumber;
				if( line.find("yes") != string::npos )
					bIsPlatform = true;
				else if( line.find("no") != string::npos )
					bIsPlatform = false;
				else
					return "Invalid input format in platform tag: line " + lineNumber;					
			}
			else if( line.find("<color>(") != string::npos )
			{
				if( line.find(")</color>") == string::npos )
					return "Missing closing tag: line " + lineNumber;
				try
				{
					stringstream tokenizer( line.substr( line.find(">")+1, line.rfind("</") - (line.find(">")+1) ) );
					tokenizer >> color[0];
					tokenizer.get();
					tokenizer >> color[1];
					tokenizer.get();
					tokenizer >> color[2];
					tokenizer.get();
					tokenizer >> color[3];
				}
				catch(...)
				{
					return "Invalid input format in color tag: line " + lineNumber;
				}				
			}
			else if( line.find("<textureType>") != string::npos )
			{
				if( line.find("</textureType>") == string::npos )
					return "Missing closing tag: line " + lineNumber;
				if( line.find("stone") )
					textureType = STONE;
				else
					return "Invalid texture type: line " + lineNumber;					
			}
			else
				return "Invalid tag: line " + lineNumber;
		}
		else if( objGroup != NONE && objType != NONE && line.find("</class") != string::npos )
		{
			if( width != 0.0f && height != 0.0f )
			{
				//TODO: impl texture type
				if( objType == GROUND && objGroup == GR )
					groundObjsTemp.push_back( ground(baseObject( origin, width, height, color ), bIsPlatform) );
				else if( objType == BASE && objGroup == BG )
					backgroundObjsTemp.push_back( baseObject( origin, width, height, color ) );
				else if( objType == PROP && objGroup == FG )
					foregroundObjsTemp.push_back( baseObject( origin, width, height, color ) );
				else
					return "Error adding object: types do not match.";

				bIsPlatform = false;
				width = height = 0.0f;
				textureType = NONE;
				objType = NONE;
				height = 0.0f, width = 0.0f;
				origin.x = 0.0f;
				origin.y = 0.0f;
				for( int i = 0; i < 4; ++i )
					color[i] = 1.0f;
			}
		}
		else if( line.find( "</groundObjs>") != string::npos || line.find( "</backgroundObjs>") != string::npos 
			|| line.find( "</foregroundObjs>") != string::npos )
			objGroup = NONE;
		else
			return "Invalid format: line " + lineNumber;
		++lineNumber;
	}
	in.close();
	backgroundObjs = backgroundObjsTemp;
	foregroundObjs = foregroundObjsTemp;
	groundObjs = groundObjsTemp;
	return "success";
}