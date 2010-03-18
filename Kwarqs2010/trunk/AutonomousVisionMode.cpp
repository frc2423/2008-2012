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

AutonomousVisionMode::AutonomousVisionMode(RobotResources& resources, Kicker& kicker, Vision& vision, NosePointer &nosePointer, PositionInformation &position, const int balls):
	m_resources(resources),
	m_kicker(kicker),
	m_vision(vision),
	m_nosePointer(nosePointer),
	m_position( position ),
	m_positionX(0.0),
	m_positionY(0.0),
	m_initialY(0.0),
	m_angle(0.0),
	
	m_Speed(0.0),
	m_turnRate(0.0),
	case_Number(STRAIGHTAWAY),
	balls_kicked(0),
	m_balls(balls)
{	
	alignTimer.Start();
}

void AutonomousVisionMode::OnEnable()
{
	balls_kicked = 0;
	case_Number = STRAIGHTAWAY;
	
	double x, a;
	m_position.getData(x, m_initialY, a);
	
}

void AutonomousVisionMode::Main()
{	
	// TODO: Need more robust state handling, what happens if we miss a ball? Also
	// need to build in delays a bit better.
	
	
	m_position.getData(m_positionX, m_positionY, m_angle);
	
	if( (m_positionY - m_initialY) > Y_LIMIT)
	{
		m_Speed = 0.0;
		m_turnRate = 0.0;

		m_resources.myRobot.Drive(m_Speed, m_turnRate);
		return;
	}
	
	switch(case_Number)
	{
	case STRAIGHTAWAY:
		//against the wall, moving in a straight line, kicking
		m_Speed = -0.3;
		m_turnRate = m_nosePointer.GetTurnRate(0.0);
		
		if (m_kicker.HasBall())
		{
			m_Speed = 0.0;
			m_turnRate = 0.0;
			case_Number = STOP_FOR_KICK;
		}
		break;
	case STOP_FOR_KICK:
		//robot stops to kick ball
		m_vision.PreferEither();
		case_Number = ALIGN;
		break;
	case ALIGN:
		// robot aligns with ball
		
		m_Speed = 0.0;
		m_turnRate = m_nosePointer.GetTurnRate(m_vision.GetVisionAngle());
				
		if (m_vision.IsRobotPointingAtTarget()) 
		{
			alignTimer.Reset();
			case_Number = WAIT_FOR_ALIGN;
		}
		break;
	case WAIT_FOR_ALIGN:
		
		m_Speed = 0.0;
		m_turnRate = m_nosePointer.GetTurnRate(m_vision.GetVisionAngle());
		
		//Waits for robot to stop.
		if( alignTimer.Get() < TIME_FOR_ALIGN )
			break;
		
		if( m_vision.IsRobotPointingAtTarget())
			case_Number = BALL_KICK;
		else
			case_Number = ALIGN;
		
		break;
	case BALL_KICK:
		// robot kicks ball
		m_kicker.Kick();
		balls_kicked++;
		case_Number = REALIGN;
		break;
	case REALIGN:
		// robot realigns with next ball
		m_Speed = 0.0;
		m_turnRate = m_nosePointer.GetTurnRate(0.0);
		
		if (m_nosePointer.IsPointingCorrectly()) 
			case_Number = SEARCH_FOR_BALLS;
		break;
	case SEARCH_FOR_BALLS:
		//Go back to case 1 if there are more balls to kick
		if(balls_kicked < m_balls)
		{
			m_Speed = -0.3;
			m_turnRate = m_nosePointer.GetTurnRate(0.0);
			case_Number = STRAIGHTAWAY;
		}
		else
		{
			m_Speed = 0.0;
			m_turnRate = 0.0;
			case_Number = STOP;
		}
		break;
	case STOP:
		m_Speed = 0.0;
		m_turnRate = 0.0;
		break;
	}
	
	m_resources.myRobot.Drive(m_Speed, m_turnRate);
}



void AutonomousVisionMode::OnDisable()
{
}

