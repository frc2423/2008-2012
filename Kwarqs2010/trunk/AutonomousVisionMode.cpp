/**
	\file 		AutonomousVisionMode.cpp
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#include "AutonomousVisionMode.h"
#include "WPILib.h"
#include <math.h>

#define AUTONOMOUSVISION_P 0.1
#define AUTONOMOUSVISION_I 0.0
#define AUTONOMOUSVISION_D 0.5

AutonomousVisionMode::AutonomousVisionMode(RobotResources& resources, Kicker& kicker, Vision& vision, const int balls):
	m_resources(resources),
	m_kicker(kicker),
	m_vision(vision),
	case_Number(STRAIGHTAWAY),
	balls_kicked(0),
	m_balls(balls)
{	
}

void AutonomousVisionMode::OnEnable()
{

}

void AutonomousVisionMode::Main()
{
	// TODO: Need to adjust this to make sure we don't go too far in the Y direction
	
	// TODO: Need more robust state handling, what happens if we miss a ball? Also
	// need to build in delays a bit better.
	
	// TODO: Need to share the PID controller with the vision system probably, 
	// after some thought that sounds more attractive. Probably should
	// move it to its own wrapper class and allow different types of access
	// depending on which function is calling it. Or something like that.

	switch(case_Number)
	{
	case STRAIGHTAWAY:
		//against the wall, moving in a straight line, kicking
		m_resources.myRobot.Drive(-0.5, 0.0);
		if (m_kicker.HasBall()) 
			case_Number = STOP_FOR_KICK;
		break;
	case STOP_FOR_KICK:
		//robot stops to kick ball
		m_resources.myRobot.Drive(0.0, 0.0);
		m_vision.PreferEither();
		case_Number = ALIGN;
		break;
	case ALIGN:
		// robot aligns with ball
		if (m_vision.IsRobotPointingAtTarget()) 
		{
			m_vision.DisableMotorControl();
			case_Number = BALL_KICK;
		}
		break;
	case BALL_KICK:
		// robot kicks ball
		m_kicker.Kick();
		balls_kicked++;
		case_Number = REALIGN;
		break;
	case REALIGN:
		// robot realigns with next ball
		double error = m_resources.gyro.GetAngle();
		error = error / 180.0;
		m_resources.myRobot.Drive(-0.2, error);
		if (fabs(error) <= 1.0) 
			case_Number = SEARCH_FOR_BALLS;
		break;
	case SEARCH_FOR_BALLS:
		//Go back to case 1 if there are more balls to kick
		if(balls_kicked < m_balls) 
			case_Number = STRAIGHTAWAY;
		else 
			case_Number = 7;
		break;
	case STOP:
		m_resources.myRobot.Drive(0.0, 0.0);
		break;
	}
}



void AutonomousVisionMode::OnDisable()
{
}

