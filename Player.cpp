#include"Actor.h"
#include "Player.h"
#include "NGLScene.h"

Player::Player(ngl::Vec3 position) : Actor(position)		//initialises to the start position
{
	m_position = m_playerStartPos;
}

void Player::SetPosition(ngl::Real _dx, ngl::Real _dz)		//sets the player's position
{
	m_position.m_x = _dx;
	m_position.m_z = _dz;
}

void Player::TakeDamage()									//sets the player's health when damage is taken
{
	m_health--;
}