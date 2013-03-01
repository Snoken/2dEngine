#include "actor.h"

void actor::move( double multiplier )
{
	GLfloat difX = runSpeed * (GLfloat)multiplier/15;
	this->origin.x += difX;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += difX;
	updateMaxMin();
}

void actor::moveX( float distance )
{
	GLfloat difX = distance;
	this->origin.x += difX;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->x += difX;
	updateMaxMin();
}

void actor::moveY( float distance )
{
	GLfloat difY = distance;
	this->origin.y += difY;
	for( list<vertex>::iterator itr = points.begin(); itr != points.end(); ++itr )
		itr->y += difY;
	updateMaxMin();
}

ground *actor::getCurrentGround( list<ground> *groundObjs )
{
	//figure out which ground is below given actor
	float lowestDif = 999.99f;
	ground *belowPlayer = NULL;
	for( list<ground>::iterator itr = groundObjs->begin(); itr != groundObjs->end(); ++itr )
	{
		/*if( collision::above( one, *itr ) && one.yMin - itr->yMax < lowestDif )
		{
			lowestDif = one.yMin - itr->yMax;
			belowPlayer = &(*itr);
		}*/
	}
	return belowPlayer;
}

//void actor::updateLocation( const long double & elapsed, double multiplier )
//{
//	//figure out which ground object the player is currently above
//	actor *temp;
//	ground *belowPlayer = getCurrentGround( *player );
//	float maxDistance = 0.5f;
//	list<ground> nearby;
//	
//	getNearbyWalls( *player, maxDistance, nearby );
//
//	if( player->bOnWall && slidingOn->yMin >= player->yMin )
//	{
//		player->bOnWall = false;
//		slidingOn = NULL;
//	}
//
//	if( slidingOn != NULL )
//	{
//		if( !player->bOnWall )
//			facingRight = !facingRight;
//		player->bOnWall = true;
//		fallStart = player->origin.y;
//	}
//
//	if( player->bOnWall && (keyMap.find('w'))->second == true )
//	{
//		facingRight ? multiplier = 1.0f: multiplier = -1.0f;
//		player->bOnWall = false;
//		player->vertSpeed = 3.0f;
//		slidingOn = NULL;
//	}
//	else if( player->bOnWall && (keyMap.find('d'))->second == true && facingRight )
//	{
//		player->bOnWall = false;
//		slidingOn = NULL;
//	}
//	else if( player->bOnWall && (keyMap.find('a'))->second == true && !facingRight )
//	{
//		player->bOnWall = false;
//		slidingOn = NULL;
//	}
//
//	if( player->vertSpeed < 0.0f && fallStart == 999.99f )
//		fallStart = player->origin.y;
//
//	if( player->yMin - belowPlayer->yMax > .005 )
//		player->bOnGround = false;
//
//	//if the player is currently in the air, apply gravity
//	if( !player->bOnGround )
//	{
//		temp = new actor(*player);
//		//apply gravity to copy to make sure they don't fall through world
//		if( player->bOnWall )
//			temp->moveY( temp->slideSpeed * (float)elapsed );
//		else
//			physics::applyGravity( temp, elapsed );
//		//if next iter of motion still leaves player above ground, do it			
//		if(collision::timeToCollisionY( *temp, *belowPlayer ) > 0)
//		{
//			player=temp;
//			timeToImpact = collision::timeToCollisionY( *temp, *belowPlayer );
//		}
//		//otherwise, move player just enough to be on ground
//		else
//		{
//			physics::moveByTimeY( player, timeToImpact );
//			fallEnd = player->origin.y;
//			player->takeFallDamage( fallStart-fallEnd );
//			fallStart = 999.99f;
//
//		}
//	}
//
//	if( multiplier != 0.0f && !player->bOnWall )
//	{
//		//check if there is a wall within 1.0f
//		if( nearby.empty() )
//		{
//			player->move( multiplier );
//		}
//		//try moving
//		else
//		{
//			temp = new actor(*player);
//			bool moved = false;
//			temp->move( multiplier );
//			for( list<ground>::iterator itr = nearby.begin(); itr != nearby.end(); ++itr )
//			{
//				if( player->xMin == itr->xMax )
//				{
//					if( multiplier < 0)
//					{
//						multiplier = 0.0f;
//						moved = true;
//						break;
//					}
//				}
//				else if( player->xMax == itr->xMin )
//				{
//					if( multiplier > 0)
//					{
//						multiplier = 0.0f;
//						moved = true;
//						break;
//					}
//				}
//				if( collision::areColliding( *temp, *itr ) && !itr->bIsPlatform )
//				{
//					if( !player->bOnGround )
//						slidingOn = &(*itr);
//					if(player->xMin > itr->xMax) 
//						player->moveX( -(player->xMin - itr->xMax) );
//					else
//						player->moveX( itr->xMin - player->xMax );
//					multiplier = 0.0f;
//					moved = true;
//					break;
//				}
//			}
//			if( !moved )
//				player = temp;
//		}
//	}
//}