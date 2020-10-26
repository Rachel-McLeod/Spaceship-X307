#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "Actor.h"

class Player : public Actor
{
public:
	Player(ngl::Vec3 _pos);
	void SetPosition(ngl::Real _dx, ngl::Real _dz);
	void TakeDamage();
	ngl::Vec3 m_playerStartPos = (-5.0, 0.0, 0.0);
	int m_health = 3;
	bool invincibility = FALSE;
};

#endif // Player.h
