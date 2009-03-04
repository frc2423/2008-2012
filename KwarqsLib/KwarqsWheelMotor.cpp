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


#include "KwarqsBCDInput.h"
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
	m_invert(invert_motor ? -1.0F : 1.0F)
{
	SetSpeed(0);
	
	//m_encoder.SetDistancePerPulse(.0019);
	m_encoder.SetReverseDirection(invert_encoder);
	
	/// @todo need the encoders for telemetry
	m_encoder.Start();
}

/// Set the speed of the motor (-1 to 1)? 
void KwarqsWheelMotor::SetSpeed(float speed)
{
	m_motor.Set(speed * m_invert);
}

/// Get the speed that the motor was assigned to go via SetSpeed
float KwarqsWheelMotor::GetSetSpeed()
{
	return m_motor.Get();
}

void KwarqsWheelMotor::SetRaw(float speed)
{
	m_motor.Set(speed * m_invert);
}

UINT32 KwarqsWheelMotor::GetRawEncoder()
{
	return m_encoder.Get();
}

float KwarqsWheelMotor::GetRawMotor()
{
	return m_motor.Get();
}

