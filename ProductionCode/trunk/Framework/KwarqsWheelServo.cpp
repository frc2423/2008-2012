/**
	\file 		KwarqsWheelServo.cpp
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
#include <math.h>

#include "KwarqsConstants.h"
#include "KwarqsWheelServo.h"

/**
	\brief Constructor
	
	@param slot					Slot for the PWM and encoder
	@param pwm_port				PWM port for motor
	@param encoder_port1		Encoder port
	@param encoder_port2		Encoder port 
	@param outputScale			A value used to indicate the max error angle to 
								generate a full-power correctional response
	@param encoderResolution	The number of wheel counts that it takes for the wheel
								to rotate 360 degrees
*/
KwarqsWheelServo::KwarqsWheelServo(
	UINT32 slot,
	UINT32 pwm_port, 
	UINT32 encoder_port1, UINT32 encoder_port2,
	double outputScale,
	int encoderResolution
) :
	m_motor(slot, pwm_port),
	m_encoder(slot, encoder_port1, slot, encoder_port2),
	m_outputScale(outputScale),
	m_encoderResolution((double)encoderResolution)
{

	// create the PID controller
	m_pidController = new TunablePIDController(SERVO_PID_P, SERVO_PID_I, SERVO_PID_D, this, this);
	
	// set the PID parameters
	m_pidController->SetContinuous();
	m_pidController->SetInputRange(0, 360);
	m_pidController->SetOutputRange(-360, 360);
	
	// enable it
	m_pidController->Enable();
	m_encoder.Start();
}

KwarqsWheelServo::~KwarqsWheelServo()
{
	delete m_pidController;
}

/// Set the angle that the wheel should be pointing
void KwarqsWheelServo::SetAngle(double angle)
{
	// do this directly, don't do shortest path via 180 degree turns.. 
	// another layer of the application should do that
	m_pidController->SetSetpoint(fmod(angle, 360.0));
}

/// Get the angle that the wheel is supposed to be pointing
float KwarqsWheelServo::GetSetAngle()
{
	return m_pidController->GetSetpoint();
}

/// Get the angle that the wheel is actually pointing
double KwarqsWheelServo::GetCurrentAngle()
{
	// convert the encoder value to a 0-360 value
	return (fmod(m_encoder.Get(), m_encoderResolution) * 360.0) / m_encoderResolution;
}

// generally you won't need to use this.. 
void KwarqsWheelServo::TuneParameters(float p, float i, float d)
{
	m_pidController->TuneParameters(p, i, d);
}


/// called when the PID Controller completes a calculation
void KwarqsWheelServo::PIDWrite(float output)
{
	// the value received should be in the range of -360, 360 so we should
	// just scale it to a value the victor can take.. but then we would not
	// have a good response time (probably). So, we use the scale parameter
	// to determine the rate of change in relation to the output
	
	if (output > m_outputScale)
		output = 1;
	else if (output < -m_outputScale)
		output = -1;
	else
		output = output/m_outputScale;
	
	// set the motor value
	m_motor.Set(output);
}

double KwarqsWheelServo::PIDGet()
{
	return GetCurrentAngle();
}
