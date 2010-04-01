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
#include "NosePointer.h"

class AutonomousVisionMode : public RobotMode
{
public:
	AutonomousVisionMode(RobotResources& resources, Kicker& kicker, Vision &vision, NosePointer &nosePointer, PositionInformation &position);
	
	void Main();
	
	void OnEnable();

		
	void OnDisable();
		
private:

	enum AutoState {
		GO_FORWARD,
		ALIGN_ROBOT_WITH_TARGET,
		WAIT_FOR_TARGET_ALIGN_TO_FINISH,
		WAIT_FOR_KICK_TO_COMPLETE,
		ALIGN_ROBOT_TO_ZERO,
		WAIT_FOR_ZERO_ALIGN_TO_FINISH,
		STOP
	} m_state ;

	RobotResources& m_resources;
	Kicker& m_kicker;
	Vision& m_vision;
	NosePointer& m_nosePointer;
	PositionInformation &m_position;
	double m_positionX, m_positionY, m_initialY, m_angle;
	
	Timer state_timer, m_timeout;
	
	const static double Y_LIMIT = 100.0;
	const static double TIME_FOR_ALIGN = 1.0;
	const static int DIGITAL_1BALL = 6;
	const static int DIGITAL_2BALLS = 7;
	const static int DIGITAL_3BALLS = 8;
	int m_balls_kicked;
	int m_balls;
	
	double m_stop_timeout;
	
	AutonomousVisionMode();
	DISALLOW_COPY_AND_ASSIGN(AutonomousVisionMode);
	
};

#endif
