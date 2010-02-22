/**
	\file 		EncoderMode.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
	
	This doesn't work, don't use it
*/


#ifndef ENCODERMODE_H
#define ENCODERMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"
#include "PositionInformation.h"


class EncoderMode: public RobotMode
{
public:
	EncoderMode(RobotResources& resources):
		m_resources(resources),
		m_position(m_resources, WHEEL_BASE),
		m_x(0.0),
		m_y(0.0),
		m_angle(0.0)
	{
		m_position.Start();
	}
		
	void Main()
	{
		m_position.getData(m_x, m_y, m_angle);
		double angle = fmod(m_angle, 360.0);
		double turn_speed = angle / 360.0;
		if(fabs(m_angle) > 0.0)
			m_resources.myRobot.Drive(.2, turn_speed);
	}
	
	void OnEnable()
	{
	
	}
	
	void OnDisable()
	{
		
	}
	
private:
	RobotResources& m_resources;
	PositionInformation m_position;
	double m_x, m_y, m_angle;
	
};

#endif

