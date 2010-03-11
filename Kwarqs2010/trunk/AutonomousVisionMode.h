/**
	\file 		AutonomousVisionMode.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef AUTONOMOUSVISIONMODE_H
#define AUTONOMOUSVISIONMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"
#include "Kicker.h"
#include "Vision.h"
#include "PositionInformation.h"

class AutonomousVisionMode : public RobotMode
{
public:
	AutonomousVisionMode(RobotResources& resources, Kicker& kicker, Vision &vision, NosePointer &nosePointer, PositionInformation &position, int balls);
	
	void Main();
	
	void OnEnable();
		
	void OnDisable();
		
private:
	enum { STRAIGHTAWAY, STOP_FOR_KICK, ALIGN, BALL_KICK, REALIGN, SEARCH_FOR_BALLS, STOP };
	
	RobotResources& m_resources;
	Kicker& m_kicker;
	Vision& m_vision;
	NosePointer& m_nosePointer;
	PositionInformation &m_position;
	double m_positionX, m_positionY, m_angle;
	
	const static double Y_LIMIT = 10.0;
	int case_Number;
	int balls_kicked;
	int m_balls;
	
	AutonomousVisionMode();
	DISALLOW_COPY_AND_ASSIGN(AutonomousVisionMode);
	
};

#endif
