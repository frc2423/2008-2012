/**
	\file 		AutonomousVisionMode.cpp
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$

	Copyright (c) 2010 Amory Galili, Dustin Spicuzza ("the authors")
	All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the authors nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "AutonomousVisionMode.h"
#include "WPILib.h"
#include <math.h>

// define this if you want to use vision, or comment
// it out if you want to just do it dead
//#define USE_VISION

AutonomousVisionMode::AutonomousVisionMode(
	RobotResources& resources, 
	Kicker& kicker, 
	Vision& vision, 
	NosePointer &nosePointer, 
	PositionInformation &position
):
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
	m_stop_timeout(5.0)
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
	
	// these statements let us select how many balls we want to kick
	
	if(DriverStation::GetInstance()->GetDigitalIn(DIGITAL_1BALL))
	{
		m_balls = 1;
		m_stop_timeout = 7.0;
	}
	else if(DriverStation::GetInstance()->GetDigitalIn(DIGITAL_2BALLS))
	{
		m_balls = 2;
		m_stop_timeout = 9.0;
	}
	else if(DriverStation::GetInstance()->GetDigitalIn(DIGITAL_3BALLS))
	{
		m_balls = 3;
		m_stop_timeout = 9.0;
	}
	else
	{
		m_balls = 0;
	}
		
	m_timeout.Reset();
	
}

void AutonomousVisionMode::Main()
{	
	// TODO: Need more robust state handling, what happens if we miss a ball? Also
	// need to build in delays a bit better.
	
	double speed = 0.0, turn_rate = 0.0;

#ifndef USE_VISION
	const double curvy_rate = -0.2;
#endif	
	
	m_position.getData(m_positionX, m_positionY, m_angle);
	
	// kill the robot if it has gone too far or if a timeout expires
	if( 
		(m_positionY - m_initialY) > Y_LIMIT ||
		m_timeout.Get() > m_stop_timeout
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
				speed = -0.6;
#ifdef USE_VISION
				turn_rate = m_nosePointer.GetTurnRate( 0.0 );
#else				
				turn_rate = curvy_rate;
#endif
				break;
			}
				
			// this only runs once
#ifdef USE_VISION	
			m_vision.PreferEither();
#endif
			state_timer.Reset();
			
			m_state = ALIGN_ROBOT_WITH_TARGET;
	
		case CONTINUE_FORWARD_TO_GET_BALL_STUCK:
			
			// continue the previous movement until the
			// ball gets stuck in the roller
			if (state_timer.Get() < 0.5)
			{
#ifdef USE_VISION
				turn_rate = m_nosePointer.GetTurnRate( 0.0 );
#else
				turn_rate = curvy_rate;
#endif
				speed = -0.6;
				break;
			}
			
			m_state = ALIGN_ROBOT_WITH_TARGET;
			
		case ALIGN_ROBOT_WITH_TARGET:
		
#ifdef USE_VISION
			// we have the ball, lets try to align ourselves correctly
			if (!m_vision.IsRobotPointingAtTarget())
			{
				turn_rate = m_nosePointer.GetTurnRate( m_vision.GetVisionAngle() );
				break;
			}
#endif
			
			state_timer.Reset();
			m_state = WAIT_FOR_TARGET_ALIGN_TO_FINISH;
			
		case WAIT_FOR_TARGET_ALIGN_TO_FINISH:
		
			if (state_timer.Get() < 0.5 )
			{
#ifdef USE_VISION
				turn_rate = m_nosePointer.GetTurnRate( m_vision.GetVisionAngle() );
#endif
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
	
#ifdef USE_VISION
			
			// we have the ball, lets try to align ourselves correctly
			if (!m_nosePointer.IsPointingCorrectly())
			{
				turn_rate = m_nosePointer.GetTurnRate( 0.0 );
				break;
			}
#endif
			
			state_timer.Reset();
			m_state = WAIT_FOR_ZERO_ALIGN_TO_FINISH;
			
		case WAIT_FOR_ZERO_ALIGN_TO_FINISH:
		
#ifdef USE_VISION

			if (state_timer.Get() < 0.5 )
			{
				turn_rate = m_nosePointer.GetTurnRate( 0.0 );
				break;
			}
#endif	
	
			// are we there yet?
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

