#pragma once
#ifndef ACTOR_H
#define ACTOR_H

#include <QTimer>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>

#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>


class Actor 
{
public:
	Actor(ngl::Vec3 pos);
	void Draw(const char [4], ngl::Mat4 m_project, ngl::Mat4 m_view, std::string model, float SpeedZ);
	
	float flame = 0.0;
	bool Increasing = TRUE;
	ngl::Vec4 m_colour = (0.2);
	ngl::Vec3 m_position;
	
	float m_width = 1.0f;
	float m_height = 0.4;

protected:
	

};
#endif