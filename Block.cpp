#include "Actor.h"
#include "Block.h"
#include "NGLScene.h"
#include <random>

Block::Block(ngl::Vec3 position) : Actor(position)				//blocks start with a randomised z coordinate, just off the top of the screen
{
	m_position.m_x = 7;											//set off the top of the screen to start
	m_position.m_z = (rand() % 10)-(5 - m_width);				//-5 for the size of the grid, width/2 so that the block lines up with the edge
}

int Block::BlockMove(int Score)
{
	m_position.m_x= m_position.m_x-(0.08+ Score / 4000.0);		//gets faster as the score increases

	if (m_position.m_x < -5)									//resets position in x when the block reaches the bottom of the screen
	{
		m_position.m_x = 7;										//x coordinate is set to just off the top of the screen
		Score++;												//the score increases with every block that reaches the bottom of the screen
		m_position.m_z = (rand() % 10) -(5 - m_width);			//the z coordinate is randomized within the bounds of the arena
	}
	return Score;
}