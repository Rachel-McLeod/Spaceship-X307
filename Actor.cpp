#include "Actor.h"
#include "Player.h"
#include "NGLScene.h"

Actor::Actor(ngl::Vec3 pos)
{
	 m_position = pos;
}

void Actor::Draw(const char[4], ngl::Mat4 m_project, ngl::Mat4 m_view, std::string Model, float SpeedZ)
{
	ngl::Mat4 PositionMat;
	PositionMat.translate(m_position.m_x, m_position.m_y, m_position.m_z); //converts the vector to a matrix
	auto shader = ngl::ShaderLib::instance();

	if (Model == "Player")
	{
		if (Increasing)						//"increasing" is a bool that stores whether to increase or decrease the flame value
		{
			flame += 0.02;
		}
		else
		{
			flame -= 0.02;
		}
		if (flame <= 0.0 || flame >= 0.5)	//if the flame value is outside these values, the direction of change is reversed
		{
			Increasing = !Increasing;
		}
		m_width = 0.1f;
		m_height = 0.4f;
		ngl::VAOPrimitives::instance()->createCone("Actor", m_width, m_height, 4, 2);		//the player is comprised of two different cone primitives
		PositionMat.rotateY(90 + SpeedZ * -8);												//the angle from central is changed slightly based on speed
		shader->setUniform("MVP", m_project * m_view * PositionMat);
		ngl::VAOPrimitives::instance()->draw("Actor");

		shader->setUniform("colour", 0.99f, 0.1f+flame*2, 0.99f);							//the flame's colour gradually changes
		ngl::VAOPrimitives::instance()->createCone("Actor", (m_width/2.0) * (1+flame/6.0), (m_height/2.0) * (flame/1.5+1), 4, 2);//the flame's height and width gradually change
		PositionMat.rotateY(-90 + SpeedZ * -8);

	}
	else if (Model == "Block")
	{
		m_width = 0.6f;
		ngl::VAOPrimitives::instance()->createSphere("Actor", m_width, 8);
	}
	else if (Model == "Star")
	{
		ngl::VAOPrimitives::instance()->createSphere("Actor", m_width, 6);		//stars appear much smaller and so can be lower poly
	}

	shader->setUniform("MVP", m_project * m_view * PositionMat);
	ngl::VAOPrimitives::instance()->draw("Actor");								//the actor is drawn
}

