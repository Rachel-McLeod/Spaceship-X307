#pragma once
#ifndef BLOCK_H
#define BLOCK_H
#include "Actor.h"

class Block : public Actor
{
public :
	Block(ngl::Vec3 position);
	int	 Block::BlockMove(int Score);
};


#endif // !BLOCK_H
