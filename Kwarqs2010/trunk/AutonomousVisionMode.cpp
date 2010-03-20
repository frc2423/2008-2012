/**
	\file 		AutonomousVisionMode.cpp
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#include "AutonomousVisionMode.h"
#include "WPILib.h"
#include <math.h>


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

	m_balls_kicked(0),
	m_balls(balls)
{	
	state_timer.Start();
}

void AutonomousVisionMode::OnEnable()
{
	m_balls_kicked = 0;
	m_state = GO_FORWARD;
	
	double x, a;
	m_position.getData(x, m_initialY, a);
	
}

void AutonomousVisionMode::Main()
{	
	// TODO: Need more robust state handling, what happens if we miss a ball? Also
	// need to build in delays a bit better.
	
	double speed = 0.0, turn_rate = 0.0;
	
	
	m_position.getData(m_positionX, m_positionY, m_angle);
	
	if( (m_positionY - m_initialY) > Y_LIMIT)
	{
		m_resources.myRobot.Drive(0.0, 0.0);
		return;
	}
	
	switch( m_state )
	{
		case GO_FORWARD:
			
			// if we don't have the ball, keep going forward
			if (!m_kicker.HasBall())
			{
				speed = -0.5;
				turn_rate = m_nosePointer.GetTurnRate( 0.0 );
				break;
			}
				
			// this only runs once
			m_vision.PreferEither();
			
			m_state = ALIGN_ROBOT_WITH_TARGET;
	
		case ALIGN_ROBOT_WITH_TARGET:
		
			// we have the ball, lets try to align ourselves correctly
			if (!m_vision.IsRobotPointingAtTarget())
			{
				turn_rate = m_nosePointer.GetTurnRate( m_vision.GetVisionAngle() );
				break;
			}
			
			state_timer.Reset();
			m_state = WAIT_FOR_TARGET_ALIGN_TO_FINISH;
			
		case WAIT_FOR_TARGET_ALIGN_TO_FINISH:
		
			if (state_timer.Get() < 0.5 )
			{
				turn_rate = m_nosePointer.GetTurnRate( m_vision.GetVisionAngle() );
				break;
			}	
			
			// ok, we're aligned. Lets kick. 
			m_kicker.Kick();
			
			m_state = WAIT_FOR_KICK_TO_COMPLETE;
			
		case WAIT_FOR_KICK_TO_COMPLETE:
		
			// if the kicker is still going, don't do anything
			if (m_kicker.IsKicking())
				break;
			
			m_state = ALIGN_ROBOT_TO_ZERO;
		
		case ALIGN_ROBOT_TO_ZERO:
	
			// we have the ball, lets try to align ourselves correctly
			if (!m_nosePointer.IsPointingCorrectly())
			{
				turn_rate = m_nosePointer.GetTurnRate( 0.0 );
				break;
			}
			
			state_timer.Reset();
			m_state = WAIT_FOR_ZERO_ALIGN_TO_FINISH;
			
		case WAIT_FOR_ZERO_ALIGN_TO_FINISH:
		
			if (state_timer.Get() < 0.5 )
			{
				turn_rate = m_nosePointer.GetTurnRate( 0.0 );
				break;
			}
	
			if (++m_balls_kicked > m_balls)
				m_state = STOP;
			else
				m_state = GO_FORWARD;
	
		case STOP:
			break;
	}
	
	m_resources.myRobot.ArcadeDrive(speed, turn_rate);
}



void AutonomousVisionMode::OnDisable()
{
}

