/**
	\file 		KwarqsWheelMotor.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

/*
 * Copyright (c) 2009 Dustin Spicuzza
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Dustin Spicuzza nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DUSTIN SPICUZZA ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL DUSTIN SPICUZZA BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 
#include <WPILib.h>

#include "KwarqsWheelMotor.h"
#include "KwarqsConstants.h"


/**
	\brief Constructor
	
	@param slot					Slot for the PWM and encoder
	@param pwm_port				PWM port for motor
	@param encoder_port1		Encoder port
	@param encoder_port2		Encoder port
	@param invert_motor			Set to true if invert the motor
*/
KwarqsWheelMotor::KwarqsWheelMotor(
		UINT32 slot, 
		UINT32 pwm_port, 
		UINT32 encoder_port1, UINT32 encoder_port2,
		bool invert_motor,
		bool invert_encoder) :
	m_motor(slot, pwm_port),
	m_encoder(slot, encoder_port1, slot, encoder_port2, false, Encoder::k1X),
	m_invert(invert_motor ? -1.0F : 1.0F),
	m_lastSpeed(0),
	m_lastUpdate(GetTime()),
{
	SetSpeed(0, 0);
	
	if (pwm_port == PWM_L_JAGUAR)
		m_line = DriverStationLCD::kUser_Line3;
	else
		m_line = DriverStationLCD::kUser_Line4;
	
	m_encoder.SetDistancePerPulse(.0019);
	m_encoder.SetReverseDirection(invert_encoder);
	
	m_encoder.Start();
}

/// Set the speed of the motor (-1 to 1)? 
void KwarqsWheelMotor::SetSpeed(float desired_speed, double velocity)
{
	const float input_speed = desired_speed;

	if (GetTime() - m_lastUpdate > 0.025)
	{
		const double encoder_velocity = m_encoder.GetRate();
		const double encoder_acceleration = encoder_velocity/m_encoder.GetPeriod();
	
		switch (GetBCDInput())
		{
		
		// no traction control
		case 0:
			m_lastSpeed = desired_speed;
			break;
			
		
		// filtered wheel speed (stage 1)
		case 1:
		
			m_lastSpeed = m_lastSpeed + (desired_speed - m_lastSpeed)* 0.1;
			break;
			
		// hard limit on encoder acceleration (stage 2)
		case 2:
		
			if (encoder_acceleration < -MAX_ACCEL)
			{
				m_lastSpeed = m_lastSpeed * .5;
			}
			else if (encoder_acceleration > MAX_ACCEL)
			{
				m_lastSpeed = m_lastSpeed * .5;
			}
			else
			{
				m_lastSpeed = desired_speed;
			}
		
			break;
			
		// the semi-real thing
		case 3:
		
			
			// this should be high to allow for quick response
			#define K_2		.5
			
			// 20% slip is an optimal value according to several sources
			#define SLIP_PCT .2
			
			
			// if we're going too fast
			if (velocity * (1 + SLIP_PCT) < encoder_velocity)
			{
				// if the user wants to go faster, ignore them
				if (desired_speed > m_lastSpeed)
				{
					// go slower
				}
			}
			
			// if we're going too slow
			else if (velocity * (1 - SLIP_PCT) > encoder_velocity)
			{
				// if the user wants to go slower, ignore them
				if (desired_speed < m_lastSpeed)
				{
					// go faster
				}
			}

			// PID style loop thing
			m_lastSpeed = m_lastSpeed + (desired_speed - m_lastSpeed)* K_2;
		
			break;
				
		// the 'real' thing
		case 4:
		
			break;
			
		// the 'ann' thing	
		case 5:
		
			/// TODO: implement this
		
		// motors disabled
		default:
			m_lastSpeed = 0;
		}
	
		// print something out each time around
		DriverStationLCD::GetInstance()->PrintfLine(
			m_line,
			"%.4f %.4f %.2f %.2f", 
			encoder_velocity,
			encoder_acceleration,
			input_speed, 
			m_lastSpeed
		);
	
		m_lastUpdate = GetTime();
	}
	
	m_motor.Set(m_lastSpeed * m_invert);

}

/// Get the speed that the motor was assigned to go via SetSpeed
float KwarqsWheelMotor::GetSetSpeed()
{
	return m_motor.Get();
}

/// Get the actual speed that the wheel is turning (via the encoder)
double KwarqsWheelMotor::GetActualSpeed()
{
	// todo: implement me
	return 0;
}

