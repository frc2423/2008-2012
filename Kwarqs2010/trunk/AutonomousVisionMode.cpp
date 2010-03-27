/**
	\file 		AutonomousVisionMode.cpp
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#include "AutonomousVisionMode.h"
#include "WPILib.h"
#include <math.h>


AutonomousVisionMode::AutonomousVisionMode(RobotResources& resources, Kicker& kicker, Vision& vision, NosePointer &nosePointer, PositionInformation &position):
	m_resources(resources),
	m_kicker(kicker),
	m_vision(vision),
	m_nosePointer(nosePointer),
	m_position( position ),
	m_positionX(0.0),
	m_positionY(0.0),
	m_initialY(0.0),
	m_angle(0.0),

	m_balls_kicked(0)
{	
	state_timer.Start();
	m_timeout.Start();
}

void AutonomousVisionMode::OnEnable()
{
	m_balls_kicked = 0;
	m_state = GO_FORWARD;
	
	m_resources.gyro.Reset();
	
	double x, a;
	m_position.getData(x, m_initialY, a);
	
	if(DriverStation::GetInstance()->GetDigitalIn(DIGITAL_1BALL))
		m_balls = 1;
	else if(DriverStation::GetInstance()->GetDigitalIn(DIGITAL_2BALLS))
		m_balls = 2;
	else if(DriverStation::GetInstance()->GetDigitalIn(DIGITAL_3BALLS))
		m_balls = 3;
	else
		m_balls = 0;
		
	m_timeout.Reset();
	
}

void AutonomousVisionMode::Main()
{	
	// TODO: Need more robust state handling, what happens if we miss a ball? Also
	// need to build in delays a bit better.
	
	double speed = 0.0, turn_rate = 0.0;
	
	const double curvy_rate = -0.05;
	
	
	m_position.getData(m_positionX, m_positionY, m_angle);
	
	// kill the robot if it has gone too far or if a timeout expires
	if( 
		(m_positionY - m_initialY) > Y_LIMIT ||
		m_timeout.Get() > 8.0
	)
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
				speed = -0.7;
				turn_rate = curvy_rate;
				//turn_rate = m_nosePointer.GetTurnRate( 0.0 );
				break;
			}
				
			// this only runs once
			//m_vision.PreferEither();
			state_timer.Reset();
			
			m_state = ALIGN_ROBOT_WITH_TARGET;
	
		case ALIGN_ROBOT_WITH_TARGET:
		
			// we have the ball, lets try to align ourselves correctly
			/*if (!m_vision.IsRobotPointingAtTarget())
			{
				turn_rate = m_nosePointer.GetTurnRate( m_vision.GetVisionAngle() );
				break;
			}*/
			
			// keep going until ball is firmly stuck in roller
			if (state_timer.Get() < 0.5)
			{
				turn_rate = curvy_rate;
				speed = -0.7;
			}
			
			state_timer.Reset();
			m_state = WAIT_FOR_TARGET_ALIGN_TO_FINISH;
			
		case WAIT_FOR_TARGET_ALIGN_TO_FINISH:
		
			if (state_timer.Get() < 0.5 )
			{
				//turn_rate = m_nosePointer.GetTurnRate( m_vision.GetVisionAngle() );
				//turn_rate = m_nosePointer.GetTurnRate( 0.0 );
				break;
			}	
			
			// ok, we're aligned. Lets kick. 
			m_kicker.Kick(true);
			
			m_state = WAIT_FOR_KICK_TO_COMPLETE;
			
		case WAIT_FOR_KICK_TO_COMPLETE:
		
			// if the kicker is still going, don't do anything
			if (m_kicker.IsKicking())
				break;
			
			m_state = ALIGN_ROBOT_TO_ZERO;
		
		case ALIGN_ROBOT_TO_ZERO:
	
			// we have the ball, lets try to align ourselves correctly
			//if (!m_nosePointer.IsPointingCorrectly())
			//{
			//	turn_rate = m_nosePointer.GetTurnRate( 0.0 );
			//	break;
			//}
			
			state_timer.Reset();
			m_state = WAIT_FOR_ZERO_ALIGN_TO_FINISH;
			
		case WAIT_FOR_ZERO_ALIGN_TO_FINISH:
		
			//if (state_timer.Get() < 0.5 )
			//{
			//	turn_rate = m_nosePointer.GetTurnRate( 0.0 );
			//	break;
			//}
	
			if (++m_balls_kicked >= m_balls)
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

